#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <mpi.h>

#define COORDINATOR 0

// Dimensión de las matrices
int N;

int resultado_valido(double *D) {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (D[i * N + j] != 0)
				return 0;
		}
	}
	return 1;
}

int main(int argc, char* argv[]) {
	int i, j, k, numProcs, rank, stripSize;
	double *A, *B, *C, *AB, *D, d;

	double maxA_local, maxB_local, maxC_local, minA_local, minB_local, minC_local;
	double totalA_local, totalB_local, totalC_local;

	double maxA, maxB, maxC, minA, minB, minC;
	double totalA, totalB, totalC;
	double avgA, avgB, avgC;

	MPI_Status status;
	double commTimes[8], maxCommTimes[8], minCommTimes[8], commTime = 0, totalTime;


	/* Lee parámetros de la línea de comando */
	if ((argc != 2) || ((N = atoi(argv[1])) <= 0) ) {
		printf("\nUsar: %s size \n  size: Dimension de la matriz\n", argv[0]);
		exit(1);
	}

	/* Inicializar OpenMP */
	omp_set_num_threads(2);

	/* Inicializar MPI */
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (N % numProcs != 0) {
		printf("El tamaño de la matriz debe ser multiplo del numero de procesos.\n");
		exit(1);
	}

	// Porción de cada worker
	stripSize = N / numProcs;

	/* Reserva de memoria */

	// Matrices resultantes
	if (rank == COORDINATOR) {
		A = (double*)malloc(sizeof(double) * N * N);
		D = (double*)malloc(sizeof(double) * N * N);
	}
	else  {
		A = (double*)malloc(sizeof(double) * N * stripSize);
		D = (double*)malloc(sizeof(double) * N * stripSize);
	}

	B = (double*)malloc(sizeof(double) * N * N);
	C = (double*)malloc(sizeof(double) * N * N);

	// Matrices temporales
	AB = (double*)malloc(sizeof(double) * N * stripSize);

	/* Inicializar valores de las matrices en 1 */
	// Se organiza a A por fila
	// Se organiza a B,C por columna
	if (rank == COORDINATOR) {
		for (i = 0; i < N ; i++)
			for (j = 0; j < N ; j++)
				A[i * N + j] = 1;
		for (i = 0; i < N ; i++)
			for (j = 0; j < N ; j++)
				B[j * N + i] = 1;
		for (i = 0; i < N ; i++)
			for (j = 0; j < N ; j++)
				C[j * N + i] = 1;
	}

	totalA_local = totalB_local = totalC_local = 0;

	// Los procesos esperan a que el coordinador termine de inicializar
	MPI_Barrier(MPI_COMM_WORLD);

	/* 1º Comunicación --> Distribución de datos */
	commTimes[0] = MPI_Wtime();

	MPI_Scatter(A, N * stripSize, MPI_DOUBLE, A, N * stripSize, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
	MPI_Bcast(B, N * N, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
	MPI_Bcast(C, N * N, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);

	commTimes[1] = MPI_Wtime();

	/* Procesamiento local */

	maxA_local = minA_local = A[0];
	maxB_local = minB_local = B[0];
	maxC_local = minC_local = C[0];

	// Calcular strip AB=A.B
	// A por filas, B por columnas, AB por filas
	#pragma omp parallel
	{
		#pragma omp for private(i,j,k) reduction(+:totalA_local,totalB_local) reduction(max:maxA_local,maxB_local) reduction(min:minA_local,minB_local) nowait
		for (i = 0; i < stripSize; i++) {
			for (j = 0; j < N; j++) {
				double suma_parcial = 0;

				// Mínimo, Máximo y Suma de B
				if (B[i * N + j] < minB_local)
					minB_local = B[i * N + j];

				if (B[i * N + j] > maxB_local)
					maxB_local = B[i * N + j];

				totalB_local += B[i * N + j];

				// Mínimo, Máximo y Suma de A
				if (A[i * N + j] < minA_local)
					minA_local = A[i * N + j];

				if (A[i * N + j] > maxA_local)
					maxA_local = A[i * N + j];

				totalA_local += A[i * N + j];

				// Multiplicación
				for (k = 0; k < N; k++) {
					suma_parcial += A[i * N + k] * B[j * N + k];
				}

				AB[i * N + j] = suma_parcial;
			}
		}

		// Calcular strip D=AB.C
		// AB por filas, C por columnas, D por filas
		#pragma omp for private(i,j,k) reduction(+:totalC_local) reduction(max:maxC_local) reduction(min:minC_local)
		for (i = 0; i < stripSize; i++) {
			for (j = 0; j < N; j++) {
				double suma_parcial = 0;

				// Mínimo, Máximo y Suma de C
				if (C[i * N + j] < minC_local)
					minC_local = C[i * N + j];

				if (C[i * N + j] > maxC_local)
					maxC_local = C[i * N + j];

				totalC_local += C[i * N + j];

				// Multiplicación
				for (k = 0; k < N; k++) {
					suma_parcial += AB[i * N + k] * C[j * N + k];
				}

				D[i * N + j] = suma_parcial;
			}
		}
	}


	/* 2º Comunicación --> Reducciones de mínimo, máximo y suma de ABC */
	commTimes[2] = MPI_Wtime();

	MPI_Reduce(&totalA_local, &totalA, 1, MPI_DOUBLE, MPI_SUM, COORDINATOR, MPI_COMM_WORLD);
	MPI_Reduce(&totalB_local, &totalB, 1, MPI_DOUBLE, MPI_SUM, COORDINATOR, MPI_COMM_WORLD);
	MPI_Reduce(&totalC_local, &totalC, 1, MPI_DOUBLE, MPI_SUM, COORDINATOR, MPI_COMM_WORLD);

	MPI_Reduce(&maxA_local, &maxA, 1, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);
	MPI_Reduce(&maxB_local, &maxB, 1, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);
	MPI_Reduce(&maxC_local, &maxC, 1, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);

	MPI_Reduce(&minA_local, &minA, 1, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
	MPI_Reduce(&minB_local, &minB, 1, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
	MPI_Reduce(&minC_local, &minC, 1, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);

	commTimes[3] = MPI_Wtime();

	/* El coordinador se encarga de calcular d y los demás esperan a que termine */
	if (rank == COORDINATOR) {
		avgA = totalA / (N * N);
		avgB = totalB / (N * N);
		avgC = totalC / (N * N);

		d = ((maxA * maxB * maxC) - (minA * minB * minC)) / (avgA * avgB * avgC);
	}

	/* 3º Comunicación --> Se comunica d a todos los procesos */
	commTimes[4] = MPI_Wtime();
	MPI_Bcast(&d, 1, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
	commTimes[5] = MPI_Wtime();

	/* Se calcula D=d.D */
	#pragma omp for private(i,j)
	for (i = 0; i < stripSize; i++) {
		for (j = 0; j < N; j++) {
			D[i * N + j] *= d;
		}
	}

	/* 4º Comunicación --> Recolección de D=d.D al coordinador */
	commTimes[6] = MPI_Wtime();
	MPI_Gather(D, N * stripSize, MPI_DOUBLE, D, N * stripSize, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
	commTimes[7] = MPI_Wtime();

	/* Se obtienen los tiempos de comunicación */
	MPI_Reduce(commTimes, minCommTimes, 8, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
	MPI_Reduce(commTimes, maxCommTimes, 8, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);

	MPI_Finalize();

	if (rank == COORDINATOR) {

		// Verificar resultado
		if (resultado_valido(D)) {
			printf("Multiplicacion de matrices resultado correcto\n");
		} else {
			printf("Multiplicacion de matrices resultado erroneo\n");
		}

		// Se calculan los tiempos de comunicación y total

		totalTime = maxCommTimes[7] - minCommTimes[0];

		for (i = 0; i < 8; i += 2) {
			double final = maxCommTimes[i + 1];
			double comienzo = minCommTimes[i];

			commTime += final - comienzo;

		}

		printf("Multiplicacion de matrices (N=%d)\tTiempo total=%lf\tTiempo comunicacion=%lf\n", N, totalTime, commTime);
	}

	/* Liberación de memoria */
	free(A);
	free(B);
	free(C);
	free(AB);
	free(D);

	return 0;
}