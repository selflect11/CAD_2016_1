#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#ifndef MATRIX_SIZE
#	define MATRIX_SIZE 4
#endif
#define FROM_MASTER 0
#define FROM_WORKER 1
#define MASTER 0
#define bool int
#define True 1
#define False 0
float **allocateMatrix(int N, bool isRandom);
int positive_mod(int i, int n){ return (i%n + n) % n; };

int main(int argc, char** argv){
	int send_partner,
	 	recv_partner,
	 	task_id,				// Identidade do proprio processo
	 	num_tasks,			// Numero total de processos
	 	num_workers,			// Numero de trabalhadores (tasks - 1)
	 	num_rows_sending,		// Numero de linhas p cada processo
	 	message_type,			// FROM_WORKER ou FROM_MASTER
	 	offset;					// Offset do endereco dos buffers'
	//float **A, **B, **C;
	float A[MATRIX_SIZE][MATRIX_SIZE],
		B[MATRIX_SIZE][MATRIX_SIZE],
		C[MATRIX_SIZE][MATRIX_SIZE];

	// Inicializa o ambiente MPI
	MPI_Init(NULL, NULL);

	MPI_Comm_rank(MPI_COMM_WORLD, &task_id);
	MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
	num_workers = num_tasks - 1;
	if ((num_tasks < 2) || ( num_workers % 4 != 0)){
		printf("Num de processo invalido\n");
		printf("Este programa precisa de no minimo 2 processos\n");
		printf("O num de processos - 1 deve ser divisivel por 4\n");
		MPI_Finalize();
		return 0;
	}

/********************************PROCESSO MESTRE**********************************/
	if (task_id == MASTER){
		// Coloca valores nas matrizes
		for (int i = 0; i < MATRIX_SIZE; i++){
			for (int j = 0; j < MATRIX_SIZE; j++){
				A[i][j] = i + j;
				B[i][j] = i*j;
			}
		}
		// Manda dados para os outros processos
		num_rows_sending = MATRIX_SIZE / num_workers;
		printf("Avg rows: %d, extra: %d\n", num_rows_sending, MATRIX_SIZE % num_workers);
		message_type = FROM_MASTER;
		offset = 0;
		// Mandando
		for (int send_id = 1; send_id <= num_workers; send_id++){
			printf("Mandando %d linhas para processo %d, offset: %d\n", num_rows_sending, send_id, offset);
			MPI_Send(&offset, 1, MPI_INT, send_id, message_type, MPI_COMM_WORLD);			
			// Manda o numero de linhas que o processo vai ler
			MPI_Send(&num_rows_sending, 1, MPI_INT, send_id, message_type, MPI_COMM_WORLD);
			// Manda o endereco do buffer da matriz A com offset de linhas
			MPI_Send(&A[offset][0], num_rows_sending*MATRIX_SIZE, MPI_FLOAT, send_id, message_type,
				MPI_COMM_WORLD);
			// Manda endereço de B
			MPI_Send(&(B[0][0]), MATRIX_SIZE*MATRIX_SIZE, MPI_FLOAT, send_id, message_type,
				MPI_COMM_WORLD);
			offset = offset + num_rows_sending;
		}

		// Recebe dados dos outros processos
		message_type = FROM_WORKER;
		for (int recv_id = 1; recv_id <= num_workers; recv_id++){
			MPI_Recv(&offset, 1, MPI_INT, recv_id, message_type, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&num_rows_sending, 1, MPI_INT, recv_id, message_type, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&C[offset][0], num_rows_sending*MATRIX_SIZE, MPI_FLOAT, recv_id, message_type, 
				MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			printf("Resultados recebidos do processo: %d\n", recv_id);
		}

		// Printa resultado
		printf("Matriz resultado:\n");
		for (int i = 0; i < MATRIX_SIZE; i++){
			printf("\n");
			for (int j = 0; j < MATRIX_SIZE; j++){
				printf("%.2f    ", C[i][j]);
			}
		}
		printf("\n****************************\n");
	}

/****************************PROCESSO TRABALHADOR*****************************/
	if (task_id != MASTER){
		message_type = FROM_MASTER;
		MPI_Recv(&offset, 1, MPI_INT, MASTER, message_type, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&num_rows_sending, 1, MPI_INT, MASTER, message_type, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&(A[0][0]), num_rows_sending*MATRIX_SIZE, MPI_FLOAT, MASTER, message_type, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&(B[0][0]), MATRIX_SIZE*MATRIX_SIZE, MPI_FLOAT, MASTER, message_type, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		// Calcula o produto
		for (int k = 0; k < MATRIX_SIZE; k++)
			for (int i = 0; i < MATRIX_SIZE; i++){
				C[i][k] = 0.0;
				for (int j = 0; j < MATRIX_SIZE; j++){
					C[i][k] = C[i][k] + A[i][j]*B[j][k];
				}
			}
		message_type = FROM_WORKER;
		MPI_Send(&offset, 1, MPI_INT, MASTER, message_type, MPI_COMM_WORLD);
		MPI_Send(&num_rows_sending, 1, MPI_INT, MASTER, message_type, MPI_COMM_WORLD);
		MPI_Send(&C, num_rows_sending*MATRIX_SIZE, MPI_FLOAT, MASTER, message_type, MPI_COMM_WORLD);
	}
	// Finaliza o ambiente MPI
	MPI_Finalize();
	return 0;
}


/***************NAO FUNCIONANDO!!!!**********************/
float **allocateMatrix(int ARRAY_SIZE, bool isRandom){
	srand(time(NULL));
	float *data = (float *) malloc(ARRAY_SIZE*ARRAY_SIZE * sizeof(float));
	float **array = (float **) malloc(ARRAY_SIZE * sizeof(float *));
	for (int i = 0; i < ARRAY_SIZE; i++){
		array[i] = &(data[ARRAY_SIZE * i]);
		if (isRandom){
			for (int j = 0; j < ARRAY_SIZE; j++){
				data[j + i*ARRAY_SIZE] = (float) (random() % 10)/ 7;
			}
		}
	}
	return array;
}