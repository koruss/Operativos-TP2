#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <pthread.h>
#include <sys/queue.h>
#include <semaphore.h>
#include <unistd.h>
#include "proceso.h"
#include "proceso.c"

static int PID = 0;
sem_t mutex;
PROCESO *critRegion;
PROCESO *Bitacora;
int *memorySize;

typedef struct _Node
{
    LIST_ENTRY(_Node)
    pointers;
    pthread_t thread;
    PROCESO *proceso;
} _Node;

_Node *createNode(pthread_t thread,PROCESO *process)
{
    _Node *nodito = (_Node *)malloc(sizeof(_Node));
    nodito->thread = thread;
    nodito->proceso = process;
    return nodito;
}

void* allocateProcess(void *process){
    struct PROCESO *myProcess = (struct PROCESO*)process;
    printf("Allocating process %d \n",myProcess->pid);
    sem_wait(&mutex);

    return NULL;
}
void* openSharedMemory(){
    int shmid;
    int *size_buf;
    shmid = shmget(BUFF_SIZE_KEY, sizeof(int), 0777 | IPC_CREAT);
    printf("ID de la memoria %d\n\n", shmid);
    size_buf = (int*) shmat(shmid, NULL, 0);
    printf("Tamaño: %d", *size_buf);
    shmdt(size_buf);
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
            procesito = create_process(++PID,'P',time,paginas ,0,1);
        }
        else{
            int segments = (rand() % 5) + 1;
            int subsegments = (rand() %3) + 1;
            procesito = create_process(++PID,'S',time,segments, subsegments,1);

        }
        printf("Proceso creado con PID: %d\n",procesito->pid);
        _Node *node = createNode(thread, procesito);
        // se guarda el proceso en la memoria compartida que tiene todas las weas

        pthread_create(&node->thread,NULL, allocateProcess,(void *)node->proceso);// se crea el thread

        int sleepTime = (rand() % 31) + 30; // sleeptime between (30 - 60) seconds
        sleep(sleepTime); 

    }

}

int main(int argc, char **argv)
{
    int opcion;
    printf("Ingrese el algoritmo con el que desear ejecutar las simulacion\n");
    printf("\t 1. Paginacion");
    printf("\t 2. Segmentacion");
    scanf("%d", &opcion);

    openSharedMemory();


    processCreator(opcion);


}