#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define COORDINATOR 0

int main(int argc, char* argv[]) {
	int i, j, k, numProcs, rank, n, stripSize, check = 1;
	double * a, * b, *c, *d, *e, *f, *r, *ab, *cd, *ef;
	MPI_Status status;
	double commTimes[4], maxCommTimes[4], minCommTimes[4], commTime, totalTime;

	/* Lee parámetros de la línea de comando */
	if ((argc != 2) || ((n = atoi(argv[1])) <= 0) ) {
		printf("\nUsar: %s size \n  size: Dimension de la matriz y el vector\n", argv[0]);
		exit(1);
	}

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (n % numProcs != 0) {
		printf("El tamaño de la matriz debe ser multiplo del numero de procesos.\n");
		exit(1);
	}

	// Porción de cada worker
	stripSize = n / numProcs;

	/* Reserva de memoria */

	// Matrices resultantes
	if (rank == COORDINATOR) {
		a = (double*) malloc(sizeof(double) * n * n);
		c = (double*) malloc(sizeof(double) * n * n);
		e = (double*) malloc(sizeof(double) * n * n);
		r = (double*) malloc(sizeof(double) * n * n);
	}
	else  {
		a = (double*) malloc(sizeof(double) * n * stripSize);
		c = (double*) malloc(sizeof(double) * n * stripSize);
		e = (double*) malloc(sizeof(double) * n * stripSize);
		r = (double*) malloc(sizeof(double) * n * stripSize);
	}

	b = (double*) malloc(sizeof(double) * n * n);
	d = (double*) malloc(sizeof(double) * n * n);
	f = (double*) malloc(sizeof(double) * n * n);

	// Matrices temporales
	ab = (double*) malloc(sizeof(double) * n * stripSize);
	cd = (double*) malloc(sizeof(double) * n * stripSize);
	ef = (double*) malloc(sizeof(double) * n * stripSize);

	/* Inicializar matrices */

	// Se organiza a A,C,E por fila
	// Se organiza a B,D,F por columna
	if (rank == COORDINATOR) {
		for (i = 0; i < n ; i++)
			for (j = 0; j < n ; j++)
				a[i * n + j] = 1;
		for (i = 0; i < n ; i++)
			for (j = 0; j < n ; j++)
				b[j * n + i] = 1;
		for (i = 0; i < n ; i++)
			for (j = 0; j < n ; j++)
				c[i * n + j] = 1;
		for (i = 0; i < n ; i++)
			for (j = 0; j < n ; j++)
				d[j * n + i] = 1;
		for (i = 0; i < n ; i++)
			for (j = 0; j < n ; j++)
				e[i * n + j] = 1;
		for (i = 0; i < n ; i++)
			for (j = 0; j < n ; j++)
				f[j * n + i] = 1;
	}

	MPI_Barrier(MPI_COMM_WORLD);

	commTimes[0] = MPI_Wtime();

	/* Comunicación --> Distribución de datos */

	// Matrices de la izq
	MPI_Scatter(a, n * stripSize, MPI_DOUBLE, a, n * stripSize, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
	MPI_Scatter(c, n * stripSize, MPI_DOUBLE, c, n * stripSize, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
	MPI_Scatter(e, n * stripSize, MPI_DOUBLE, e, n * stripSize, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);

	// Matrices de la der
	MPI_Bcast(b, n * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
	MPI_Bcast(d, n * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
	MPI_Bcast(f, n * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);

	commTimes[1] = MPI_Wtime();

	/* Procesamiento --> El worker calcula los strips R = AB + CD + EF */

	// Multiplicación AB = A.B
	for (i = 0; i < stripSize; i++) {
		for (j = 0; j < n ; j++ ) {
			double suma_parcial = 0;
			for (k = 0; k < n ; k++ ) {
				suma_parcial += a[i * n + k] * b[j * n + k];
			}
			ab[i * n + j] = suma_parcial;
		}
	}

	// Multiplicación CD = C.D
	for (i = 0; i < stripSize; i++) {
		for (j = 0; j < n ; j++ ) {
			double suma_parcial = 0;
			for (k = 0; k < n ; k++ ) {
				suma_parcial += c[i * n + k] * d[j * n + k];
			}
			cd[i * n + j] = suma_parcial;
		}
	}

	// Multiplicación EF = E.F
	for (i = 0; i < stripSize; i++) {
		for (j = 0; j < n ; j++ ) {
			double suma_parcial = 0;
			for (k = 0; k < n ; k++ ) {
				suma_parcial += e[i * n + k] * f[j * n + k];
			}
			ef[i * n + j] = suma_parcial;
		}
	}

	// Sumatoria R = AB + CD + EF
	for (i = 0; i < n*stripSize; i++)
		r[i] = ab[i] + cd[i] + ef[i];

	commTimes[2] = MPI_Wtime();

	/* Comunicación --> Recolección de los strips R */
	MPI_Gather(r, n * stripSize, MPI_DOUBLE, r, n * stripSize, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);

	commTimes[3] = MPI_Wtime();

	MPI_Reduce(commTimes, minCommTimes, 4, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
	MPI_Reduce(commTimes, maxCommTimes, 4, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);

	MPI_Finalize();

	if (rank == COORDINATOR) {

		// Verificar resultado
		for (i = 0; i < n; i++)
			for (j = 0; j < n; j++) {
				check = check && (r[i * n + j] == 3*n);
			}

		if (check) {
			printf("Multiplicacion de matrices resultado correcto\n");
		} else {
			printf("Multiplicacion de matrices resultado erroneo\n");
		}

		totalTime = maxCommTimes[3] - minCommTimes[0];
		commTime = (maxCommTimes[1] - minCommTimes[0]) + (maxCommTimes[3] - minCommTimes[2]);

		printf("Multiplicacion de matrices (N=%d)\tTiempo total=%lf\tTiempo comunicacion=%lf\n", n, totalTime, commTime);
	}

	/* Liberación de memoria */
	free(a);
	free(b);
	free(c);
	free(d);
	free(e);
	free(f);
	free(r);
	free(ab);
	free(cd);
	free(ef);

	return 0;
}