#include <mpi.h>
#include <stdio.h>

int positive_mod(int i, int n){ return (i%n + n) % n; };

int main(int argc, char** argv){
	int send_id, recv_id, task_id, current_id, received, num_tasks, SUM;
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
	send_id = positive_mod(task_id + 1, num_tasks);
	recv_id = positive_mod(task_id - 1, num_tasks);
	// Inicializa a current_id
	current_id = task_id;
	// Enquanto não terminar...
	int TOTAL = (num_tasks+1)*num_tasks/2;
	for (int count = 0; count < num_tasks; count++){
		MPI_Send(&current_id, 1, MPI_INT, send_id, 0, 
		MPI_COMM_WORLD);
		MPI_Recv(&received, 1, MPI_INT, recv_id, 0,
		MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		// Atualiza a id atual
		current_id = received;
		// Atualiza soma
		SUM = SUM + received;
	}
	printf("Processo %d, soma atual %d\n", task_id, SUM);
	// Finaliza o ambiente MPI
	MPI_Finalize();
	return 0;
}