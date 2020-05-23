#include<stdio.h>
#include<stdlib.h>

// Tamaño del vector
int N;

// Para calcular tiempo
double dwalltime() {
	double sec;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	sec = tv.tv_sec + tv.tv_usec / 1000000.0;
	return sec;
}

int resultado_valido(double min, double max, double promedio) {
	double sumatoria_naturales = (N * (N + 1)) / 2;

	return min == 1 && max == N && promedio == sumatoria_naturales / N;
}

int main(int argc, char*argv[]) {
	double *V;
	double max, min, suma_total, promedio;
	int i;
	double timetick;

	// Controla los argumentos al programa
	if ((argc != 2) || ((N = atoi(argv[1])) <= 0) ) {
		printf("\nUsar: %s n\n  n: Tamaño del vector N\n", argv[0]);
		exit(1);
	}

	// Reserva de memoria
	V = (double*)malloc(sizeof(double) * N);

	// Inicialización vector V[i] = i+1
	for (i = 0; i < N; i++)
		V[i] = i + 1;

	// Inicializaciones
	max, min = V[0];
	suma_total = 0;

	printf("Vector de tamaño N=%i\n", N);

	printf("Realizando cálculos: Máximo, mínimo y promedio de V . . .\n\n");

	timetick = dwalltime();

	for (i = 0; i < N; i++)	{
		if (V[i] > max)
			max = V[i];

		if (V[i] < min)
			min = V[i];

		suma_total += V[i];
	}

	promedio = suma_total / N;

	printf("Tiempo en segundos %f\n", dwalltime() - timetick);

	if (resultado_valido(min, max, promedio)) {
		printf("Resultado correcto\n");
	} else {
		printf("Resultado erroneo\n");
	}

	free(V);

	return 0;
}