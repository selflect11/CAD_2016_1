/***********************

Jogo da Vida
Versão sequencial 

************************/

#include <stdio.h>

#include <stdlib.h>

#define NI 700        /* tamanho dos array  */
#define NJ 700

#define NSTEPS 500    /* Numero de iteracoes */

int main(int argc, char *argv[]) 
{

  int i, j, n, im, ip, jm, jp, ni, nj, nsum, isum;
  int **old, **new;  
  float x;

  /* alocacao */

  ni = NI + 2;  /* celulas fantasmas na borda  */
  nj = NJ + 2;
  old = malloc(ni*sizeof(int*));
  new = malloc(ni*sizeof(int*));

  for(i=0; i<ni; i++){
    old[i] = malloc(nj*sizeof(int));
    new[i] = malloc(nj*sizeof(int));
  }

/*  inicializando elementos  */

  for(i=1; i<=NI; i++)
  {
    for(j=1; j<=NJ; j++)
    {
       x = rand()/((float)RAND_MAX + 1);
       if(x<0.5){
	  old[i][j] = 0;
       } else {
	  old[i][j] = 1;
       }
    }
  }

  /* */
  for(n=0; n<NSTEPS; n++)
  {

    /* condicoes de controno para as esquinas do dominio */
    old[0][0]       = old[NI][NJ];
    old[0][NJ+1]    = old[NI][1];
    old[NI+1][NJ+1] = old[1][1];
    old[NI+1][0]    = old[1][NJ];

    /* cond. contorno para faces direita/esquerda  */

    for(i=1; i<=NI; i++){
      old[i][0]    = old[i][NJ];
      old[i][NJ+1] = old[i][1];
    }

    /* cond. controno face inferior e superior */

    for(j=1; j<=NJ; j++){
      old[0][j]    = old[NI][j];
      old[NI+1][j] = old[1][j];
    }

    // Codigo Paralelo: Trocar elementos da interface paralela

    for(i=1; i<=NI; i++)
    {
       for(j=1; j<=NJ; j++)
       {
	  im = i-1;
	  ip = i+1;
	  jm = j-1;
	  jp = j+1;

	  nsum =  old[im][jp] + old[i][jp] + old[ip][jp]
	        + old[im][j ]              + old[ip][j ] 
	        + old[im][jm] + old[i][jm] + old[ip][jm];

	  switch(nsum)
          {
             case 3:
	        new[i][j] = 1;
	     break;
	     case 2:
	        new[i][j] = old[i][j];
	     break;
             default:
	       new[i][j] = 0;
	  }
       }
    }

    /* copia estado  */
    for(i=1; i<=NI; i++){
      for(j=1; j<=NJ; j++){
	old[i][j] = new[i][j];
      }
    }
  }

  /*  Conta o número de celulas  vivas no final */
  isum = 0;
  for(i=1; i<=NI; i++){
    for(j=1; j<=NJ; j++){
      isum = isum + new[i][j];
    }
  }
  printf("\n# Celulas Vivas = %d\n", isum);


  for(i=0; i<ni; i++){
    free(old[i]); 
    free(new[i]); 
  }

  free(old);
  free(new);

  return 0;
}
