#include<stdio.h>
#include<stdlib.h>

//Dimension por defecto de las matrices
int N = 100;

//Para calcular tiempo
double dwalltime() {
  double sec;
  struct timeval tv;

  gettimeofday(&tv, NULL);
  sec = tv.tv_sec + tv.tv_usec / 1000000.0;
  return sec;
}

int resultado_valido(double *D) {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (D[i * N + j] != 0)
        return 0;
    }
    return 1;
  }
}

int main(int argc, char*argv[]) {
  double *A, *B, *C, *AB, *D;
  double maxA, minA, maxB, minB, maxC, minC;
  double totalA, totalB, totalC, avgA, avgB, avgC;
  int i, j, k;
  int d;
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

  //Inicializa las matrices A,B y C en 1
  // A por filas
  // B por columnas
  // C por columnas
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      A[i * N + j] = 1;
      B[j * N + i] = 1;
      C[j * N + i] = 1;
    }
  }

  maxA = minA = A[0];
  maxB = minB = B[0];
  maxC = minC = C[0];
  totalA = 0;
  totalB = 0;
  totalC = 0;


  //Realiza la multiplicacion

  timetick = dwalltime();

  // Calcular AB=A.B
  // A por filas, B por columnas, AB por filas

  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      double suma_parcial = 0;

      // Mínimo, Máximo y Suma de B
      if (B[i * N + j] < minB)
        minB = B[i * N + j];

      if (B[i * N + j] > maxB)
        maxB = B[i * N + j];

      totalB += B[i * N + j];

      // Mínimo, Máximo y Suma de A
      if (A[i * N + j] < minA)
        minA = A[i * N + j];

      if (A[i * N + j] > maxA)
        maxA = A[i * N + j];

      totalA += A[i * N + j];

      // Multiplicación
      for (k = 0; k < N; k++) {
        suma_parcial += A[i * N + k] * B[j * N + k];
      }

      AB[i * N + j] = suma_parcial;
    }

    // Calcular D=AB.C
    // AB por filas, C por columnas, D por filas
    for (j = 0; j < N; j++) {
      double suma_parcial = 0;

      // Mínimo, Máximo y Suma de C
      if (C[i * N + j] < minC)
        minC = C[i * N + j];

      if (C[i * N + j] > maxC)
        maxC = C[i * N + j];

      totalC += C[i * N + j];

      // Multiplicación
      for (k = 0; k < N; k++) {
        suma_parcial += AB[i * N + k] * C[j * N + k];
      }

      D[i * N + j] = suma_parcial;
    }
  }

  // Calcular d
  avgA = totalA / (N * N);
  avgB = totalB / (N * N);
  avgC = totalC / (N * N);

  d = ((maxA * maxB * maxC) - (minA * minB * minC)) / (avgA * avgB * avgC);

  // Calcular D=d.D
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      D[i * N + j] *= d;
    }
  }

  printf("Multiplicacion de matrices de %dx%d. Tiempo en segundos %f\n", N, N, dwalltime() - timetick);

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
