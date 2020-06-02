#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define COORDINATOR 0

// Dimensión de las matrices
int N;

// Número de comunicaciones
int N_COMM = 3;

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
	int i, j, k, numProcs, rank, stripSize, offset, index;
	double *A, *B, *C, *AB, *D, d;

	double maxA_local, maxB_local, maxC_local, minA_local, minB_local, minC_local;
	double totalA_local, totalB_local, totalC_local;

	double maxA, maxB, maxC, minA, minB, minC;
	double totalA, totalB, totalC;
	double avgA, avgB, avgC;

	MPI_Status status;
	double commTimes[N_COMM * 2], maxCommTimes[N_COMM * 2], minCommTimes[N_COMM * 2], commTime = 0, totalTime;

	/* Lee parámetros de la línea de comando */
	if ((argc != 2) || ((N = atoi(argv[1])) <= 0) ) {
		printf("\nUsar: %s size \n  size: Dimension de la matriz\n", argv[0]);
		exit(1);
	}

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

	// Offset que tendrá que sumarle cada proceso a las matrices..
	// ..comunicadas por Broadcast
	offset = stripSize * rank;

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
	for (i = 0; i < stripSize; i++) {
		for (j = 0; j < N; j++) {
			double suma_parcial = 0;

			/* Este índice se utiliza para acceder
			a las matrices que fueron comunicadas mediante
			broadcast */
			index = (i + offset) * N + j;

			// Mínimo, Máximo y Suma de B
			if (B[index] < minB_local)
				minB_local = B[index];

			if (B[index] > maxB_local)
				maxB_local = B[index];

			totalB_local += B[index];

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
	for (i = 0; i < stripSize; i++) {
		for (j = 0; j < N; j++) {
			double suma_parcial = 0;

			/* Este índice se utiliza para acceder
			a las matrices que fueron comunicadas mediante
			broadcast */
			index = (i + offset) * N + j;

			// Mínimo, Máximo y Suma de C
			if (C[index] < minC_local)
				minC_local = C[index];

			if (C[index] > maxC_local)
				maxC_local = C[index];

			totalC_local += C[index];

			// Multiplicación
			for (k = 0; k < N; k++) {
				suma_parcial += AB[i * N + k] * C[j * N + k];
			}

			D[i * N + j] = suma_parcial;
		}
	}


	/* 2º Comunicación --> Reducciones de mínimo, máximo y suma de ABC */
	commTimes[2] = MPI_Wtime();

	MPI_Allreduce(&totalA_local, &totalA, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	MPI_Allreduce(&totalB_local, &totalB, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	MPI_Allreduce(&totalC_local, &totalC, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

	MPI_Allreduce(&maxA_local, &maxA, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
	MPI_Allreduce(&maxB_local, &maxB, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
	MPI_Allreduce(&maxC_local, &maxC, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

	MPI_Allreduce(&minA_local, &minA, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
	MPI_Allreduce(&minB_local, &minB, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
	MPI_Allreduce(&minC_local, &minC, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);

	commTimes[3] = MPI_Wtime();

	avgA = totalA / (N * N);
	avgB = totalB / (N * N);
	avgC = totalC / (N * N);

	d = ((maxA * maxB * maxC) - (minA * minB * minC)) / (avgA * avgB * avgC);

	/* Se calcula D=d.D */
	for (i = 0; i < stripSize; i++) {
		for (j = 0; j < N; j++) {
			D[i * N + j] *= d;
		}
	}

	/* 3º Comunicación --> Recolección de D=d.D al coordinador */
	commTimes[4] = MPI_Wtime();
	MPI_Gather(D, N * stripSize, MPI_DOUBLE, D, N * stripSize, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
	commTimes[5] = MPI_Wtime();

	/* Se obtienen los tiempos de comunicación */
	MPI_Reduce(commTimes, minCommTimes, N_COMM * 2, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
	MPI_Reduce(commTimes, maxCommTimes, N_COMM * 2, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);

	MPI_Finalize();

	if (rank == COORDINATOR) {

		// Verificar resultado
		if (resultado_valido(D)) {
			printf("Multiplicacion de matrices resultado correcto\n");
		} else {
			printf("Multiplicacion de matrices resultado erroneo\n");
		}

		// Se calculan los tiempos de comunicación y total

		totalTime = maxCommTimes[(N_COMM * 2) - 1] - minCommTimes[0];

		for (i = 0; i < N_COMM * 2; i += 2) {
			double final = maxCommTimes[i + 1];
			double comienzo = minCommTimes[i];

			printf("Comunicación %i: %f\n", i/2, final - comienzo);

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