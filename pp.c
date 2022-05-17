#include <stdio.h>
#include <sys/shm.h>
#include <pthread.h>
#include <sys/queue.h>
#include "proceso.h"

static int PID = 0;
typedef struct _Node
{
    LIST_ENTRY(_Node)
    pointers;
    pthread_t thread;
    PROCESO *proceso;
} _Node;

_Node *createNode(pthread_t thread,PROCESO process)
{
    _Node *nodito = (_Node *)malloc(sizeof(_Node));
    nodito->thread = thread;
    return nodito;
}



void processCreator(int type)
{
    while (1)
    {
        
        pthread_t thread;
        PROCESO *procesito;
        int time = (rand() % 41) + 20; //

        if (type == 1){
            int paginas = (rand() % 10) + 1; // calculate (1- 10)/
            procesito = create_process(++PID, 'P', time, paginas , NULL);
        }
        else{
            int segments = (rand() % 5) + 1;
            int subsegments = (rand() %3) + 1;
            procesito = create_process(++PID,'S',time, segments, subsegments);

        }
        pthread_create(&node->thread,NULL, enviarAlReady,0);// como mandar el proceso por el thread 

    }

}

int main(int argc, char **argv)
{
    int opcion;
    
    printf("Ingrese el algoritmo con el que desear ejecutar las simulacion\n");
    printf("\t 1. Paginacion");
    printf("\t 2. Segmentacion");
    scanf("%d", &opcion);
    processCreator(opcion);


}