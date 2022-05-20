#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <pthread.h>
#include <sys/queue.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include "proceso.h"
#include "proceso.c"

#define DEBUG 1

static int PID = 0;
static int nprocess = 0;
int *size_buf;
int shmid, shm2id, shm_size_id;
sem_t mutex;
PROCESO *crit_region;
PROCESO *bitacora;
FILE *file;
int opcion;
time_t t;

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

//chequea si se puede guardar el proceso en la memoria y se cambia el estado
int checkMainMemory(PROCESO *proceso){
    
}

void* allocateProcess(void *process){
    struct PROCESO *myProcess = (struct PROCESO*)process;
    printf("Allocating process %d \n",myProcess->pid);
    sem_wait(&mutex);
    //se cambia el estado del proceso
    myProcess->state=2;
    //busca en la memoria compartida a ver si puede guardarse
    t= time(NULL);
    if (1 == checkMainMemory(myProcess))//si encontro espacio
        fprintf(file,"PID: %d, Accion: running, Tipo: Asignacion, hora: %d Segmentos: %d", myProcess->pid,t,myProcess->division);
    else{
        fprintf(file,"PID: %d, Accion: running, Tipo: Asignacion, hora: %d Segmentos: %d", myProcess->pid,t,myProcess->division);
    }
    



    

    return NULL;
}



void* openSharedMemory(){
    int mem_size;
    // Attach a la memoria compartida que contiene el tamaño ingresado por el usuario.
    shm_size_id = shmget(BUFF_SIZE_KEY, sizeof(int), 0777 | IPC_CREAT);
    size_buf = (int*) shmat(shm_size_id, NULL, NULL);
    mem_size = *size_buf;
    // Attach a la memoria que va a contener a todos los procesos.
    shmid = shmget(SHM_KEY, sizeof(PROCESO)*mem_size, 0777 | IPC_CREAT);
    crit_region = (PROCESO*) shmat(shmid, NULL, NULL);
    // Attach a la memoria que va a servir como bitácora de procesos.
    shm2id = shmget(SHM_KEY, sizeof(PROCESO)*mem_size*1000, 0777 | IPC_CREAT);
    bitacora = (PROCESO*) shmat(shm2id, NULL, NULL);

    
    if(DEBUG){
        printf("ID de la memoria de tamaño: %d\n", shm_size_id);
        printf("Tamaño de la memoria de tamaño: %d\n", *size_buf);
        printf("ID de la memoria de región crítica: %d\n", shm_size_id);
        printf("Tamaño de la memoria región crítica: %d\n", *size_buf);
        printf("ID de la memoria de bitacora: %d\n", shm_size_id);
        printf("Tamaño de la memoria de bitacora: %d\n", *size_buf);
    }
    
}




void processCreator(int type)
{
    while (1)
    {
        pthread_t thread;
        PROCESO *procesito;
        int processTime = (rand() % 41) + 20; //

        if (type == 1){
            int paginas = (rand() % 10) + 1; // calcula entre(1- 10)/
            procesito = create_process(++PID,'P',processTime,paginas ,0,1);
        }
        else{
            int segments = (rand() % 5) + 1;
            int subsegments = (rand() %3) + 1;
            procesito = create_process(++PID,'S',processTime,segments, subsegments,1);
        }
        
        printf("Proceso creado con PID: %d\n",procesito->pid);
        _Node *node = createNode(thread, procesito);
        
        // se guarda el proceso en la memoria compartida que tiene todas las weas
        //memcpy(bitacora, node->proceso, sizeof(PROCESO *));//agregar el proceso a la memoria de la bitacora
        
        t= time(NULL);
        //se guarda el proceso en el log
        if(opcion == 1) fprintf(file,"PID: %d, Accion: ready, Tipo: Asignacion, hora: %d Paginas: %d" , procesito->pid,t,procesito->division);
        else{
            fprintf(file,"PID: %d, Accion: ready, Tipo: Asignacion, hora: %d Segmentos: %d", procesito->pid,t,procesito->division);
        }


        
        
        pthread_create(&node->thread,NULL, allocateProcess,(void *)node->proceso);// se crea el thread

        int sleepTime = (rand() % 31) + 30; // sleeptime between (30 - 60) seconds
        sleep(sleepTime); 

    }

}

int main(int argc, char **argv)
{
    file = fopen("bitacora.log","w");

    printf("Ingrese el algoritmo con el que desear ejecutar las simulacion\n");
    printf("\t 1. Paginacion");
    printf("\t 2. Segmentacion\n");
    scanf("%d", &opcion);

    openSharedMemory();


    processCreator(opcion);


    //Detach a todas las memorias compartidas
    shmdt(size_buf);
    shmdt(crit_region);
    shmdt(bitacora);
}