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
      check = check && (M[i * N + j] == 3*N);
    }
  }

  return check;
}


int main(int argc, char*argv[]) {
  double *A, *B, *C, *D, *E, *F, *AB, *CD, *EF, *R;
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
  D = (double*)malloc(sizeof(double) * N * N);
  E = (double*)malloc(sizeof(double) * N * N);
  F = (double*)malloc(sizeof(double) * N * N);
  AB = (double*)malloc(sizeof(double) * N * N);
  CD = (double*)malloc(sizeof(double) * N * N);
  EF = (double*)malloc(sizeof(double) * N * N);
  R = (double*)malloc(sizeof(double) * N * N);

  // Inicializa las matrices A, B y C en 1
  // Se organiza a A,C,E por fila
  // Se organiza a B,D,F por columna
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      A[i * N + j] = 1;
      B[j * N + i] = 1;
      C[i * N + j] = 1;
      D[j * N + i] = 1;
      E[i * N + j] = 1;
      F[j * N + i] = 1;
    }
  }

  printf("Dimensión de las matrices: %ix%i\n\n", N, N);

  printf("Realizando suma R = AB.CD.EF . . .\n\n");

  timetick = dwalltime();

  // Realiza el producto AB=A.B
  for ( i = 0; i < N; i++) {

    for ( j = 0; j < N; j++) {
      double suma_parcial = 0;

      for ( k = 0; k < N; k++) {
        suma_parcial += A[i * N + k] * B[j * N + k];
      }

      AB[i * N + j] = suma_parcial;
    }
  }

  // Realiza el producto CD=C.D
  for ( i = 0; i < N; i++) {

    for ( j = 0; j < N; j++) {
      double suma_parcial = 0;

      for ( k = 0; k < N; k++) {
        suma_parcial += C[i * N + k] * D[j * N + k];
      }

      CD[i * N + j] = suma_parcial;
    }
  }

  // Realiza el producto EF=E.F
  for ( i = 0; i < N; i++) {

    for ( j = 0; j < N; j++) {
      double suma_parcial = 0;

      for ( k = 0; k < N; k++) {
        suma_parcial += E[i * N + k] * F[j * N + k];
      }

      EF[i * N + j] = suma_parcial;
    }
  }

  // Suma los productos R = AB.CD.EF
  for ( i = 0; i < N*N; i++) 
    R[i] = AB[i] + CD[i] + EF[i];

  printf("Tiempo en segundos %f\n", dwalltime() - timetick);

  if (resultado_valido(R)) {
    printf("Multiplicacion de matrices resultado correcto\n");
  } else {
    printf("Multiplicacion de matrices resultado erroneo\n");
  }

  free(A);
  free(B);
  free(C);
  free(D);
  free(E);
  free(F);
  free(AB);
  free(CD);
  free(EF);
  free(R);

  return (0);
}