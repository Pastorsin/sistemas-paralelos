#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <mpi.h>

#define COORDINATOR 0

// Dimensión de las matrices
int N;

// Cantidad de threads
int T;

// Número de comunicaciones
// En este caso son 3, pero el broadcast se hace en 2 partes
int N_COMM = 4;

int imprimir(double *M) {
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			printf("%ix%i --> %f\n", i, j, M[i * N + j]);
		}
	}
}

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
	int i, j, k, numProcs, rank, stripSize, offset, index, provided;
	double *A, *B, *C, *AB, *D, d;

	double maximos[3], minimos[3], totales[3];
	double maximos_locales[3], minimos_locales[3], totales_locales[3];

	double maxA, minA, maxB, minB, maxC, minC;
	double totalA, totalB, totalC;

	double avgA, avgB, avgC;

	MPI_Status status;
	double commTimes[N_COMM * 2], maxCommTimes[N_COMM * 2], minCommTimes[N_COMM * 2], commTime = 0, totalTime;


	/* Lee parámetros de la línea de comando */
	if (argc < 3) {
		printf("\n Faltan argumentos:: N dimension de la matriz, T cantidad de threads \n");
		exit(1);
	}

	N = atoi(argv[1]);
	T = atoi(argv[2]);

	/* Inicializar MPI */
	MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	/* Inicializar OpenMP */
	omp_set_num_threads(T);

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

	// Inicializar totales locales
	totalA = totalB = totalC = 0;

	// Los procesos esperan a que el coordinador termine de inicializar
	MPI_Barrier(MPI_COMM_WORLD);

	/* 1º Comunicación --> Distribución de datos */
	commTimes[0] = MPI_Wtime();
	MPI_Scatter(A, N * stripSize, MPI_DOUBLE, A, N * stripSize, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
	commTimes[1] = MPI_Wtime();

	/* Procesamiento local */

	// Inicializar máximo y mínimo local de A
	maxA = minA = A[0];

	// Sólo el coordinador podrá inicializar los max y min de B/C
	if (rank == COORDINATOR)
	{
		maxB = minB = B[0];
		maxC = minC = C[0];
	}

	// Calcular strip AB=A.B
	// A por filas, B por columnas, AB por filas
	#pragma omp parallel
	{

		// Solo 1 hilo del coordinador hace la comunicación
		// Los demás hilos del proceso coordinador realizan el cómputo A.B.C
		// Solo 1 hilo de los workers hace la comunicación
		// Los demás hilos de los workers se duermen esperando los datos
		if (rank == COORDINATOR) {
			#pragma omp single nowait
			{
				commTimes[2] = MPI_Wtime();
				MPI_Bcast(B, N * N, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
				MPI_Bcast(C, N * N, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
				commTimes[3] = MPI_Wtime();
			}
		} else {
			#pragma omp single
			{
				commTimes[2] = MPI_Wtime();
				MPI_Bcast(B, N * N, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
				MPI_Bcast(C, N * N, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
				commTimes[3] = MPI_Wtime();

				maxB = minB = B[0];
				maxC = minC = C[0];
			}
		}

		// Calcular strip AB=A.B
		// A por filas, B por columnas, AB por filas
		#pragma omp for private(i,j,k,index) \
		reduction(+:totalA, totalB) \
		reduction(max:maxA, maxB) \
		reduction(min:minA, minB) \
		nowait
		for (i = 0; i < stripSize; i++) {
			for (j = 0; j < N; j++) {
				double suma_parcial = 0;

				/* Este índice se utiliza para acceder
				a las matrices que fueron comunicadas mediante
				Broadcast */
				index = (i + offset) * N + j;

				// Mínimo, Máximo y Suma de B
				if (B[index] < minB)
					minB = B[index];

				if (B[index] > maxB)
					maxB = B[index];

				totalB += B[index];

				// Mínimo, Máximo y Suma de A
				if (A[i * N + j] < minA)
					minA = A[i * N + j];

				if (A[i * N + j] > maxA)
					maxA = A[i * N + j];

				totalA += A[i * N + j];

				// Multiplicación
				for (k = 0; k < N; k++) {
					suma_parcial += A[i * N + k] * B[j * N + k];
				}

				AB[i * N + j] = suma_parcial;
			}
		}

		// Calcular strip D=AB.C
		// AB por filas, C por columnas, D por filas
		#pragma omp for private(i,j,k,index) \
		reduction(+: totalC) \
		reduction(max: maxC) \
		reduction(min: minC)
		for (i = 0; i < stripSize; i++) {
			for (j = 0; j < N; j++) {
				double suma_parcial = 0;

				/* Este índice se utiliza para acceder
				a las matrices que fueron comunicadas mediante
				broadcast */
				index = (i + offset) * N + j;

				// Mínimo, Máximo y Suma de C
				if (C[index] < minC)
					minC = C[index];

				if (C[index] > maxC)
					maxC = C[index];

				totalC += C[index];

				// Multiplicación
				for (k = 0; k < N; k++) {
					suma_parcial += AB[i * N + k] * C[j * N + k];
				}

				D[i * N + j] = suma_parcial;
			}
		}

		#pragma omp single
		{
			// Empaquetación de los datos locales
			totales_locales[0] = totalA;
			totales_locales[1] = totalB;
			totales_locales[2] = totalC;

			maximos_locales[0] = maxA;
			maximos_locales[1] = maxB;
			maximos_locales[2] = maxC;

			minimos_locales[0] = minA;
			minimos_locales[1] = minB;
			minimos_locales[2] = minC;

			// 2º Comunicación -- > Intercambio de datos
			commTimes[4] = MPI_Wtime();
			MPI_Allreduce(&totales_locales, &totales, 3, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
			MPI_Allreduce(&maximos_locales, &maximos, 3, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
			MPI_Allreduce(&minimos_locales, &minimos, 3, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
			commTimes[5] = MPI_Wtime();

			// Cálculo del escalar
			avgA = totales[0] / (N * N);
			avgB = totales[1] / (N * N);
			avgC = totales[2] / (N * N);

			d = ((maximos[0] * maximos[1] * maximos[2]) - (minimos[0] * minimos[1] * minimos[2])) / (avgA * avgB * avgC);
		}

		/* Se calcula D=d.D */
		#pragma omp parallel for private(i,j)
		for (i = 0; i < stripSize; i++) {
			for (j = 0; j < N; j++) {
				D[i * N + j] *= d;
			}
		}

	}

	/* 4º Comunicación --> Recolección de D=d.D al coordinador */
	commTimes[6] = MPI_Wtime();
	MPI_Gather(D, N * stripSize, MPI_DOUBLE, D, N * stripSize, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
	commTimes[7] = MPI_Wtime();

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

			printf("Comunicación %i: %f\n", i / 2, final - comienzo);

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
