#include<stdio.h>
#include<stdlib.h>
#define ORDENXFILAS 0
#define ORDENXCOLUMNAS 1


// Dimension por defecto de las matrices
int N=2;


// Retorna el valor de la matriz en la posicion fila y columna segun el orden que este ordenada
static inline double getValor(double *matriz,int fila,int columna,int orden){
  if (orden == ORDENXFILAS){
    return matriz[fila*N+columna];
  } else {
    return matriz[fila+columna*N];
  }
}


// Establece el valor de la matriz en la posicion fila y columna segun el orden que este ordenada
static inline void setValor(double *matriz, int fila, int columna, int orden, double valor){
  if (orden == ORDENXFILAS){
    matriz[fila*N+columna] = valor;
  }else{
    matriz[fila+columna*N] = valor;
  }
}


// Para calcular tiempo
double dwalltime(){
  double sec;
  struct timeval tv;

  gettimeofday(&tv,NULL);
  sec = tv.tv_sec + tv.tv_usec/1000000.0;
  return sec;
}


// Incializa A,B,C de ejemplo de tamaño 2x2
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


// Imprime los elementos de la matriz
void print_matrix(double *D){
  for (int i = 0; i < N; i++)
  {
    for (int j = 0; j < N; j++)
    {
      printf("%ix%i: %f\n", i, j, getValor(D, i, j, ORDENXFILAS));
    }
  }
}


// Realiza RESULTADO=M1*M2
static inline void multiplicar(double *M1, double *M2, double *RESULTADO){
  for(int i = 0; i < N; i++){

    for(int j = 0; j < N; j++){
      double suma_parcial = 0;

      for(int k = 0; k < N; k++){
        suma_parcial += getValor(M1, i, k, ORDENXFILAS) * getValor(M2, k, j, ORDENXCOLUMNAS);
      }

      setValor(RESULTADO, i, j, ORDENXFILAS, suma_parcial);
    }
  }
}


// Chequea si el resultado de la matriz resultante es N*N
int resultado_valido(double *M){
  int check = 1;

  for(int i = 0; i < N; i++){
    for(int j = 0; j < N; j++){
      check=check&&(getValor(M,i,j,ORDENXFILAS)==N*N);
    }
  }

  return check;
}


// Inicializa las matrices A, B y C en 1
// El resultado sera una matriz con todos sus valores en N*N
void inicializar(double *A, double *B, double *C){
  for(int i=0; i < N; i++){
   for(int j=0; j < N; j++){
     setValor(A,i,j,ORDENXFILAS,1);
     setValor(B,i,j,ORDENXCOLUMNAS,1);
     setValor(C,i,j,ORDENXCOLUMNAS,1);
   }
 }
}


int main(int argc,char*argv[]){
  double *A,*B,*C, *AB, *D;
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

  inicializar(A,B,C);   
  printf("Dimensión de las matrices: %ix%i\n\n", N, N);

  printf("Realizando multiplicación D=ABC . . .\n\n");

  timetick = dwalltime();

  multiplicar(A, B, AB);
  multiplicar(AB, C, D);

  printf("Tiempo en segundos %f\n", dwalltime() - timetick);

  if (resultado_valido(D)){
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
