#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#define ORDENXFILAS 0
#define ORDENXCOLUMNAS 1

//Dimension por defecto de las matrices A, B, C
int N=256;
double *A,*B,*C;

//Cantidad de threads
int T=4;


//Retorna el valor de la matriz en la posicion fila y columna segun el orden que este ordenada
double getValor(double *matriz,int fila,int columna,int orden){
  if(orden==ORDENXFILAS){
    return(matriz[fila*N+columna]);
  } else {
    return(matriz[fila+columna*N]);
  }
}


//Establece el valor de la matriz en la posicion fila y columna segun el orden que este ordenada
void setValor(double *matriz,int fila,int columna,int orden,double valor){
  if(orden==ORDENXFILAS){
    matriz[fila*N+columna]=valor;
  }else{
    matriz[fila+columna*N]=valor;
  }
}


//Inicializa las matrices A y B en 1, el resultado sera una matriz con todos sus valores en N
void inicializar(double *A, double *B){
  for (int i = 0; i < N; i++){
    for (int j = 0; j < N; j++){
      setValor(A,i,j,ORDENXFILAS,1);
      setValor(B,i,j,ORDENXCOLUMNAS,1);
    }
  }
}


void multiplicar(void *ptr){
  int id = * (int *) ptr;

  int principio = id * N/T;
  int final = (id + 1) * N/T;

  for (int i=0; i < N; i++){
    for (int j=principio; j < final; j++){
      double suma_parcial = 0;

      for(int k = 0; k<N; k++){
        suma_parcial += getValor(A,i,k,ORDENXFILAS)*getValor(B,k,j,ORDENXCOLUMNAS);
      }

      setValor(C,i,j,ORDENXFILAS,suma_parcial);
    }
  }   
}


int resultado_valido(){
  int check = 1;

  for(int i=0;i<N;i++){
    for(int j=0;j<N;j++){
      check = check && (getValor(C,i,j,ORDENXFILAS)==N);
    }
  }  

  return check;
}


//Para calcular tiempo
double dwalltime(){
  double sec;
  struct timeval tv;

  gettimeofday(&tv,NULL);
  sec = tv.tv_sec + tv.tv_usec/1000000.0;
  return sec;
}


int main(int argc, char const *argv[]){
  double timetick;
  int ids[T];
  pthread_t multiplicacion_thread[T];

  //Controla los argumentos al programa
  if ((argc != 2) || ((N = atoi(argv[1])) <= 0) ){
    printf("\nUsar: %s n\n  n: Dimension de la matriz (nxn X nxn)\n", argv[0]);
    exit(1);
  }

  //Aloca memoria para las matrices
  A=(double*)malloc(sizeof(double)*N*N);
  B=(double*)malloc(sizeof(double)*N*N);
  C=(double*)malloc(sizeof(double)*N*N);

  inicializar(A, B);

  //Realiza la multiplicacion
  timetick = dwalltime();

  for (int i = 0; i < T; i++){
    ids[i] = i;
    pthread_create(&multiplicacion_thread[i], NULL, multiplicar, &ids[i]);
  }

  for (int i = 0; i < T; i++)
    pthread_join(multiplicacion_thread[i], NULL);

  printf("Multiplicacion de matrices de %dx%d. Tiempo en segundos %f\n", N, N, dwalltime() - timetick);   

  if(resultado_valido()){
    printf("Multiplicacion de matrices resultado correcto\n");
  }else{
    printf("Multiplicacion de matrices resultado erroneo\n");
  }

  free(A);
  free(B);
  free(C);

  return 0;
}