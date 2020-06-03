#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#define DEBUG 1            // comentar esta linha quando for medir tempo
#define ARRAY_SIZE 10000      // trabalho final com o valores 10.000, 100.000, 1.000.000
#define WORKSET 1000

void bs(int n, int * vetor)
{
    int c=0, d, troca, trocou =1;

    while (c < (n-1) & trocou )
        {
        trocou = 0;
        for (d = 0 ; d < n - c - 1; d++)
            if (vetor[d] > vetor[d+1])
                {
                troca      = vetor[d];
                vetor[d]   = vetor[d+1];
                vetor[d+1] = troca;
                trocou = 1;
                }
        c++;
        }
}

int main(int argc , char **argv)
{
   // int vetor[WORKSET];
    int i,j;
    clock_t time;



     int *work[WORKSET];
     for(i = 0;i < WORKSET;i++)
        work[i] = (int*)malloc(ARRAY_SIZE * sizeof(int));
        
    //int (*work)[WORKSET] = malloc (ARRAY_SIZE * sizeof(*work));


    for (i=0 ; i<WORKSET; i++)              /* init array with worst case for sorting */
        for(j=0;j<ARRAY_SIZE;j++)
            work[i][j] = ARRAY_SIZE-j;

   

    #ifdef DEBUG
    printf("\nVetor: ");
    for (i=0 ; i<WORKSET; i++)    
    {          /* print unsorted array */
        for(j=0;j<ARRAY_SIZE;j++)
            printf("[%03d] ", work[i][j]);
        printf("\n");
    }
    #endif

    time = clock();

for(i = 0;i < WORKSET;i++)
    bs(ARRAY_SIZE, work[i]);                     /* sort array */

    #ifdef DEBUG
    printf("\nVetor: ");
    for (i=0 ; i<WORKSET; i++)    
    {          /* print sorted array */
        for(j=0;j<ARRAY_SIZE;j++)
            printf("[%03d] ", work[i][j]);
        printf("\n");
    }
    #endif



    printf("\nTempo de execucao: %f\n\n",(clock()-time)/(double)CLOCKS_PER_SEC);

    for(i = 0;i < WORKSET;i++)
        free(work[i]);

    return 0;
}