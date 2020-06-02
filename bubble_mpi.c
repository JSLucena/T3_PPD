#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

//#define DEBUG 1            // comentar esta linha quando for medir tempo
//#define DEBUG2 1
//#define DEBUG3 1
//#define DEBUG_VERBOSE 1
//#define DEBUG_DEADLOCK 1
//#define DEBUG_MESSAGE 1

#define ARRAY_SIZE 10000   // trabalho final com o valores 10.000, 100.000, 1.000.000
#define WORKSET 1000

#define STOP_TAG 0
#define READY_TAG 1
#define WORK_TAG 2
#define DONE_TAG 3
#define STOPPING_TAG 4

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

int search_source(int n, int sent[], int source)
{
    int i = 0;
    for(i=0;i<n;i++)
    {
        #ifdef DEBUG2
            printf("[%d]",sent[i]);
        #endif
        if(sent[i] == source)
            {
                sent[i] = -1;
                return i;
            }

        
    }
     #ifdef DEBUG2
            printf("\n");
    #endif
    return -1;
}

int validate_sort(int n, int sent[])
{
    int i = 0;
    for(i = 0;i < n; i++)
    {
        
        if(sent[i] != -1)
            return 0;
    }
   
    return 1;
}

int main(int argc , char **argv)
{
    int i,j;
    double t1,t2;

    int proc_n;
    int my_rank;
    int dest;
    MPI_Status status;

    int *sent_to;
    int sorted = 0;
    int stop = 0;
    int to_sort = 0;
    int *message;
    int done_processess = 0;

    

    MPI_Init (&argc , & argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); // pega pega o numero do processo atual (rank)
    MPI_Comm_size(MPI_COMM_WORLD, &proc_n);  // pega informacao do numero de processos (quantidade total)

    
    message = malloc(ARRAY_SIZE * sizeof(int));

    if(my_rank == 0)
    {
        sent_to = malloc(proc_n * sizeof(int));

        int (*work)[WORKSET] = malloc (ARRAY_SIZE * sizeof(*work));
        /*
        for(i = 0;i < WORKSET;i++)
            work[i] = (int*)malloc(ARRAY_SIZE * sizeof(int));
            */
            


        for (i=0 ; i<WORKSET; i++)              /* init array with worst case for sorting */
            for(j=0;j<ARRAY_SIZE;j++)
                work[i][j] = ARRAY_SIZE-j+i;

    #ifdef DEBUG  
    for (i=0 ; i<WORKSET; i++)    
    {          /* print unsorted array */
        printf("\nVetor (%d): ", i);
        for(j=0;j<ARRAY_SIZE;j++)
            printf("[%03d] ", work[i][j]);
        
    }
    printf("\n");
    #endif
        t1 = MPI_Wtime();
        while(sorted == 0)
        {
            #ifdef DEBUG_DEADLOCK
                    printf("A");
                #endif
            MPI_Recv(message, ARRAY_SIZE, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status); 
            
           
            
            
            if(status.MPI_TAG == READY_TAG)
            {

                //sent_to[to_sort] = status.MPI_SOURCE;
                sent_to[status.MPI_SOURCE] = to_sort;
                
                for(int i = 0; i < ARRAY_SIZE;i++)
                {
                    message[i] = work[to_sort][i];
                }
                
                //message = work[to_sort];
                to_sort++;
                
                MPI_Send(message, ARRAY_SIZE, MPI_INT, status.MPI_SOURCE, WORK_TAG, MPI_COMM_WORLD);

                #ifdef DEBUG_VERBOSE
                printf("mandei vetor %d para proc %d\n",to_sort-1, status.MPI_SOURCE);
                #endif

            }
            else if(status.MPI_TAG == STOPPING_TAG)
            {
                #ifdef DEBUG_VERBOSE
                printf("proc %d morto\n", status.MPI_SOURCE);
                #endif
                done_processess++;
            } 
            else if(status.MPI_TAG == DONE_TAG)
            {            
                    #ifdef DEBUG_MESSAGE  
                        for(j=0;j<ARRAY_SIZE;j++)
                            printf("MESSAGE [%03d] ", message[j]);
                        printf("\n");
                    #endif
                   
                    int found = sent_to[status.MPI_SOURCE];

                    #ifdef DEBUG_VERBOSE
                    printf("recebi vetor %d de proc %d\n",found, status.MPI_SOURCE);
                    #endif

                   
                    for(int i = 0; i < ARRAY_SIZE;i++)
                        {
                            work[found][i] = message[i];
                        }
                    
                   // work[found][0] = *message;
                if(to_sort < WORKSET)
                {
                    //sent_to[to_sort] = status.MPI_SOURCE;
                    sent_to[status.MPI_SOURCE] = to_sort;
                    
                    for(int i = 0; i < ARRAY_SIZE;i++)
                        {
                            message[i] = work[to_sort][i];
                        }
                    
                    //message = work[to_sort];
                    to_sort++;
                    #ifdef DEBUG_VERBOSE
                    printf("mandei vetor %d para proc %d\n",to_sort-1, status.MPI_SOURCE);
                    #endif                                     
                    MPI_Send(message, ARRAY_SIZE, MPI_INT, status.MPI_SOURCE, WORK_TAG, MPI_COMM_WORLD);
                }
                if(to_sort > WORKSET-1)
                {
                    #ifdef DEBUG_VERBOSE
                    printf("matarei proc %d\n", status.MPI_SOURCE);
                    #endif
                    MPI_Send(message, 1, MPI_INT, status.MPI_SOURCE, STOP_TAG, MPI_COMM_WORLD);
             
                }                              
            }
            
             if(done_processess >= proc_n-1)
                {
                    #ifdef DEBUG_DEADLOCK
                    printf("FODEU");
                    #endif
                    break;
                }
            
        
        }
        
        
        
        t2 = MPI_Wtime();

         #ifdef DEBUG
         for (i=0 ; i<WORKSET; i++)    
        {         
            printf("\nVetor (%d): ", i);
            for(j=0;j<ARRAY_SIZE;j++)
                printf("[%03d] ", work[i][j]);
        
        }
        #endif
        printf("\nTempo de execucao: %f\n\n",t2-t1);
    }
    else
    {
        MPI_Send(message, 1, MPI_INT, 0, READY_TAG, MPI_COMM_WORLD);
        while(1)
        {
            MPI_Recv(message, ARRAY_SIZE, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);  
            if(status.MPI_TAG == WORK_TAG)
            {
                #ifdef DEBUG2   
                    for(j=0;j<ARRAY_SIZE;j++)
                        printf("[%03d] ", message[j]);
                #endif
                bs(ARRAY_SIZE,message);

                #ifdef DEBUG2 
                    for(j=0;j<ARRAY_SIZE;j++)
                        printf("[%03d] ", message[j]);
                #endif
                MPI_Send(message, ARRAY_SIZE, MPI_INT, 0, DONE_TAG, MPI_COMM_WORLD);
                 
            }
            if(status.MPI_TAG == STOP_TAG)
            {
                MPI_Send(message, 1, MPI_INT, 0, STOPPING_TAG, MPI_COMM_WORLD);
                break;
            }
        }
    }
    


MPI_Finalize();
    



    

    return 0;
}