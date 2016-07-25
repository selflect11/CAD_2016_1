#include <mpi.h>
#include <stdio.h>

#ifndef ARRAY_SIZE
#	define ARRAY_SIZE 4
#endif
#define FROM_MASTER 0
#define FROM_WORKER 1
#define MASTER 0
#define bool int
#define True 1
#define False 0

float dot_product(float *X, float *Y, int N);

int main(int argc, char** argv){
	int task_id,
		num_tasks,
		local_N,
		message_type,
		offset;
	float local_dot_product, global_dot_product;

	float A[ARRAY_SIZE], B[ARRAY_SIZE];
	// Inicializa o ambiente MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &task_id);
	
	// Checa se numero de processos é valido
	if ((num_tasks < 2 ) || (ARRAY_SIZE % num_tasks != 0)){
		printf("Numero de processos invalido\n");
		MPI_Finalize();
		return 0;
	}
	// Inicializa outras vars
	local_N = ARRAY_SIZE / num_tasks;
	float local_X[local_N],
		local_Y[local_N];
	
/*********************PROCESSO MESTRE**********************/
	//srand(time(NULL));
	if (task_id == MASTER){
		// Aloca os vetores
		for (int i=0; i < ARRAY_SIZE; i++){
		A[i] = i * i;
		B[i] = 2*i +1;
		printf("A[%d]=%.2f  ", i, A[i]);
		printf("B[%d]=%.2f  ", i, B[i]);
		}
	}
	// Distribui
	MPI_Scatter(A, local_N, MPI_FLOAT,
		local_X, local_N, MPI_FLOAT, 0,
		MPI_COMM_WORLD);
	MPI_Scatter(B, local_N, MPI_FLOAT,
		local_Y, local_N, MPI_FLOAT, 0,
		MPI_COMM_WORLD);
	
	// Faz o produto escalar localmente
	local_dot_product = dot_product(local_X, local_Y, local_N);
	// Faz reduce dos resultados
	MPI_Allreduce(&local_dot_product, &global_dot_product, 1,
			MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD);
	
	// Imprime o resultado
	if (task_id == MASTER){
		printf("\nProduto escalar = %.2f\n", global_dot_product);
	}
	// Finaliza o ambiente MPI
	MPI_Finalize();
	return 0;
}

float dot_product(float *X, float *Y, int N){
	float sum = 0;
	for (int i = 0; i < N; i++){
		sum += X[i]*Y[i];
	}
	return sum;
}