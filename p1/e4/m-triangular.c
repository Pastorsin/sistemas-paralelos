#include<stdio.h>
#include<stdlib.h>

int N=3;


//Para calcular tiempo
double dwalltime(){
        double sec;
        struct timeval tv;

        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}

//Para calcular la cantidad de 0 a restar
int cantidad_ceros(int columna){
  int ceros,tamMatriz,dimMatriz,cantElementos;

	dimMatriz=columna+1;
	tamMatriz=dimMatriz*dimMatriz;
  cantElementos=dimMatriz*(dimMatriz+1)/2;
	ceros=tamMatriz-cantElementos;

  return ceros;
}

int main(int argc,char* argv[]){

 double *A,*BT,*C,*Prueba;
 int i,j,k;

 int check = 1;
 double timetick;

  //Controla los argumentos al programa
  if ((argc != 2) || ((N = atoi(argv[1])) <= 0) ){
    printf("\nUsar: %s n\n  n: Dimension de la matriz (nxn X nxn)\n", argv[0]);
    exit(1);
  }


 // Alocación de memoria para las matrices
 int TRIANGULAR_SIZE = ((N*N)+N) / 2;
 printf("%i\n", N);

 A=(double*)malloc(sizeof(double)*N*N);
 BT=(double*)malloc(sizeof(double)*TRIANGULAR_SIZE);
 C=(double*)malloc(sizeof(double)*N*N);


 //Inicializa las matrices
 //La matriz A se inicializa todas las columnas en 0 menos la ultima
 //La matriz BT se inicializa triangular inferior con N
 //Esto permite que el resultado sean todos 1
 //La matriz C se inicializa en 0
 for(i=0;i<N;i++){
  for(j=0;j<N;j++){

   if(j<N-1){
    A[i*N+j]=0;
   }else{
    A[i*N+j]=1;
   }

 
   //Para cargar la matriz triangular en forma de vector
   if(i >= j){
    int index = i - cantidad_ceros(j) + N*j;
    BT[index] = N;
   }

   C[i*N+j]=0;  
  }
 }

  timetick = dwalltime();


  // Codigo optimizado:
  // - Se encarga de recorrer las matrices sin iterar sobre los ceros de la matriz triangular
  for(i=0; i<N; i++){

    for(j=0; j<N; j++){

      // Notar que solo recorre las diagonales


      // for(k=0; k<(N-j); k++){

      //   C[i*N+j] = C[i*N+j] + A[i*N+k+j] * BT[k+j+j*N];

      // }

      for(k=0; k<N; k++)
      {
        if (k >= j){
          int index = k - cantidad_ceros(j) + N*j;
          C[i*N+j] = C[i*N+j] + A[i*N+k] * BT[index];
        }
      }
    }
 }

 printf("Tiempo en segundos %f\n", dwalltime() - timetick);

 //Verifica el resultado
 for(i=0;i<N;i++){
  for(j=0;j<N;j++){
   check = check && (C[i*N+j]==N);
  }
 }

 if(check){
  printf("Multiplicacion de matriz triangular correcta\n");
 }else{
  printf("Multiplicacion de matriz triangular erroneo\n");
 }
 
 free(A);
 free(BT);
 free(C);

 return(0);
}
