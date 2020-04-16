#include<stdio.h>
#include<stdlib.h>


struct set {
	int* elements;
	int size;	
};

// Conjuntos
struct set AB, A, B;

// Cardinalidad de los conjuntos A y B
int A_N=131072;
int B_N=131072;



//Para calcular tiempo
double dwalltime(){
  double sec;
  struct timeval tv;

  gettimeofday(&tv,NULL);
  sec = tv.tv_sec + tv.tv_usec/1000000.0;
  return sec;
}


// Inicializa A con los pares desde el 0
// Inicializa B con los pares desde el 2
void inicializar(){
	int i;

	for (i = 0; i < A.size; i++)
		A.elements[i] = i * 2;

	for (i = 0; i < B.size; i++)
		B.elements[i] = i * 2 + 2;
}


void interseccion(){
	for (int i = 0; i < B.size; i++){
		for (int j = 0; j < A.size; j++){
			if (A.elements[j] == B.elements[i]){
				AB.elements[AB.size] = B.elements[i];
				AB.size++;
				break;
			}
		}
	}
}


int resultado_valido(){
	if (A_N <= B_N)
		return AB.size == (A_N - 1);
	else
		return AB.size == B_N;
}


void resultado(){
	for (int i = 0; i < AB.size; i++){
		printf("%i\n", AB.elements[i]);
	}
}


int main(int argc, char const *argv[]){
	double timetick;

	A.elements = (int*)malloc(sizeof(int)*A_N);
	A.size = A_N;

	B.elements = (int*)malloc(sizeof(int)*B_N);
	B.size = B_N;

	AB.elements = (int*)malloc(sizeof(int)*B_N);
	AB.size = 0;

	inicializar();

	timetick = dwalltime();

	interseccion();

	printf("Intersección |A|=%i y |B|=%i. Tiempo en segundos %f\n", A_N, B_N, dwalltime() - timetick);

	// resultado();

	if(resultado_valido()){
		printf("Intersección correcta\n");
	}else{
		printf("Intersección errónea\n");		
	}

	free(A.elements);
	free(B.elements);
	free(AB.elements);

	return 0;
}