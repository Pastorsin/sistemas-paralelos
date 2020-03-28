#include<stdio.h>
#include<stdlib.h>
#include <math.h>

int N = 1048576;

double dwalltime();

static inline void init_array(double *array){
	// array[0] = 500;
	// array[1] = 10;
	// array[2] = 6;
	// array[3] = 3;
	// array[4] = 60;
	// array[5] = 2;
	// array[6] = 18;
	// array[7] = 3;
	for (int i = 0; i < N; i++)
	{
		array[i] = rand();
	}
}

static inline void print_array(double *array){
	for (int i = 0; i < N; i++){
		printf("%f\n", array[i]);
	}
}

int main(int argc, char const *argv[]){
	double *ARRAY;
	int STEPS = log2(N);


	ARRAY = (double*)malloc(sizeof(double)*N);
	init_array(ARRAY);

	double timetick = dwalltime();

	for (int step = STEPS; step > 0; step--){

		int elements = pow(2, step);
		
		for (int element = 0; element < elements; element += 2){

			// printf("--- %f / %f \n", ARRAY[element], ARRAY[element + 1]);

			ARRAY[element / 2] = ARRAY[element] / ARRAY[element + 1];
		}

		// printf("----\n");
	}

	printf("Final %f reduction in %fs\n", ARRAY[0] ,dwalltime() - timetick);

	free(ARRAY);
	
	return 0;
}

/*****************************************************************/

#include <sys/time.h>

double dwalltime()
{
	double sec;
	struct timeval tv;

	gettimeofday(&tv,NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}