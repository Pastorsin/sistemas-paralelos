#include<stdio.h>
#include<stdlib.h>

// Cantidad de Threads
int T=4;

struct set {
	int* elements;
	int size;	
};

// Conjuntos
struct set AB, A, B;

// Cardinalidad de los conjuntos A y B
int A_N=131072;
int B_N=131072;

pthread_mutex_t ab_mutex;


//Para calcular tiempo
double dwalltime(){
  double sec;
  struct timeval tv;

  gettimeofday(&tv,NULL);
  sec = tv.tv_sec + tv.tv_usec/1000000.0;
  return sec;
}


// Inicializa A con las potencias de 2 desde 2^0
// Inicializa B con los pares desde el 0
void inicializar(){
	int i;

	for (i = 0; i < A.size; i++)
		// A.elements[i] = 1 << i;
		A.elements[i] = i * 2;

	for (i = 0; i < B.size; i++)
		B.elements[i] = i * 2;
}


void interseccion(void *ptr){
	int id = * (int *) ptr;

	int principio = id * B.size/T;
	int final = (id + 1) * B.size/T;

	for (int i = principio; i < final; i++){
		for (int j = 0; j < A.size; j++){
			if (A.elements[j] == B.elements[i]){
				pthread_mutex_lock(&ab_mutex);
				AB.elements[AB.size] = B.elements[i];
				AB.size++;
				pthread_mutex_unlock(&ab_mutex);
				break;
			}
		}
	}
}


int resultado_valido(){
	return 1;
}


void resultado(){
	for (int i = 0; i < AB.size; i++){
		printf("%i\n", AB.elements[i]);
	}
}


int main(int argc, char const *argv[]){
	double timetick;
	int ids[T], i;
	pthread_t interseccion_thread[T];

	pthread_mutex_init(&ab_mutex, NULL); 

	A.elements = (int*)malloc(sizeof(int)*A_N);
	A.size = A_N;

	B.elements = (int*)malloc(sizeof(int)*B_N);
	B.size = B_N;

	AB.elements = (int*)malloc(sizeof(int)*B_N);
	AB.size = 0;

	inicializar();

	timetick = dwalltime();

	for (i = 0; i < T; i++){
		ids[i] = i;
		pthread_create(&interseccion_thread[i], NULL, interseccion, &ids[i]);
	}

	for (i = 0; i < T; i++)
		pthread_join(interseccion_thread[i], NULL);

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