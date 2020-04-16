#include<stdio.h>
#include<stdlib.h>


// Tamaño por defecto del vector
int N=4096;
int *VECTOR;

// Valor a buscar
int VALOR=1;


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


int buscar_ocurrencias(){
	int ocurrencias = 0;

  	for (int i = 0; i < N; i++){
  		if (VECTOR[i] == VALOR)
  			ocurrencias++;
  	}

  	return ocurrencias;
}


int resultado_valido(int ocurrencias){
	return (ocurrencias == N/2);
}


int main(int argc, char const *argv[]){
	double timetick;

	//Controla los argumentos al programa
	if ((argc != 2) || ((N = atoi(argv[1])) <= 0) ){
		printf("\nUsar: %s N \n N: Tamaño del vector  ", argv[0]);
		exit(1);
	}

	VECTOR = (int*)malloc(sizeof(int)*N);

	inicializar();

	timetick = dwalltime();

	int ocurrencias = buscar_ocurrencias();

	printf("Ocurrencias en Vector de tamaño %i. Tiempo en segundos %f\n", N, dwalltime() - timetick);

	if(resultado_valido(ocurrencias)){
    	printf("Cantidad de ocurrencias correcta: %i\n", ocurrencias);
  	}else{
    	printf("Cantidad de ocurrencias erronea: %i\n", ocurrencias);
  	}

  	free(VECTOR);

	return 0;
}