#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define COORDINATOR 0

// Tamaño del vector
int N;

int resultado_valido(double min, double max, double promedio) {
	double sumatoria_naturales = (N * (N + 1.0)) / 2;

	return min == 1 && max == N && promedio == sumatoria_naturales / N;
}

int main(int argc, char* argv[]) {
	int i, numProcs, rank, stripSize;
	double *V;
	double max_local, min_local, suma_local = 0, suma_total, promedio, max, min;

	MPI_Status status;
	double commTimes[4], maxCommTimes[4], minCommTimes[4], commTime;
	double totalTimes[2], maxTotalTime[2], minTotalTime[2], totalTime;


	// Controla los argumentos al programa
	if ((argc != 2) || ((N = atoi(argv[1])) <= 0) ) {
		printf("\nUsar: %s n\n  n: Tamaño del vector N\n", argv[0]);
		exit(1);
	}

	// Inicialización MPI
	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// Control de stripSize
	if (N % numProcs != 0) {
		printf("El tamaño del vector debe ser multiplo del numero de procesos.\n");
		exit(1);
	}

	stripSize = N / numProcs;

	// Reserva de memoria
	if (rank == COORDINATOR)
		V = (double*)malloc(sizeof(double) * N);
	else
		V = (double*)malloc(sizeof(double) * stripSize);

	// Inicialización vector V[i] = i+1
	if (rank == COORDINATOR)
		for (i = 0; i < N; i++)
			V[i] = i + 1;

	// Los workers esperan a que el coordinador inicialice V
	MPI_Barrier(MPI_COMM_WORLD);

	totalTimes[0] = MPI_Wtime();

	/* Comunicación --> Se distribuyen los datos de V */
	commTimes[0] = MPI_Wtime();
	MPI_Scatter(V, stripSize, MPI_DOUBLE, V, stripSize, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
	commTimes[1] = MPI_Wtime();

	/* Procesamiento -> Se calcula max, min y suma total de V en forma local */
	max_local, min_local = V[0];

	for (i = 0; i < stripSize; i++)	{
		if (V[i] > max_local)
			max_local = V[i];

		if (V[i] < min_local)
			min_local = V[i];

		suma_local += V[i];
	}

	commTimes[2] = MPI_Wtime();
	/* Comunicación -> Se realizan 3 reducciones de máximo, mínimo y suma
	de los valores calculados por cada worker
	*/

	MPI_Reduce(&suma_local, &suma_total, 1, MPI_DOUBLE, MPI_SUM, COORDINATOR, MPI_COMM_WORLD);
	MPI_Reduce(&max_local, &max, 1, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);
	MPI_Reduce(&min_local, &min, 1, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);

	commTimes[3] = MPI_Wtime();

	// Cálculo del promedio
	if (rank == COORDINATOR)
		promedio = suma_total / N;

	totalTimes[1] = MPI_Wtime();


	// Cálculo del tiempo
	MPI_Reduce(commTimes, minCommTimes, 4, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
	MPI_Reduce(commTimes, maxCommTimes, 4, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);

	MPI_Reduce(totalTimes, minTotalTime, 2, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
	MPI_Reduce(totalTimes, maxTotalTime, 2, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);

	MPI_Finalize();

	if (rank == COORDINATOR) {
		/* Verificar resultado */
		if (resultado_valido(min, max, promedio)) {
			printf("Resultado correcto\n");
		} else {
			printf("Resultado erroneo\n");
		}

		/* Calcular tiempo consumido */
		totalTime = maxTotalTime[1] - minTotalTime[0];
		commTime = (maxCommTimes[1] - minCommTimes[0]) + (maxCommTimes[3] - minCommTimes[2]);
		printf("Max, min, avg sobre V (N=%d)\tTiempo total=%f\tTiempo comunicacion=%f\n", N, totalTime, commTime);

	}

	/* Liberación de memoria */
	free(V);

	return 0;
}



