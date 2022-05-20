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


static int PID = 0;
static int nprocess = 0;
int *size_buf;
int shmid, shm2id, shm_size_id, cantidad, crit_reg_size;
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

// Chequea si se puede guardar el proceso en la memoria y se cambia el estado.
int checkMainMemory(PROCESO *proceso){
    PROCESO *proc_ptr = crit_region;
    
    char *conv = calloc(1, sizeof(unsigned) * 2 + 3); // each byte is 2 characters in hex, and 2 characters for the 0x and 1 character for the trailing NUL

    for(int i=0; i<cantidad; i++){
        //sprintf(conv, "0x%X", (unsigned) sizeof(PROCESO)*i);
        sprintf(conv, "%x", sizeof(PROCESO)*i);
        long num = strtol(conv, NULL, 16);
        proc_ptr = crit_region + num;
        printf("%x\n", conv);
        printf("%d, %d, %p",i ,cantidad, proc_ptr);
        toString(proc_ptr);
        //if(proc_ptr->pid==0){
        //    proceso->state=2;
        //    return 1;
        //}
    }
    return 0;
}

int releaseMemory(PROCESO *proceso){
    return 1;
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
        fprintf(file,"PID: %d, Accion: En Ejecucion, Tipo: Asignacion, hora: %d Segmentos: %d\n", myProcess->pid,t,myProcess->division);
    else{// si no encontro espacio
        fprintf(file,"PID: %d, Accion: Muerto, Tipo: Asignacion, hora: %d Segmentos: %d\n", myProcess->pid,t,myProcess->division);
    }
    sem_post(&mutex);//devuelve el semaforo

    sleep(myProcess->time);// se simula la ejecucion del proceso

    sem_wait(&mutex);
    
    if (1 == releaseMemory(myProcess))
        fprintf(file,"PID: %d, Accion: Finalizado, Tipo: Desasignacion, hora: %d Segmentos: %d\n", myProcess->pid,t,myProcess->division);
    else{
        perror("Error releasing the process");
		return NULL;
    }

    sem_post(&mutex);

    pthread_exit(NULL);
    return NULL;
}

void* openSharedMemory(){
    // Attach a la memoria compartida que contiene el tamaño ingresado por el usuario.
    shm_size_id = shmget(BUFF_SIZE_KEY, sizeof(int), 0777 | IPC_CREAT);
    size_buf = (int*) shmat(shm_size_id, NULL, 0);
    cantidad = *size_buf;
    crit_reg_size = sizeof(PROCESO)*cantidad;
    // Attach a la memoria que va a contener a todos los procesos.
    shmid = shmget(SHM_KEY, crit_reg_size, 0777 | IPC_CREAT);
    crit_region = (PROCESO*) shmat(shmid, NULL, 0);
    // Attach a la memoria que va a servir como bitácora de procesos.
    shm2id = shmget(SHM2_KEY, crit_reg_size*1000, 0777 | IPC_CREAT);
    bitacora = (PROCESO*) shmat(shm2id, NULL, 0);

    if(DEBUG){
        printf("ID de la memoria de tamaño: %d\n", shm_size_id);
        printf("ID de la memoria de región crítica: %d\n", shmid);
        printf("ID de la memoria de bitacora: %d\n", shm2id);
    }
    
}

void processCreator(int type)
{
    while (1)
    {
        pthread_t thread;
        PROCESO *procesito;
        int processTime = (rand() % 41) + 20; //
        //crea el proceso
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
        if(opcion == 1) fprintf(file,"PID: %d, Accion: ready, Tipo: Asignacion, hora: %d Paginas: %d\n" , procesito->pid,t,procesito->division);
        else{
            fprintf(file,"PID: %d, Accion: ready, Tipo: Asignacion, hora: %d Segmentos: %d\n", procesito->pid,t,procesito->division);
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
    printf("\t 1. Paginación\n");
    printf("\t 2. Segmentación\n");
    scanf("%d", &opcion);

    openSharedMemory();


    //processCreator(opcion);
    PROCESO *sexo = create_process(1, 'p', 5, 3, 0, 1);
    checkMainMemory(sexo);
    fclose(file);


    //Detach a todas las memorias compartidas
    shmdt(size_buf);
    shmdt(crit_region);
    shmdt(bitacora);
}