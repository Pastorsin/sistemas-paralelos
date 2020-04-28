#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

//Dimension por defecto de las matrices
int N = 2;

//Para calcular tiempo
double dwalltime() {
  double sec;
  struct timeval tv;

  gettimeofday(&tv, NULL);
  sec = tv.tv_sec + tv.tv_usec / 1000000.0;
  return sec;
}

int resultado_valido(double *D) {
  return (D[0] == 24 && D[1] == 88 && D[2] == 104 && D[3] == 360);
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
  int d;
  double timetick;

  //Controla los argumentos al programa
  if (argc < 2) {
    printf("\n Faltan argumentos:: T cantidad de threads \n");
    return 0;
  }
  int numThreads = atoi(argv[1]);
  omp_set_num_threads(numThreads);

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
      A[i * N + j] = cont;
      B[i * N + j] = cont;
      C[i * N + j] = cont;
      cont++;
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

  #pragma omp parallel
  {
    // Calcular AB=A.B
    // A por filas, B por columnas, AB por filas
    #pragma omp for private(i,j,k) reduction(+:totalA,totalB) reduction(max:maxA,maxB) reduction(min:minA,minB) nowait
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
    #pragma omp for private(i,j,k) reduction(+:totalC) reduction(max:maxC) reduction(min:minC)
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

    // Calcular d
    #pragma omp single
    {
      avgA = totalA / (N * N);
      avgB = totalB / (N * N);
      avgC = totalC / (N * N);

      d = ((maxA * maxB * maxC) - (minA * minB * minC)) / (avgA * avgB * avgC);
    }

    // Calcular D=d.D
    #pragma omp for private(i,j)
    for (i = 0; i < N; i++) {
      for (j = 0; j < N; j++) {
        D[i * N + j] *= d;
      }
    }
  }

  imprimir(D);

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
