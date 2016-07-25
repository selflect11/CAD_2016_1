#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define MATRIX_SIZE 10
float **allocateMatrix(int N, int r);
int positive_mod(int i, int n){ return (i%n + n) % n; };

int main(int argc, char** argv){
	int send_partner, recv_partner, task_id, num_tasks;
	float **A, **B;

	// Inicializa o ambiente MPI
	MPI_Init(NULL, NULL);
	// Indices aleatorios da matriz
	srand(time(NULL));
	int rand_i = random() % MATRIX_SIZE;
	int rand_j = random() % MATRIX_SIZE;

	MPI_Comm_rank(MPI_COMM_WORLD, &task_id);
	MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);

	// Aloca buffers de envio e recebimento
	A = allocateMatrix(MATRIX_SIZE, task_id);
	B = allocateMatrix(MATRIX_SIZE, 0);

	if (num_tasks < 2){
		printf("Este programa precisa de no minimo 2 processos\n");
		MPI_Finalize();
		return 0;
	}

	// Mapeia os parceiros
	send_partner = positive_mod(task_id + 1, num_tasks);
	recv_partner = positive_mod(task_id - 1, num_tasks);
	
	MPI_Send(&(A[0][0]), MATRIX_SIZE*MATRIX_SIZE, MPI_FLOAT, send_partner, 0, 
		MPI_COMM_WORLD);

	MPI_Recv(&(B[0][0]), MATRIX_SIZE*MATRIX_SIZE, MPI_FLOAT, recv_partner, MPI_ANY_TAG,
		MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	// Printa o resultado	
	printf("Processo %d tem parceria com"
		" processo %.0f\n", task_id, B[rand_i][rand_j]);
	// Finaliza o ambiente MPI
	free(A[0]);
	free(A);
	free(B[0]);
	free(B);
	MPI_Finalize();
	return 0;
}

// NAO FUNCIONANDO
float **allocateMatrix(int ARRAY_SIZE, int rank){
	float *data = (float *) malloc(ARRAY_SIZE*ARRAY_SIZE * sizeof(float));
	float **array = (float **) malloc(ARRAY_SIZE * sizeof(float *));
	for (int i = 0; i < ARRAY_SIZE; i++){
		array[i] = &(data[ARRAY_SIZE * i]);
		for (int j = 0; j < ARRAY_SIZE; j++){
			data[j + i*ARRAY_SIZE] = (float) rank;
		}
	}
	return array;
}