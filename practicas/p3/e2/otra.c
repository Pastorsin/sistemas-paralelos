//Ejercicio 2
#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include<math.h>


//Para calcular tiempo
double dwalltime(){
  double sec;
  struct timeval tv;

  gettimeofday(&tv,NULL);
  sec = tv.tv_sec + tv.tv_usec/1000000.0;
  return sec;
}


int main(int argc,char*argv[]){
 double x,scale;
 int i;
 int numThreads = atoi(argv[2]);
 int N=atoi(argv[1]);
 omp_set_num_threads(numThreads);
 scale=2.78;
 x=0.0;

 double roots[N];

 double timetick = dwalltime();

 #pragma omp parallel for
 for(i=1; i<=N; i++){
    roots[i-1] = sqrt(i*scale);
 }

 for (int j=1; j < N; j++) {
   roots[j] = roots[j] + roots[j-1]*3;
 }

 printf("%f\n", dwalltime() - timetick);


 x = roots[N-1];

 printf("\n Resultado: %f \n",x);

 return(0);
}