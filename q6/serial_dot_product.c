#include <stdio.h>

#ifndef ARRAY_SIZE
#	define ARRAY_SIZE 4
#endif
#define True 1
#define False 0

float dot_product(float *X, float *Y, int N);

int main(int argc, char** argv){
	float A[ARRAY_SIZE], B[ARRAY_SIZE],
		dot_prod;
	for (int i = 0; i < ARRAY_SIZE; i++){
		A[i] = i*i;
		B[i] = 2*i + 1;
		printf("A[%d]=%.2f\n", i, A[i]);
		printf("B[%d]=%.2f\n", i, B[i]);
	}
	dot_prod = dot_product(A, B, ARRAY_SIZE);
	printf("Produto escalar: %.2f\n", dot_prod);
	return 0;
}

float dot_product(float *X, float *Y, int N){
	float sum = 0;
	for (int i = 0; i < N; i++){
		sum += X[i]*Y[i];
	}
	return sum;
}