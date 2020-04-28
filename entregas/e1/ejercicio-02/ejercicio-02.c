#include<stdio.h>
#include<stdlib.h>


// Dimension por defecto de las matrices
int N = 100;


// Para calcular tiempo
double dwalltime() {
  double sec;
  struct timeval tv;

  gettimeofday(&tv, NULL);
  sec = tv.tv_sec + tv.tv_usec / 1000000.0;
  return sec;
}


// Chequea si el resultado de la matriz resultante es N*N
int resultado_valido(double *M) {
  int check = 1;

  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      check = check && (M[i * N + j] == N * N);
    }
  }

  return check;
}


int main(int argc, char*argv[]) {
  double *A, *B, *C, *AB, *D;
  int i, j, k;
  double timetick;

  //Controla los argumentos al programa
  if ((argc != 2) || ((N = atoi(argv[1])) <= 0) ) {
    printf("\nUsar: %s n\n  n: Dimension de la matriz (nxn X nxn)\n", argv[0]);
    exit(1);
  }

  //Aloca memoria para las matrices
  A = (double*)malloc(sizeof(double) * N * N);
  B = (double*)malloc(sizeof(double) * N * N);
  C = (double*)malloc(sizeof(double) * N * N);
  AB = (double*)malloc(sizeof(double) * N * N);
  D = (double*)malloc(sizeof(double) * N * N);


  // Inicializa las matrices A, B y C en 1
  // El resultado sera una matriz con todos sus valores en N*N
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      A[i * N + j] = 1;
      B[j * N + i] = 1;
      C[j * N + i] = 1;
    }
  }

  printf("Dimensión de las matrices: %ix%i\n\n", N, N);

  printf("Realizando multiplicación D=ABC . . .\n\n");

  timetick = dwalltime();

  // Realiza el producto AB=A.B
  // AB está organizada por filas
  // A está organizada por filas
  // B está organizada por columnas
  for ( i = 0; i < N; i++) {

    for ( j = 0; j < N; j++) {
      double suma_parcial = 0;

      for ( k = 0; k < N; k++) {
        suma_parcial += A[i * N + k] * B[j * N + k];
      }

      AB[i * N + j] = suma_parcial;
    }
  }

  // Realiza el producto D=AB.C
  // D está organizada por filas
  // AB está organizada por filas
  // C está organizada por columnas
  for ( i = 0; i < N; i++) {

    for ( j = 0; j < N; j++) {
      double suma_parcial = 0;

      for ( k = 0; k < N; k++) {
        suma_parcial += AB[i * N + k] * C[j * N + k];
      }

      D[i * N + j] = suma_parcial;
    }
  }
  printf("Tiempo en segundos %f\n", dwalltime() - timetick);

  if (resultado_valido(D)) {
    printf("Multiplicacion de matrices resultado correcto\n");
  } else {
    printf("Multiplicacion de matrices resultado erroneo\n");
  }

  free(A);
  free(B);
  free(C);
  free(AB);
  free(D);

  return (0);
}