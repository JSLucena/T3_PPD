#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#define DEBUG 1            // comentar esta linha quando for medir tempo
#define ARRAY_SIZE 200000      // trabalho final com o valores 10.000, 100.000, 1.000.000
#define WORKSET 1000

int compare(const void * a, const void * b)
{
    return(*(int*)a - *(int*)b);
}

int main(int argc , char **argv)
{
    int i,j;
    clock_t time;



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

    time = clock(); // inicio de medicao

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



    printf("\nTempo de execucao: %f\n\n",(clock()-time)/(double)CLOCKS_PER_SEC); //fim de medicao

    for(i = 0;i < WORKSET;i++)
        free(work[i]); //libera memoria

    return 0;
}