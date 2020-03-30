#include<stdio.h>
#include<stdlib.h>
#define ORDENXFILAS 0
#define ORDENXCOLUMNAS 1

//Dimension por defecto de las matrices
int N=2;

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

//Para calcular tiempo
double dwalltime(){
  double sec;
  struct timeval tv;

  gettimeofday(&tv,NULL);
  sec = tv.tv_sec + tv.tv_usec/1000000.0;
  return sec;
}

void init_matrices(double *A, double *B, double *C){
  setValor(A, 0, 0, ORDENXFILAS, 1);
  setValor(A, 0, 1, ORDENXFILAS, 2);
  setValor(A, 1, 0, ORDENXFILAS, 3);
  setValor(A, 1, 1, ORDENXFILAS, 4);

  setValor(B, 0, 0, ORDENXCOLUMNAS, 4);
  setValor(B, 0, 1, ORDENXCOLUMNAS, 3);
  setValor(B, 1, 0, ORDENXCOLUMNAS, 3);
  setValor(B, 1, 1, ORDENXCOLUMNAS, 2);

  setValor(C, 0, 0, ORDENXCOLUMNAS, 1);
  setValor(C, 0, 1, ORDENXCOLUMNAS, 2);
  setValor(C, 1, 0, ORDENXCOLUMNAS, 3);
  setValor(C, 1, 1, ORDENXCOLUMNAS, 1);
}

void print_matrix(double *D){
  for (int i = 0; i < N; i++)
  {
    for (int j = 0; j < N; j++)
    {
      printf("%ix%i: %f\n", i, j, getValor(D, i, j, ORDENXFILAS));
    }
  }
}

int main(int argc,char*argv[]){
  double *A,*B,*C, *AB, *D;
  int i,j,k;
  int check=1;
  double timetick;

  //Controla los argumentos al programa
  if ((argc != 2) || ((N = atoi(argv[1])) <= 0) ){
    printf("\nUsar: %s n\n  n: Dimension de la matriz (nxn X nxn)\n", argv[0]);
    exit(1);
  }

  //Aloca memoria para las matrices
  A=(double*)malloc(sizeof(double)*N*N);
  B=(double*)malloc(sizeof(double)*N*N);
  C=(double*)malloc(sizeof(double)*N*N);
  AB=(double*)malloc(sizeof(double)*N*N);
  D=(double*)malloc(sizeof(double)*N*N);

  // Inicializa las matrices A y B en 1, el resultado sera una matriz con todos sus valores en N*N
  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
     setValor(A,i,j,ORDENXFILAS,1);
     setValor(B,i,j,ORDENXCOLUMNAS,1);
     setValor(C,i,j,ORDENXCOLUMNAS,1);
   }
  }   

  // init_matrices(A,B,C);

 //Realiza la multiplicacion

  timetick = dwalltime();

  for(i=0;i<N;i++){

    for(j=0;j<N;j++){
      double suma_parcial = 0;
    
      for(k=0;k<N;k++){
        suma_parcial += getValor(A,i,k,ORDENXFILAS)*getValor(B,k,j,ORDENXCOLUMNAS);
      }

      setValor(AB,i,j,ORDENXFILAS,suma_parcial);
    }
  }   

  for (i = 0; i < N; i++){

    for (j = 0; j < N; j++){
      double suma = 0;

      for (k = 0; k < N; k++){
        suma += getValor(AB, i, k, ORDENXFILAS) * getValor(C, k, j, ORDENXCOLUMNAS);
      }
      setValor(D, i, j, ORDENXFILAS, suma);
    }

  }

  printf("Multiplicacion de matrices de %dx%dx%d. Tiempo en segundos %f\n",N,N,N, dwalltime() - timetick);

  // print_matrix(D);

  //Verifica el resultado
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
      check=check&&(getValor(D,i,j,ORDENXFILAS)==N*N);
    }
  }   
  if(check){
    printf("Multiplicacion de matrices resultado correcto\n");
  }else{
    printf("Multiplicacion de matrices resultado erroneo\n");
  }

 free(A);
 free(B);
 free(C);
 free(AB);
 free(D);

 return(0);
}
