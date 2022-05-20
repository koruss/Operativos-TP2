#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <unistd.h>
#include "proceso.h"
#include "proceso.c"
#include "espia.h"

#define BUF_SIZE 1024
#define SHM_KEY 0x1234

static int KEY_D = 1;

pthread_t tid0;

int *size_buf;
int shmid, shm2id, shm_size_id;
PROCESO *crit_region;


struct shmseg {
   int cnt;
   int complete;
   char buf[BUF_SIZE];
};


void currently_memory(){

}

void seeking_space(){
    
}

void process_blocked(){

}

void process_dead(){

}

void process_finished(){
    
}

void print_memory_state(){
    printf( "Memory information\n");
    
}

void print_process_state(){
    printf( "Process information \n");
    printf( "\nProcesses currently in memory \n");
    currently_memory();
    printf( "\nSingle process that is seeking memory space \n");
    seeking_space();
    printf( "\nProcesses blocked \n");
    process_blocked();
    printf( "\nProcesses that have died due to not having enough space \n");
    process_dead();
    printf( "\nProcesses that have finished their execution \n");
    process_finished();

}

void print_help(){
    printf("\n****Help menu****\n");
    printf("\tPress m to show memory state.\n");
    printf("\tPress p to show process state.\n");
}


void *start_keyboard_daemon(){
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
    }
    pthread_exit(0);
    return 0;
}

int main (int argc, char **argv)
{ 
    const char* arg1 = argv[1];       
    // Create the thread for the Keyboard daemon.
    // pthread_create(&tid0, NULL, start_keyboard_daemon(), NULL);

    // int mem_size;
    // // Attach a la memoria compartida que contiene el tamaño ingresado por el usuario.
    // shm_size_id = shmget(BUFF_SIZE_KEY, sizeof(int), 0777 | IPC_CREAT);
    // size_buf = (int*) shmat(shm_size_id, NULL, 0);
    // mem_size = *size_buf;
    // // Attach a la memoria que va a contener a todos los procesos.
    // shmid = shmget(SHM_KEY, sizeof(PROCESO)*mem_size, 0777 | IPC_CREAT);
    // crit_region = (PROCESO*) shmat(shmid, NULL, 0);


    // printf("ID de la memoria de tamaño: %d\n", shm_size_id);
    // printf("Tamaño de la memoria de tamaño: %d\n", *size_buf);
    // printf("ID de la memoria de región crítica: %d\n", shmid);
    // printf("Tamaño de la memoria región crítica: %d\n", *crit_region);


    int shmid;
    struct shmseg *shmp;
    shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0777|IPC_CREAT);
    shmp = shmat(shmid, NULL, 0);


    while (shmp->complete != 1) {
        printf("segment contains : \n\"%s\"\n", shmp->buf);
        printf("Reading Process: Shared Memory: Read %d bytes\n", shmp->cnt);
        sleep(3);
    
    }
    return 0;       
}