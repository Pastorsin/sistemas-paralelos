#include<stdio.h>
#include<stdlib.h>

//Dimension por defecto de las matrices
int N = 4;

//Para calcular tiempo
double dwalltime() {
  double sec;
  struct timeval tv;

  gettimeofday(&tv, NULL);
  sec = tv.tv_sec + tv.tv_usec / 1000000.0;
  return sec;
}

int resultado_valido(double *D) {
  for (int j = 0; j < N; j++)
  {
    if (D[0 * N + j] != 0)
      return 0;
    if (D[1 * N + j] != 1672)
      return 0;
    if (D[2 * N + j] != 3344)
      return 0;
    if (D[3 * N + j] != 5016)
      return 0;
  }
  return 1;
}

int imprimir(double *M) {
  for (int i = 0; i < N; i++)
  {
    for (int j = 0; j < N; j++)
    {
      printf("%ix%i --> %f\n", i, j, M[i * N + j]);
    }
  }
}

int main(int argc, char*argv[]) {
  double *A, *B, *C, *AB, *D;
  double maxA, minA, maxB, minB, maxC, minC;
  double totalA, totalB, totalC, avgA, avgB, avgC;
  int i, j, k;
  double d;
  double timetick;

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
  int cont = 0;
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      A[i * N + j] = i;
      B[i * N + j] = i+N;
      C[i * N + j] = i+2*N;
    }
  }


  printf("A:\n");
  imprimir(A);
  printf("B:\n");
  imprimir(B);
  printf("C:\n");
  imprimir(C);

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
  }

  // Calcular D=AB.C
  // AB por filas, C por columnas, D por filas

  for (i = 0; i < N; i++) {
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

  printf("AB:\n");
  imprimir(D);

  // Calcular d
  avgA = totalA / (N * N);
  avgB = totalB / (N * N);
  avgC = totalC / (N * N);

  d = ((maxA * maxB * maxC) - (minA * minB * minC)) / (avgA * avgB * avgC);
  printf("d --> %f\n", d);

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

  imprimir(D);

  free(A);
  free(B);
  free(C);
  free(AB);
  free(D);

  return (0);
}
