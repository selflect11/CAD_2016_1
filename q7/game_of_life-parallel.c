#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>

#define NI 700        /* tamanho dos array  */
#define NJ 700
#define MASTER 0
#define NSTEPS 500    /* Numero de iteracoes */

int positive_mod(int i, int n){ return (i%n + n) % n; };

int main(int argc, char *argv[])
{
    MPI_Init(NULL, NULL);

    int i, j, n, im, ip, jm, jp, ni, nj,
        nsum,
        local_sum,
        num_tasks,
        task_id,
        subdomain_size,
        subdomain_excess,
        real_ni,
        real_nj,
        next_id,
        prev_id,
        total_sum;
    int **old_arr, **new_arr;
    MPI_Request reqs[4];
    MPI_Status stats[4];
    float x;
    struct timeval start_time, times[4]; 
    MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &task_id);

    // Pega tempo de inicio
    gettimeofday(&start_time, NULL);

    total_sum = 0;
    char outfilename[40];
    sprintf(outfilename, "files/found.data_%d", task_id);
    FILE* outfile = fopen(outfilename, "w");
    fprintf(outfile, "\n\nProcesso %d inicializado\n Start Time:%.06ld\n", task_id, start_time.tv_sec);
    fflush(outfile);
    // todos os processos recebem (floor(NI / num_tasks) + 2) linhas
    // exceto processo MESTRE, que receberá todo o restante

    subdomain_size = NI / num_tasks;
    subdomain_excess = NI % num_tasks;
    /* alocacao */

    ni = (subdomain_size) + 2;  /* celulas fantasmas na borda  */
    nj = NJ + 2;
    if (task_id == MASTER)
    {
        ni = (subdomain_excess == 0) ? ni : ni + subdomain_excess;
        printf("Total de processos: %d \n",  num_tasks);
    }
    fprintf(outfile, "Sou processo %d, tenho %i linhas\n", task_id, ni);
    fflush(outfile);
    real_ni = ni - 2;
    real_nj = nj - 2;

    old_arr = malloc(ni*sizeof(int*));
    new_arr = malloc(ni*sizeof(int*));

    for(i=0; i<ni; i++){
    old_arr[i] = malloc(nj*sizeof(int));
    new_arr[i] = malloc(nj*sizeof(int));
    }
    // 
    gettimeofday(&times[0], NULL);
    fprintf(outfile, "Memoria alocada \n Delta_t:%.*e s", times[0].tv_sec - start_time.tv_sec);
    fflush(outfile);
    /*  inicializando elementos  */

    for(i=1; i<=real_ni; i++)
    {
        for(j=1; j<=real_nj; j++)
        {
            x = rand()/((float)RAND_MAX + 1);
            if(x<0.5){
                old_arr[i][j] = 0;
            } else {
                old_arr[i][j] = 1;
            }
        }
    }
    // Ids de envio e recebimento
    next_id = positive_mod(task_id + 1, num_tasks);
    prev_id = positive_mod(task_id - 1, num_tasks);

    /* */
    for(n=0; n<NSTEPS; n++)
    {

        /* cond. contorno para faces direita/esquerda  */

        for(i=1; i<=real_ni; i++){
            old_arr[i][0]    = old_arr[i][real_nj];
            old_arr[i][nj-1] = old_arr[i][1];
        }

        /*Exchange data between cells*/

        //up
        MPI_Isend(old_arr[1], nj, MPI_INT, prev_id, n, MPI_COMM_WORLD, &reqs[0]);
        MPI_Irecv(old_arr[0], nj, MPI_INT, prev_id, n, MPI_COMM_WORLD, &reqs[1]);
        //down
        MPI_Isend(old_arr[real_ni], nj, MPI_INT, next_id, n, MPI_COMM_WORLD, &reqs[2]);
        MPI_Irecv(old_arr[real_ni + 1], nj, MPI_INT, next_id, n, MPI_COMM_WORLD, &reqs[3]);

        MPI_Waitall(4, reqs, stats);
        // tempo
        if (n % 100 == 0){
          gettimeofday(&times[1], NULL);
          fprintf(outfile, "Mensagens trocadas \n Delta_t:%.*e s", times[1].tv_sec - start_time.tv_sec);
          fflush(outfile);
        }
        /* calculating self-enclosed data */
        for(i=2; i<=ni-2; i++)
        {
            for(j=1; j<=nj-1; j++)
            {
                im = i-1;
                ip = i+1;
                jm = j-1;
                jp = j+1;

                nsum =  old_arr[im][jp] + old_arr[i][jp] + old_arr[ip][jp]
                + old_arr[im][j ]              + old_arr[ip][j ]
                + old_arr[im][jm] + old_arr[i][jm] + old_arr[ip][jm];

                switch(nsum)
                {
                    case 3:
                    new_arr[i][j] = 1;
                    break;
                    case 2:
                    new_arr[i][j] = old_arr[i][j];
                    break;
                    default:
                    new_arr[i][j] = 0;
                }
            }
        }

        /* condicoes de controno para as esquinas do dominio */

        if (task_id == 0)
        {
            old_arr[0][0]    = old_arr[real_ni][real_nj];
            old_arr[0][nj-1] = old_arr[real_ni][1];
        }
        if (task_id == num_tasks - 1)
        {
            old_arr[ni-1][nj-1] = old_arr[1][1];
            old_arr[ni-1][0]    = old_arr[1][real_nj];
        }

        /* calculating the rest */
        for(i=1; i< real_ni + 1; i += (real_ni - 1))
        {
            // for i in (1, real_ni)
            for(j=1; j<=nj-1; j++)
            {
                im = i-1;
                ip = i+1;
                jm = j-1;
                jp = j+1;

                nsum =  old_arr[im][jp] + old_arr[i][jp] + old_arr[ip][jp]
                + old_arr[im][j ]              + old_arr[ip][j ]
                + old_arr[im][jm] + old_arr[i][jm] + old_arr[ip][jm];

                switch(nsum)
                {
                    case 3:
                    new_arr[i][j] = 1;
                    break;
                    case 2:
                    new_arr[i][j] = old_arr[i][j];
                    break;
                    default:
                    new_arr[i][j] = 0;
                }
            }
        }

        /* copia estado  */
        for(i=1; i<=real_ni; i++)
        {
            for(j=1; j<=real_nj; j++)
            {
                old_arr[i][j] = new_arr[i][j];
            }
        }
    }

    gettimeofday(&times[2], NULL);
    fprintf(outfile, "Fim do loop principal \n Delta_t:%.*e s", times[2].tv_sec - start_time.tv_sec);
    fflush(outfile);

    /*  Conta o número de celulas  vivas no final */
    local_sum = 0;
    for(i=1; i<=real_ni; i++){
        for(j=1; j<=real_nj; j++){
            local_sum = local_sum + new_arr[i][j];
        }
    }
    printf("Celulas vivas para o processo [%d]: %d\n", task_id, local_sum);
    gettimeofday(&times[3], NULL);
    fprintf(outfile, "Celulas vivas para o processo[%d]: %d\n Delta_t:%.*e s", 
        task_id, local_sum, times[3].tv_sec - start_time.tv_sec);
    fflush(outfile);

    MPI_Reduce(&local_sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (task_id == MASTER)
    {
        printf("Total de celulas vivas: %d\n", total_sum);
    }

    for(i=0; i<ni; i++){
        free(old_arr[i]);
        free(new_arr[i]);
    }
    free(old_arr);
    free(new_arr);

    MPI_Finalize();

    return 0;
    }