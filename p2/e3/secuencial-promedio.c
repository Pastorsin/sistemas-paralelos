#include<stdio.h>
#include<stdlib.h>


// Tamaño por defecto del vector
int N=4096;
int *VECTOR;


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


int total(){
	int suma = 0;

  	for (int i = 0; i < N; i++){
  		suma += VECTOR[i];
  	}

  	return suma;
}


int resultado_valido(double promedio){
	return (promedio == 0.5);
}


int main(int argc, char const *argv[]){
	double timetick, promedio;

	//Controla los argumentos al programa
	if ((argc != 2) || ((N = atoi(argv[1])) <= 0) ){
		printf("\nUsar: %s N \n N: Tamaño del vector  ", argv[0]);
		exit(1);
	}

	VECTOR = (int*)malloc(sizeof(int)*N);

	inicializar();

	timetick = dwalltime();

	promedio = (double)total() / N;

	printf("Ocurrencias en Vector de tamaño %i. Tiempo en segundos %f\n", N, dwalltime() - timetick);

	if(resultado_valido(promedio)){
    	printf("Cantidad de ocurrencias correcta: %f\n", promedio);
  	}else{
    	printf("Cantidad de ocurrencias erronea: %f\n", promedio);
  	}

	return 0;
}