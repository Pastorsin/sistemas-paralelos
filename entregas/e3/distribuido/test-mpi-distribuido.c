#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define COORDINATOR 0

// Dimensión de las matrices
int N = 4;

// Número de comunicaciones
int N_COMM = 3;

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
	for (int j = 0; j < N; j++)
	{
		if (D[0 * N + j] != 0)
			return 0;
	}
	return 1;
}


int main(int argc, char* argv[]) {
	int i, j, k, numProcs, rank, stripSize, offset, index;
	double *A, *B, *C, *AB, *D, d;

	double maximos[3], minimos[3], totales[3];
	double maximos_locales[3], minimos_locales[3], totales_locales[3];
	double avgA, avgB, avgC;

	MPI_Status status;
	double commTimes[N_COMM * 2], maxCommTimes[N_COMM * 2], minCommTimes[N_COMM * 2], commTime = 0, totalTime;

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

	B = (double*) malloc(sizeof(double) * N * N);
	C = (double*) malloc(sizeof(double) * N * N);

	// Matrices temporales
	AB = (double*)malloc(sizeof(double) * N * stripSize);

	//Inicializa las matrices A,B y C en 1
	// A por filas
	// B por columnas
	// C por columnas
	if (rank == COORDINATOR) {
		for (i = 0; i < N; i++) {
			for (j = 0; j < N; j++) {
				A[i * N + j] = i;
				B[i * N + j] = i + N;
				C[i * N + j] = i + 2 * N;
			}
		}
	}

	// Inicializar totales locales
	for (i = 0; i < 3 ; i++)
		totales_locales[i] = 0;


	// Los procesos esperan a que el coordinador termine de inicializar
	MPI_Barrier(MPI_COMM_WORLD);

	/* 1º Comunicación --> Distribución de datos */
	commTimes[0] = MPI_Wtime();

	MPI_Scatter(A, N * stripSize, MPI_DOUBLE, A, N * stripSize, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
	MPI_Bcast(B, N * N, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
	MPI_Bcast(C, N * N, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);

	commTimes[1] = MPI_Wtime();

	/* Procesamiento local */

	// Inicializar máximos y mínimos locales
	maximos_locales[0] = minimos_locales[0] = A[0];
	maximos_locales[1] = minimos_locales[1] = B[0];
	maximos_locales[2] = minimos_locales[2] = C[0];

	// Calcular strip AB=A.B
	// A por filas, B por columnas, AB por filas
	for (i = 0; i < stripSize; i++) {
		for (j = 0; j < N; j++) {
			double suma_parcial = 0;

			/* Este índice se utiliza para acceder
			a las matrices que fueron comunicadas mediante
			Broadcast */
			index = (i + offset) * N + j;

			// Mínimo, Máximo y Suma de B
			if (B[index] < minimos_locales[1])
				minimos_locales[1] = B[index];

			if (B[index] > maximos_locales[1])
				maximos_locales[1] = B[index];

			totales_locales[1] += B[index];

			// Mínimo, Máximo y Suma de A
			if (A[i * N + j] < minimos_locales[0])
				minimos_locales[0] = A[i * N + j];

			if (A[i * N + j] > maximos_locales[0])
				maximos_locales[0] = A[i * N + j];

			totales_locales[0] += A[i * N + j];

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
			if (C[index] < minimos_locales[2])
				minimos_locales[2] = C[index];

			if (C[index] > maximos_locales[2])
				maximos_locales[2] = C[index];

			totales_locales[2] += C[index];

			// Multiplicación
			for (k = 0; k < N; k++) {
				suma_parcial += AB[i * N + k] * C[j * N + k];
			}

			D[i * N + j] = suma_parcial;
		}
	}


	/* 2º Comunicación --> Reducciones de mínimo, máximo y suma de ABC */
	commTimes[2] = MPI_Wtime();

	MPI_Allreduce(&totales_locales, &totales, 3, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
	MPI_Allreduce(&maximos_locales, &maximos, 3, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
	MPI_Allreduce(&minimos_locales, &minimos, 3, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);

	commTimes[3] = MPI_Wtime();

	avgA = totales[0] / (N * N);
	avgB = totales[1] / (N * N);
	avgC = totales[2] / (N * N);

	d = ((maximos[0] * maximos[1] * maximos[2]) - (minimos[0] * minimos[1] * minimos[2])) / (avgA * avgB * avgC);

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

		imprimir(D);

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