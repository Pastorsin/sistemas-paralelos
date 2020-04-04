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


int resultado_valido(int max, int min){
	return max == 1 && min == 0;
}


int main(int argc, char const *argv[]){
	double timetick;
	int max, min;

	//Controla los argumentos al programa
	if ((argc != 2) || ((N = atoi(argv[1])) <= 0) ){
		printf("\nUsar: %s N \n N: Tamaño del vector  ", argv[0]);
		exit(1);
	}

	VECTOR = (int*)malloc(sizeof(int)*N);

	inicializar();

	max = min = VECTOR[0];

	timetick = dwalltime();

	for (int i = 0; i < N; i++){
		if (VECTOR[i] > max)
			max = VECTOR[i];

		if (VECTOR[i] < min)
			min = VECTOR[i];
	}

	printf("Maximo/Mínimo en Vector de tamaño %i. Tiempo en segundos %f\n", N, dwalltime() - timetick);

	if(resultado_valido(max, min)){
		printf("Máximo y mínimo correcto. Máximo: %i - Mínimo: %i\n", max, min);
	}else{
		printf("Máximo y mínimo erroneo. Máximo: %i - Mínimo: %i\n", max, min);		
	}

	free(VECTOR);

	return 0;
}