//Ejercicio 2
#include<stdio.h>
#include<stdlib.h>
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
 int N=atoi(argv[1]);
 scale=2.78;
 x=0.0;

 double timetick = dwalltime();
 for(i=1;i<=N;i++){
	x = sqrt(i*scale) + 3*x;
 }
 printf("%f\n", dwalltime() - timetick);

 printf("\n Resultado: %f \n",x);

 return(0);
}

