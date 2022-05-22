#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "proceso.h"
#include "espia.h"

#define BUF_SIZE 1024
#define SHM_KEY 0x1234

static int KEY_D = 1;
static int num_proc = 0;

pthread_t tid0;

int *shm_tertiary;
int (*shm_primary)[3];
PROCESO *shm_secondary;
int shmid, shm2id, shm3id, usr_size, buffer_size, usr_choice;


struct shmseg {
   int cnt;
   int complete;
   char buf[BUF_SIZE];
};

//(pid, cantidad de
// páginas/segmentos, número de página/segmento)
void print_memory_state(){
    printf( "*****Contenido en la memoria*****\n");
    int(*mem_ptr)[3];
    char pid[5];
    char proc_size[5];
    char seg_num[5];
    char mode[9];

    if(shm_tertiary[1] < 1 || shm_tertiary[1] == 1){
	    strncpy(mode, "página", sizeof(mode));
    }
    else{
	    strncpy(mode, "segmento", sizeof(mode));
    }

    // Conseguir el puntero de la memoria principal en el primero espacio.
    mem_ptr = shm_primary;
    for (int i = 0; i < usr_size ; i++){
        printf("┃PID: %d",mem_ptr[i][0]);
        printf(" ┃ # de celdas en memoria: %d",mem_ptr[i][1]);
        printf(" ┃ # de %s: %d┃\n", mode, mem_ptr[i][2]);
    }  
}

void print_list(int *lista, int idx){
    fflush(stdout);
    for(int i = 0; i < idx ; i++){
        printf("\tPID: %d \n" ,lista[i]);
    }
}

void print_sec_mem(){
    PROCESO *tmp_ptr = shm_secondary;

    printf("********LISTA DE MEMORIA SECUNDARIA********\n");
    for(int i = 1; i<SEC_MEM_SIZE; i++){
        print_proc(&tmp_ptr[i]);
    }
}

void print_process_state(){
    PROCESO *procesito;
    PROCESO *tmp_ptr = shm_secondary;
    // listas temporales para imprimir los PID en orden de acuerdo al estado
    int running_process[1000];
    int running_idx= 0;
    int dead_process[1000];
    int dead_idx= 0;
    int searching_process[10];
    int searching_idx= 0;
    int ready_process[1000];
    int ready_idx= 0;
    int terminated_process[1000];
    int terminated_idx= 0;
    int process_state;
    int process_pid;
    // se parsea la memoria secundaria y se anade los pid a las listas
    // dependiendo del estado del proceso
    for (int i =0; i < SEC_MEM_SIZE; i++ ){
        process_state = tmp_ptr[i].state;
        process_pid = tmp_ptr[i].pid;
        if(DEBUG){
            printf("pid:%d|estado:%d\n", process_pid, process_state);
        }
        switch (process_state)
        {
        case 0:// ready -> esperando por region critica
            ready_process[ready_idx] = process_pid;
            ready_idx++;
            break;
        case 1:// searching ->buscando espacio en memoria
            searching_process[searching_idx] = process_pid;
            searching_idx++;
            break;
        case 2:
            running_process[running_idx] = process_pid;
            running_idx++;
            break; 
        case 3:
            terminated_process[terminated_idx] = process_pid;
            terminated_idx++;
            break; 
        case 4:
            dead_process[dead_idx] = process_pid;
            dead_idx++;
            break;
        default:
            break;
        }
    }
    printf( "**********Informacion de los procesos**********");
    printf( "\nPID de los procesos actualmente en ejecución\n");
    print_list(running_process,(running_idx));
    printf( "\nPID del proceso buscando espacio \n");
    print_list(searching_process,(searching_idx));
    printf( "\nPID de los procesos bloqueados (en ready)\n");
    print_list(ready_process,(ready_idx));
    printf( "\nPID de los procesos muertos(no encontraron espacio)\n");
    print_list(dead_process,(dead_idx));
    printf( "\nPID de los procesos que ya terminaron su ejecucion \n");
    print_list(terminated_process,(terminated_idx));
}

void print_help(){
    printf("\n****Menu de Ayuda****\n");
    printf("\tPresione m para ver el estado de la memoria.\n");
    printf("\tPresione p para ver el estado de los procesos.\n");
    printf("\tPresione q para salir del programa.\n");
}


void start_keyboard_daemon(){
    int user_input;
    usleep(400);
    printf("Starting keyboard daemon\n");
    while(KEY_D){
        printf( "Enter a command (h for help):");
        user_input = getchar();
        if(user_input == 'h')
            print_help();
        if(user_input == 'm')
            print_memory_state();
        if(user_input == 'p')
            print_process_state();
        if(user_input == 'a')
            print_sec_mem();
        if(user_input == 'q')
            exit(0);
    }
    pthread_exit(0);
    
}

void *openSharedMemory()
{
    // Attach a la memoria compartida que contiene el tamaño ingresado por el usuario.
    shm3id = shmget(BUFF_SIZE_KEY, sizeof(int)*TERC_MEM_SIZE, 0777);
    shm_tertiary = (int *)shmat(shm3id, NULL, 0);
    //usr_size = *shm_tertiary;
    //num_proc = usr_size;

    usr_size = shm_tertiary[0];
    num_proc = usr_size;

    shm_tertiary[3] = getpid();



    buffer_size = sizeof(int) * usr_size * 3;
    // Attach a la memoria que va a contener a todos los procesos.
    shmid = shmget(SHM_KEY, buffer_size, 0777);
    shm_primary = (int(*)[3])shmat(shmid, NULL, 0);
    // Attach a la memoria que va a servir como bitácora de procesos.
    shm2id = shmget(SHM2_KEY, sizeof(PROCESO) * SEC_MEM_SIZE, 0777);
    shm_secondary = (PROCESO *)shmat(shm2id, NULL, 0);
    if (DEBUG)
    {
        printf("ID de la memoria compartida primaria: %d\n", shmid);
        printf("ID de la memoria compartida secundaria: %d\n", shm2id);
        printf("ID de la memoria compartida terciaria: %d\n", shm3id);
    }
}

int main (int argc, char **argv)
{ 

    openSharedMemory();
    start_keyboard_daemon();

    shmdt(shm_primary);
    shmdt(shm_secondary);
    shmdt(shm_tertiary);
}
