#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

//#define DEBUG 1            // comentar esta linha quando for medir tempo
#define ARRAY_SIZE 200000      // trabalho final com o valores 10.000, 100.000, 1.000.000
#define WORKSET 1000

int compare(const void * a, const void * b)
{
    return(*(int*)a - *(int*)b);
}

int main(int argc , char **argv)
{
   // int vetor[WORKSET];
    int i,j;
    double t1,t2;



     int *work[WORKSET];
     for(i = 0;i < WORKSET;i++)
        work[i] = (int*)malloc(ARRAY_SIZE * sizeof(int));


    for (i=0 ; i<WORKSET; i++)              /* init array with worst case for sorting */
        for(j=0;j<ARRAY_SIZE;j++)
            work[i][j] = ARRAY_SIZE-j;

   

    #ifdef DEBUG
    
    for (i=0 ; i<WORKSET; i++)  
    {          /* print unsorted array */
        printf("\nVetor (%d): ", i);
        for(j=0;j<ARRAY_SIZE;j++)
            printf("[%03d] ", work[i][j]);
        printf("\n");
    }
    #endif

    t1 = MPI_Wtime();

for(i = 0;i < WORKSET;i++)
    qsort(work[i],ARRAY_SIZE,sizeof(int),compare);                     /* sort array */

    #ifdef DEBUG
    for (i=0 ; i<WORKSET; i++)    
    {          /* print sorted array */
    printf("\nVetor (%d): ", i);
        for(j=0;j<ARRAY_SIZE;j++)
            printf("[%03d] ", work[i][j]);
        printf("\n");
    }
    #endif



    t2 = MPI_Wtime();
    printf("\nTempo de execucao: %f\n\n",t2-t1);

    for(i = 0;i < WORKSET;i++)
        free(work[i]);

    return 0;
}