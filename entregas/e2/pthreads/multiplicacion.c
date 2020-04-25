#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

// Dimension por defecto de las matrices
int N = 256;

// Cantidad de Threads
int T = 4;

// Variables compartidas
double *A, *B, *C, *AB, *D;
double maxA, minA, maxB, minB, maxC, minC;
double totalA, totalB, totalC;
double d;

// Locks
pthread_mutex_t mutex_maxA, mutex_minA, mutex_maxB, mutex_minB, mutex_maxC, mutex_minC;
pthread_mutex_t mutex_totalA, mutex_totalB, mutex_totalC;
pthread_barrier_t barrera;

//Para calcular tiempo
double dwalltime() {
  double sec;
  struct timeval tv;

  gettimeofday(&tv, NULL);
  sec = tv.tv_sec + tv.tv_usec / 1000000.0;
  return sec;
}


void multiplicar(void *ptr) {
  int id = * (int *) ptr;
  int i,j,k;

  double maxA_local, minA_local, maxB_local, minB_local, maxC_local, minC_local;
  double totalA_local, totalB_local, totalC_local;

  // Strip del Thread i
  int principio = id * N / T;
  int final = (id + 1) * N / T;

  maxA_local = minA_local = maxA;
  maxB_local = minB_local = maxB;
  maxC_local = minC_local = maxC;
  totalA_local = 0;
  totalB_local = 0;
  totalC_local = 0;


  // Calcular strip AB=A.B
  // A por filas, B por columnas, AB por filas
  for (i = principio; i < final; i++) {
    for (j = 0; j < N; j++) {
      double suma_parcial = 0;

      // Mínimo, Máximo y Suma de B
      if (B[i * N + j] < minB_local)
        minB_local = B[i * N + j];

      if (B[i * N + j] > maxB_local)
        maxB_local = B[i * N + j];

      totalB_local += B[i * N + j];

      // Mínimo, Máximo y Suma de A
      if (A[i * N + j] < minA_local)
        minA_local = A[i * N + j];

      if (A[i * N + j] > maxA_local)
        maxA_local = A[i * N + j];

      totalA_local += A[i * N + j];

      // Multiplicación
      for (k = 0; k < N; k++) {
        suma_parcial += A[i * N + k] * B[j * N + k];
      }

      AB[i * N + j] = suma_parcial;
    }
  }

  // Calcular strip D=AB.C
  // AB por filas, C por columnas, D por filas
  for (i = principio; i < final; i++) {
    for (j = 0; j < N; j++) {
      double suma_parcial = 0;

      // Mínimo, Máximo y Suma de C
      if (C[i * N + j] < minC_local)
        minC_local = C[i * N + j];

      if (C[i * N + j] > maxC_local)
        maxC_local = C[i * N + j];

      totalC_local += C[i * N + j];

      // Multiplicación
      for (k = 0; k < N; k++) {
        suma_parcial += AB[i * N + k] * C[j * N + k];
      }

      D[i * N + j] = suma_parcial;
    }
  }

  // Comunicación de variables
  if (id != 0) {

    // Mínimos
    pthread_mutex_lock(&mutex_minA);
    if (minA_local < minA)
      minA = minA_local;
    pthread_mutex_unlock(&mutex_minA);

    pthread_mutex_lock(&mutex_minB);
    if (minB_local < minB)
      minB = minB_local;
    pthread_mutex_unlock(&mutex_minB);

    pthread_mutex_lock(&mutex_minC);
    if (minC_local < minC)
      minC = minC_local;
    pthread_mutex_unlock(&mutex_minC);

    // Máximos
    pthread_mutex_lock(&mutex_maxA);
    if (maxA_local > maxA)
      maxA = maxA_local;
    pthread_mutex_unlock(&mutex_maxA);

    pthread_mutex_lock(&mutex_maxB);
    if (maxB_local > maxB)
      maxB = maxB_local;
    pthread_mutex_unlock(&mutex_maxB);

    pthread_mutex_lock(&mutex_maxC);
    if (maxC_local > maxC)
      maxC = maxC_local;
    pthread_mutex_unlock(&mutex_maxC);

    // Totales
    if (totalA_local > 0) {
      pthread_mutex_lock(&mutex_totalA);
      totalA += totalA_local;
      pthread_mutex_unlock(&mutex_totalA);
    }

    if (totalB_local > 0) {
      pthread_mutex_lock(&mutex_totalB);
      totalB += totalB_local;
      pthread_mutex_unlock(&mutex_totalB);
    }

    if (totalC_local > 0) {
      pthread_mutex_lock(&mutex_totalC);
      totalC += totalC_local;
      pthread_mutex_unlock(&mutex_totalC);
    }
  }

  pthread_barrier_wait(&barrera);

  if (id == 0) {

    // Máximos
    if (maxA_local > maxA)
      maxA = maxA_local;
    if (maxB_local > maxB)
      maxB = maxB_local;
    if (maxC_local > maxC)
      maxC = maxC_local;

    // Mínimos
    if (minA_local < minA)
      minA = minA_local;
    if (minB_local < minB)
      minB = minB_local;
    if (minC_local < minC)
      minC = minC_local;

    // Totales
    totalA += totalA_local;
    totalB += totalB_local;
    totalC += totalC_local;

    // Calcular d
    double avgA = totalA / (N * N);
    double avgB = totalB / (N * N);
    double avgC = totalC / (N * N);

    d = ((maxA * maxB * maxC) - (minA * minB * minC)) / (avgA * avgB * avgC);
  }

  pthread_barrier_wait(&barrera);

  // Calcular strip D=d.D
  for (i = principio; i < final; i++) {
    for (j = 0; j < N; j++) {
      D[i * N + j] *= d;
    }
  }

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
  double timetick;
  int ids[T], i, j;
  pthread_t multiplicacion_thread[T];

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

  pthread_barrier_init(&barrera, NULL, T);

  //Realiza la multiplicacion
  timetick = dwalltime();

  for (i = 0; i < T; i++){
    ids[i] = i;
    pthread_create(&multiplicacion_thread[i], NULL, multiplicar, &ids[i]);
  }

  for (i = 0; i < T; i++)
    pthread_join(multiplicacion_thread[i], NULL);


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

  pthread_barrier_destroy(&barrera);

  return (0);
}
