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
        sent_to = malloc(proc_n * sizeof(int)); //array de controle de ordem do saco de trabalho

        int (*work)[WORKSET] = malloc (ARRAY_SIZE * sizeof(*work));
            


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
        t1 = MPI_Wtime(); //inicio de medicao
        while(sorted == 0) //enquanto nao estiver tudo pronto
        {
            #ifdef DEBUG_DEADLOCK
                    printf("A");
                #endif
            MPI_Recv(message, ARRAY_SIZE, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status); //espera mensagens dos escravos
            
           
            
            
            if(status.MPI_TAG == READY_TAG) //se os escravos estao prontos para iniciar a execucao
            {
                int i;
                sent_to[status.MPI_SOURCE] = to_sort; //guarda o indice do vetor a ser mandado na posicao do escravo a ser mandado
                
                for(i = 0; i < ARRAY_SIZE;i++)
                {
                    message[i] = work[to_sort][i]; // copia o vetor a ser ordenado para a mensagem
                }
                to_sort++; //incrementa o contador do saco
                
                MPI_Send(message, ARRAY_SIZE, MPI_INT, status.MPI_SOURCE, WORK_TAG, MPI_COMM_WORLD); //envia pro escravo que pediu

                #ifdef DEBUG_VERBOSE
                printf("mandei vetor %d para proc %d\n",to_sort-1, status.MPI_SOURCE);
                #endif

            }
            else if(status.MPI_TAG == STOPPING_TAG) //se os escravos responderam
            {
                #ifdef DEBUG_VERBOSE
                printf("proc %d morto\n", status.MPI_SOURCE);
                #endif
                done_processess++; // significa que eles terminaram de executar
            } 
            else if(status.MPI_TAG == DONE_TAG) // quando o escravo termina o trabalho e esta pronto para outro
            {            
                    #ifdef DEBUG_MESSAGE  
                        for(j=0;j<ARRAY_SIZE;j++)
                            printf("MESSAGE [%03d] ", message[j]);
                        printf("\n");
                    #endif
                   
                    int found = sent_to[status.MPI_SOURCE]; // pesquisa qual foi o vetor recebido agora

                    #ifdef DEBUG_VERBOSE
                    printf("recebi vetor %d de proc %d\n",found, status.MPI_SOURCE);
                    #endif

                   
                    for(i = 0; i < ARRAY_SIZE;i++)
                        {
                            work[found][i] = message[i]; //copia o vetor ordenado na mensagem para o saco de trabalho
                        }
                    
                if(to_sort < WORKSET) // se ainda nao terminamos nosso trabalho
                {
                    sent_to[status.MPI_SOURCE] = to_sort; //guarda o indice do vetor a ser mandado na posicao do escravo a ser mandado
                    
                    for(i = 0; i < ARRAY_SIZE;i++)
                        {
                            message[i] = work[to_sort][i]; // copia o vetor a ser ordenado para a mensagem
                        }
                    
                    to_sort++; //incrementa o contador do saco
                    #ifdef DEBUG_VERBOSE
                    printf("mandei vetor %d para proc %d\n",to_sort-1, status.MPI_SOURCE);
                    #endif                                     
                    MPI_Send(message, ARRAY_SIZE, MPI_INT, status.MPI_SOURCE, WORK_TAG, MPI_COMM_WORLD); //envia pro escravo que pediu
                }
                if(to_sort > WORKSET-1) //se ja enviamos tudo, mas os escravos estao pedindo mais
                {
                    #ifdef DEBUG_VERBOSE
                    printf("matarei proc %d\n", status.MPI_SOURCE);
                    #endif
                    MPI_Send(message, 1, MPI_INT, status.MPI_SOURCE, STOP_TAG, MPI_COMM_WORLD); //manda o escravo morrer pois nao tem mais trabalho
             
                }                              
            }
            
             if(done_processess >= proc_n-1) //se todos os escravos estao mortos
                {
                    #ifdef DEBUG_DEADLOCK
                    printf("FODEU");
                    #endif
                    break; //terminamos nosso trabalho
                }
            
        
        }
        
        
        
        t2 = MPI_Wtime(); //terminamos a medicao

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
        MPI_Send(message, 1, MPI_INT, 0, READY_TAG, MPI_COMM_WORLD); //inicia a execucao o escravo estra pronto pra trabalhar
        while(1)
        {
            MPI_Recv(message, ARRAY_SIZE, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status); 
            if(status.MPI_TAG == WORK_TAG) // se for uma mensagem de trabalho
            {
                #ifdef DEBUG2   
                    for(j=0;j<ARRAY_SIZE;j++)
                        printf("[%03d] ", message[j]);
                #endif
                bs(ARRAY_SIZE,message); // ordena o vetor

                #ifdef DEBUG2 
                    for(j=0;j<ARRAY_SIZE;j++)
                        printf("[%03d] ", message[j]);
                #endif
                MPI_Send(message, ARRAY_SIZE, MPI_INT, 0, DONE_TAG, MPI_COMM_WORLD); //manda de volta
                 
            }
            if(status.MPI_TAG == STOP_TAG) // se o mestre pede para eu parar
            {
                MPI_Send(message, 1, MPI_INT, 0, STOPPING_TAG, MPI_COMM_WORLD); //respondo que entendi
                break;// e paro
            }
        }
    }
    


MPI_Finalize();
    



    

    return 0;
}