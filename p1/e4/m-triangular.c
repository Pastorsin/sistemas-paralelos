#include<stdio.h>
#include<stdlib.h>

int N=100;


//Para calcular tiempo
double dwalltime(){
        double sec;
        struct timeval tv;

        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}

//Para calcular la cantidad de 0 a restar
int calcularCeros(int columna){
        int ceros,tamMatriz,dimMatriz,cantElementos;
	dimMatriz=columna+1;
	tamMatriz=dimMatriz*dimMatriz;
        cantElementos=dimMatriz*(dimMatriz+1)/2;
	ceros=tamMatriz-cantElementos;
        return ceros;
}

int main(int argc,char* argv[]){

 double *A,*BT,*C,*Prueba;;
 int i,j,k,tamTriangular;
 int check = 1;
 double timetick;

 //Controla los argumentos al programa
 if ((argc != 2) || ((N = atoi(argv[1])) <= 0) ) {
    printf("\nUsar: %s n\n  n: Dimension de la matriz (nxn X nxn)\n", argv[0]);
    exit(1);
  }

 
 //Aloca memoria para las matrices
 A=(double*)malloc(sizeof(double)*N*N);
 BT=(double*)malloc(sizeof(double)*N*N);
 C=(double*)malloc(sizeof(double)*N*N);
 //Esta es la matraiz triangular en forma de vector guardando todo menos los ceros
 Prueba=(double*)malloc(sizeof(double)*tamTriangular);

 //Inicializa las matrices
 //La matriz A se inicializa todas las columnas en 0 menos la ultima
 //La matriz BT se inicializa triangular inferior con 1
 //Esto permite que el resultado sean todos 1
 //La matriz C se inicializa en 0
 for(i=0;i<N;i++){
  for(j=0;j<N;j++){

   if(j<N-1){
    A[i*N+j]=0;
   }else{
    A[i*N+j]=1;
   }

   if(i>=j){
    BT[j*N+i]=N;
   }
   
   //Para cargar la matriz triangular en forma de vector
   if(i>=j){
    Prueba[i - calcularCeros(j) + N*j]= i - calcularCeros(j) + N*j;
   }

   C[i*N+j]=0;  
  }
 }

 //Realiza la multiplicacion 
 timetick = dwalltime();


 for(i=0;i<N;i++){
  for(j=0;j<N;j++){
   for(k=0;k<N;k++){

    // Si está arriba de la diagonal, entonces el valor de B[k,j] es 0
    // C ya está inicializada en 0, entonces salteo las operaciones en diagonal

    // Esto evita:
    // - 1 asignación
    // - 2 operaciones Punto Flotante = 1 suma y 1 multiplicación
    // - Los 4 accesos a memoria (asignación + operadores)

    if (k >= j){
      C[i*N+j] = C[i*N+j] + A[i*N+k] * BT[j*N+k];
    }

   }
  }
 }

  //Este codigo es el que se encarga de recorrer las matrices sin iterar sobre los ceros de la matriz triangular
  //for(i=0;i<N;i++){
  //for(j=0;j<N;j++){
  //for(k=0;k<N-j;k++){
    //C[i*N+j]=C[i*N+j] + A[i*N+k+j]*BT[k+j+j*N];
   //}
  //}
 //}

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
