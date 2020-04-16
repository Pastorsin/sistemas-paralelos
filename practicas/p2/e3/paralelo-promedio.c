#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

// Cantidad de Threads
int T=4;

// Tamaño por defecto del vector
int N=4096;
int *VECTOR;

int total=0;
pthread_mutex_t total_mutex;


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


void acumular_valores(void *ptr){
	int id = * (int *) ptr;
	int total_parcial = 0;

	int principio = id * N/T;
  	int final = (id + 1) * N/T;

  	for (int i = principio; i < final; i++){
		total_parcial += VECTOR[i];
  	}

  	if (total_parcial != 0){
	  	pthread_mutex_lock(&total_mutex);
  		total += total_parcial;
  		pthread_mutex_unlock(&total_mutex);
  	}  	
}


int resultado_valido(double promedio){
	return (promedio == 0.5);
}


int main(int argc, char const *argv[]){
	double timetick, promedio;
  	int ids[T];
  	pthread_t acumulador_thread[T];

	//Controla los argumentos al programa
	if ((argc != 2) || ((N = atoi(argv[1])) <= 0) ){
		printf("\nUsar: %s N \n N: Tamaño del vector  ", argv[0]);
		exit(1);
	}

	pthread_mutex_init(&total_mutex, NULL); 

	VECTOR = (int*)malloc(sizeof(int)*N);
	inicializar();

	timetick = dwalltime();

	for (int i = 0; i < T; i++){
		ids[i] = i;
		pthread_create(&acumulador_thread[i], NULL, acumular_valores, &ids[i]);
	}

	for (int i = 0; i < T; i++)
    	pthread_join(acumulador_thread[i], NULL);

    promedio = (double)total / N;

	printf("Promedio en Vector de tamaño %i. Tiempo en segundos %f\n", N, dwalltime() - timetick);

	if(resultado_valido(promedio)){
    	printf("Promedio correcto: %f\n", promedio);
  	}else{
    	printf("Promedio erroneo: %f\n", promedio);
  	}

  	free(VECTOR);

	return 0;
}