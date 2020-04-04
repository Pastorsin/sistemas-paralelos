#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

// Cantidad de Threads
int T=4;

// Tamaño por defecto del vector
int N=4096;
int *VECTOR;

// Valor a buscar
int VALOR=1;
int ocurrencias=0;
pthread_mutex_t ocurrencias_mutex;


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


void buscar_ocurrencias(void *ptr){
	int id = * (int *) ptr;
	int ocurrencias_parciales = 0;

	int principio = id * N/T;
  	int final = (id + 1) * N/T;

  	for (int i = principio; i < final; i++){
  		if (VECTOR[i] == VALOR)
  			ocurrencias_parciales++;
  	}

  	if (ocurrencias_parciales != 0){
	  	pthread_mutex_lock(&ocurrencias_mutex);
	  	ocurrencias += ocurrencias_parciales;
	  	pthread_mutex_unlock(&ocurrencias_mutex);
  	}
}


int resultado_valido(){
	return (ocurrencias == N/2);
}


int main(int argc, char const *argv[]){
	double timetick;
  	int ids[T];
  	pthread_t buscador_thread[T];

	//Controla los argumentos al programa
	if ((argc != 2) || ((N = atoi(argv[1])) <= 0) ){
		printf("\nUsar: %s N \n N: Tamaño del vector  ", argv[0]);
		exit(1);
	}

	pthread_mutex_init(&ocurrencias_mutex, NULL); 

	VECTOR = (int*)malloc(sizeof(int)*N);
	inicializar();

	timetick = dwalltime();

	for (int i = 0; i < T; i++){
		ids[i] = i;
		pthread_create(&buscador_thread[i], NULL, buscar_ocurrencias, &ids[i]);
	}

	for (int i = 0; i < T; i++)
    	pthread_join(buscador_thread[i], NULL);

	printf("Ocurrencias en Vector de tamaño %i. Tiempo en segundos %f\n", N, dwalltime() - timetick);

	if(resultado_valido()){
    	printf("Cantidad de ocurrencias correcta: %i\n", ocurrencias);
  	}else{
    	printf("Cantidad de ocurrencias erronea: %i\n", ocurrencias);
  	}

  	free(VECTOR);

	return 0;
}