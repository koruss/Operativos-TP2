#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <pthread.h>
#include <sys/queue.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include "proceso.h"
#include "bitacora.h"

static int PID = 0;
static int nprocess = 0;
int *shm_size_buf;
int (*shm_primary)[3];
PROCESO *shm_secondary;
int shmid, shm2id, shm_size_id, usr_size, buffer_size, usr_choice;;
sem_t mutex;
FILE *file;
time_t t;

// Chequea si se puede guardar el proceso en la memoria y se cambia el estado.
int checkMainMemory(PROCESO *proceso){
	//int (*mem_ptr)[3];
    PROCESO (*mem_ptr)[usr_size] = shm_secondary;
	// Attach a la memoria compartida.
    // mem_ptr = shm_primary;
	// for(int i = 0; i < usr_size; i++){
    //     printf("pid: %d\n", mem_ptr[i][0]);
    //     printf("\t cantpag: %d\n", mem_ptr[i][0]);
    //     printf("\t numpag: %d\n", mem_ptr[i][0]);
	// }
    mem_ptr[1]->pid =0;
    mem_ptr[0]->pid = 69;
    print_proc(mem_ptr[0]);
    print_proc(mem_ptr[1]);

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
    // if (1 == checkMainMemory(myProcess))//si encontro espacio
    //     fprintf(file,"PID: %d, Accion: En Ejecucion, Tipo: Asignacion, hora: %d Segmentos: %d\n", myProcess->pid,t,myProcess->division);
    // else{// si no encontro espacio
    //     fprintf(file,"PID: %d, Accion: Muerto, Tipo: Asignacion, hora: %d Segmentos: %d\n", myProcess->pid,t,myProcess->division);
    // }
    // sem_post(&mutex);//devuelve el semaforo

    // sleep(myProcess->time);// se simula la ejecucion del proceso

    // sem_wait(&mutex);
    
    // if (1 == releaseMemory(myProcess))
    //     fprintf(file,"PID: %d, Accion: Finalizado, Tipo: Desasignacion, hora: %d Segmentos: %d\n", myProcess->pid,t,myProcess->division);
    // else{
    //     perror("Error releasing the process");
	// 	return NULL;
    // }

    // sem_post(&mutex);

    // pthread_exit(NULL);
    return NULL;
}

void* openSharedMemory(){
    // Attach a la memoria compartida que contiene el tamaño ingresado por el usuario.
    shm_size_id = shmget(BUFF_SIZE_KEY, sizeof(int), 0777 | IPC_CREAT);
    shm_size_buf = (int*) shmat(shm_size_id, NULL, 0);
    usr_size = *shm_size_buf;
	buffer_size = sizeof(int) * usr_size * 3;
    // Attach a la memoria que va a contener a todos los procesos.
    shmid = shmget(SHM_KEY, buffer_size, 0777 | IPC_CREAT);
    shm_primary = (int*) shmat(shmid, NULL, 0);
    // Attach a la memoria que va a servir como bitácora de procesos.
    shm2id = shmget(SHM2_KEY, sizeof(PROCESO)*SEC_MEM_SIZE, 0777 | IPC_CREAT);
    shm_secondary = (PROCESO*) shmat(shm2id, NULL, 0);

    if(DEBUG){
        printf("ID de la memoria compartida primaria: %d\n", shmid);
        printf("ID de la memoria compartida secundaria: %d\n", shm2id);
        printf("ID de la memoria compartida terciaria: %d\n", shm_size_id);
    }
}

void processCreator(int mode)
{
    while (PID<SEC_MEM_SIZE)
    {
        pthread_t thread;
        PROCESO *procesito;
        PROCESO (*tmp_ptr)[usr_size] = shm_secondary;
        int process_time = (rand() % 41) + 20; // El tiempo de vida del proceso.

        // Crea el proceso dependiendo del modo. (1 = Paginación y 2 = Segmentación)
        if (mode == 1){
            int paginas = (rand() % 10) + 1; // calcula entre(1- 10)/
            procesito = create_process(++PID, 0,process_time, paginas, 0);
        }
        else{
            int segments = (rand() % 5) + 1;
            procesito = create_process(++PID, 1,process_time, segments, 0);
        }

        // Poner al proceso nuevo en bitácora.
        appendLog(procesito);

        printf("Proceso creado con PID: %d\n",procesito->pid);

        // Agregar el proceso a la memoria compartida secundaria.
        tmp_ptr[PID]->pid = procesito->pid;
        tmp_ptr[PID]->size = procesito->size;
        tmp_ptr[PID]->state = procesito->state;
        tmp_ptr[PID]->time = procesito->time;
        tmp_ptr[PID]->type = procesito->type;

        pthread_create(thread,NULL, allocateProcess,(void *)procesito);// se crea el thread
        fflush(stdout);
        int sleepTime = (rand() % 31) + 30; // sleeptime between (30 - 60) seconds
        sleep(sleepTime); 
    }

}

int main(int argc, char **argv)
{
    //file = fopen("bitacora.log","w");

    printf("Ingrese el algoritmo con el que desear ejecutar las simulacion\n");
    printf("\t 1. Paginación\n");
    printf("\t 2. Segmentación\n");
    scanf("%d", &usr_choice);

    openSharedMemory();


    //processCreator(usr_choice);
    PROCESO *sexo = create_process(1, 1, 1, 3, 0);
    checkMainMemory(sexo);
    //fclose(file);


    //Detach a todas las memorias compartidas
    shmdt(shm_size_buf);
    shmdt(shm_primary);
    shmdt(shm_secondary);
}