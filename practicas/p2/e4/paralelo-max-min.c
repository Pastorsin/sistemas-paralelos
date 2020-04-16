#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include <semaphore.h>

// Cantidad de Threads
int T=4;

// Tamaño por defecto del vector
int N=4096;
int *VECTOR;

int max;
int min;
sem_t max_sem;
sem_t min_sem;


//Para calcular tiempo
double dwalltime(){
  double sec;
  struct timeval tv;

  gettimeofday(&tv,NULL);
  sec = tv.tv_sec + tv.tv_usec/1000000.0;
  return sec;
}


// Inicializa al vector 
// Con un 0 es sus posiciones pares
// Con un 1 en sus posiciones impares
void inicializar(){
	for (int i = 0; i < N; i++){
		VECTOR[i] = i % 2;
	}
}


void buscar_max_min(void *ptr){
	int max_local, min_local;
	int id = * (int *) ptr;

	int principio = id * N/T;
	int final = (id + 1) * N/T;

	max_local = min_local = VECTOR[principio];

	for (int i = principio; i < final; i++){
		if (VECTOR[i] > max_local)
			max_local = VECTOR[i];

		if (VECTOR[i] < min_local)
			min_local = VECTOR[i];
	}

	sem_wait(&max_sem);
	if (max_local > max)
		max = max_local;
	sem_post(&max_sem);

	sem_wait(&min_sem);
	if (min_local < min)
		min = min_local;
	sem_post(&min_sem);

}


int resultado_valido(){
	return max == 1 && min == 0;
}


int main(int argc, char const *argv[]){
	double timetick;
	int ids[T];
	pthread_t max_min_thread[T];

	//Controla los argumentos al programa
	if ((argc != 2) || ((N = atoi(argv[1])) <= 0) ){
		printf("\nUsar: %s N \n N: Tamaño del vector  ", argv[0]);
		exit(1);
	}

	sem_init(&max_sem, NULL, 1); 
	sem_init(&min_sem, NULL, 1);

	VECTOR = (int*)malloc(sizeof(int)*N);
	inicializar();

	min = max = VECTOR[0];

	timetick = dwalltime();

	for (int i = 0; i < T; i++){
		ids[i] = i;
		pthread_create(&max_min_thread[i], NULL, buscar_max_min, &ids[i]);
	}

	for (int i = 0; i < T; i++)
		pthread_join(max_min_thread[i], NULL);

	printf("Maximo/Mínimo en Vector de tamaño %i. Tiempo en segundos %f\n", N, dwalltime() - timetick);

	if(resultado_valido()){
		printf("Máximo y mínimo correcto. Máximo: %i - Mínimo: %i\n", max, min);
	}else{
		printf("Máximo y mínimo erroneo. Máximo: %i - Mínimo: %i\n", max, min);		
	}

	free(VECTOR);

	return 0;
}