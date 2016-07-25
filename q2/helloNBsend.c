#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv){
	int partner, received, task_id, num_tasks;
	MPI_Request reqs[2];
	MPI_Status stats[2];
	// Inicializa o ambiente MPI
	MPI_Init(NULL, NULL);

	MPI_Comm_rank(MPI_COMM_WORLD, &task_id);
	MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);

	if (num_tasks < 2){
		printf("Este programa precisa de no minimo 2 processos\n");
		MPI_Finalize();
		return 0;
	}
	// Mapeia os parceiros
	if (task_id < num_tasks/2){
		partner = num_tasks/2 + task_id;
	} else {
		partner = task_id - num_tasks/2;
	}
	
	
	MPI_Isend(&task_id, 1, MPI_INT, partner, 0, 
		MPI_COMM_WORLD, &reqs[0]);
	MPI_Irecv(&received, 1, MPI_INT, partner, 0,
		MPI_COMM_WORLD, &reqs[1]);
	MPI_Wait(&reqs[1], &stats[1]);
	MPI_Wait(&reqs[0], &stats[0]);
	printf("Processo %d tem parceria com"
		" processo %d\n", task_id, received);

	// Espera td terminar
	//MPI_Waitall(2, reqs, stats);
	// Finaliza o ambiente MPI
	MPI_Finalize();
	return 0;
}