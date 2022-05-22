#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <pthread.h>
#include <sys/queue.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include "proceso.h"
#include "bitacora.h"

static int PID = 0;
static int num_proc = 0;
int *shm_tertiary;
int (*shm_primary)[3];
PROCESO *shm_secondary;
int shmid, shm2id, shm3id, usr_size, buffer_size, usr_choice;

// Chequea la memoria principal, y busca espacios disponibles. Utilizando
// paginación. Si hay campo para colocar el proceso, retorna el índice inicial
// para insertar. Si no hay campo, retorna -1.
int checkMemPage(PROCESO *proceso)
{
    int first_idx = 0;
    int consec_cells = 0;
    int(*mem_ptr)[3];
    // Conseguir el puntero de la memoria principal en el primero espacio.
    mem_ptr = shm_primary;
    for (int i = 0; i < usr_size; i++)
    {
        // Si la celda actual está vacía.
        if (mem_ptr[i][0] == -1)
        {
            consec_cells++;
            if (consec_cells == proceso->size)
                return first_idx;
        }
        // Cambiar el índice actual y reiniciar el contador de celdas
        // consecutivas.
        else
        {
            first_idx = i + 1;
            consec_cells = 0;
        }
    }
    return -1;
}

// Chequea la memoria principal, y busca espacios disponibles. Utilizando
// segmentación. Si hay campo, la función retorna un puntero a un arreglo con
// las posiciones disponibles para inserción. Si no hay campo, retorna -1.
void checkMemSegment(int *indices, PROCESO *proceso)
{
    int(*mem_ptr)[3];
    int seg_counter = 0;
    // Conseguir el puntero de la memoria principal en el primero espacio.
    mem_ptr = shm_primary;

    for (int i = 0; i < usr_size; i++)
    {
        // Conseguir la dirección del segmento si hay campo disponible.
        if (seg_counter == proceso->size){
            if(DEBUG){
                printf("Indices seleccionados para los segmentos de PID %d\n", proceso->pid);
                for(int j = 0; j<proceso->size;j++){
                    printf(" Indice: %d\n", indices[j]);
                }
            }
            // Retornar si sí hay campo.
            return;
        }

        if (mem_ptr[i][0] == -1){
            indices[seg_counter] = i;
            seg_counter++;
        }
    }
    // Retornar si no hay campo.
    indices[0] = -1;
    return;
}

// Inserta un proceso a la memoria principal, utilizando paginación.
void insertMemPage(int idx, PROCESO *proceso)
{
    int(*mem_ptr)[3];
    int page_counter = 1;
    // Conseguir el puntero de la memoria principal en el primero espacio.
    mem_ptr = shm_primary;
    for (int i = idx; i < (idx + proceso->size); i++)
    {
        mem_ptr[i][0] = proceso->pid;
        mem_ptr[i][1] = proceso->size;
        mem_ptr[i][2] = page_counter;
        page_counter++;
    }
    if (DEBUG)
    {
        printf("IDX recibido: %d\n", idx);
        print_proc(proceso);
    }
}

// Inserta un proceso a la memoria principal, utilizando segmentación.
void insertMemSegment(int *indices, PROCESO *proceso)
{
    int(*mem_ptr)[3];
    // Conseguir el puntero de la memoria principal en el primero espacio.
    mem_ptr = shm_primary;
    for(int i = 0; i<proceso->size; i++){
        mem_ptr[indices[i]][0] = proceso->pid;
        mem_ptr[indices[i]][1] = proceso->size;
        mem_ptr[indices[i]][2] = i+1;
    }
}

void reset_arr( int *array, int size) {
    for(int i = 0; i<size; i++)
        array[i] = 0;
}

int insertProcess(PROCESO *proceso)
{
    // Insertar utilizando paginación.
    if (usr_choice == 1)
    {
        int array_idx = checkMemPage(proceso);
        // No se encontró espacio disponible para el proceso.
        if (array_idx == -1)
        {
            // Cambiar el estado del proceso a "blocked".
            proceso->state = 4;
            return -1;
        }
        insertMemPage(array_idx, proceso);
        // Cambiar el estado del proceso a "running".
        proceso->state = 2;
        return 0;
    }
    // Insertar utilizando segmentación.
    else
    {
        int arr_size = proceso->size;
        int indices[arr_size];
        //int *indices;
        //indices = checkMemSegment2(proceso);

        reset_arr(indices,arr_size);
        //memset(indices, 0, arr_size*sizeof(int));

        checkMemSegment(indices, proceso);
        if (indices[0] == -1)
        {
            // Cambiar el estado del proceso a "blocked".
            proceso->state = 4;
            return -1;
        }
        insertMemSegment(indices, proceso);
        // Cambiar el estado del proceso a "running".
        proceso->state = 2;
        return 0;
    }
}

void releaseMemory(PROCESO *proceso)
{
    int(*mem_ptr)[3];
    // Conseguir el puntero de la memoria principal en el primero espacio.
    mem_ptr = shm_primary;

    if (DEBUG)
    {
        printf("Liberando memoria para el proceso: %d\n", proceso->pid);
    }
    // Optimización posible: Preguntar a PhD: Kenneth Corrales.
    for (int i = 0; i < usr_size; i++)
    {
        // if(DEBUG){
        //     printf("PID en memoria: %d\n", mem_ptr[i][0]);
        //     printf("PID del struct: %d\n", proceso->pid);
        // }
        if (mem_ptr[i][0] == proceso->pid)
        {
            mem_ptr[i][0] = -1;
            mem_ptr[i][1] = -1;
            mem_ptr[i][2] = -1;
        }
    }
}

void *allocateProcess(void *process)
{
    struct PROCESO *current_proc = (struct PROCESO *)process;
    sem_t *semaphore = sem_open(SEM_NAME, 0, 0644, 0);
    int inserted = -1;

    printf("Asignación del proceso: %d \n", current_proc->pid);
    // Bloqueo de la región crítica por el semáforo.
    sem_wait(semaphore);
    // Cambiar el estado del proceso a "searching".
    current_proc->state = 1;
    appendLog(current_proc);
    inserted = insertProcess(current_proc);
    appendLog(current_proc);
    // Desbloqueo de la región crítica por el semáforo.
    sem_post(semaphore);

    // Ejecutar el proceso.
    if (inserted == 0)
    {
        sleep(current_proc->time);

        // Bloqueo de la región crítica por el semáforo.
        sem_wait(semaphore);
        // Cambiar el estado del proceso a "terminated".
        releaseMemory(current_proc);
        current_proc->state = 3;
        appendLog(current_proc);
        // Desbloqueo de la región crítica por el semáforo.
        sem_post(semaphore);
    }

    pthread_exit(NULL);
    return NULL;
}

void *openSharedMemory(int mode)
{
    // Attach a la memoria compartida que contiene el tamaño ingresado por el usuario.
    int _shm3id = shmget(BUFF_SIZE_KEY, sizeof(int)*TERC_MEM_SIZE, 0777);

    shm_tertiary = (int *)shmat(_shm3id, NULL, 0);
    usr_size = shm_tertiary [0];
    num_proc = usr_size;
    // Guardar el modo de operación. (Paginación o segmentación)
    shm_tertiary[1] = mode;
    //guarda el pid del proceso en la memoria compartida terciaria 
    shm_tertiary[4] = getpid();


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

void processCreator(int mode)
{
    while (PID < SEC_MEM_SIZE)
    {
        pthread_t thread;
        PROCESO *procesito;
        PROCESO *tmp_ptr = shm_secondary;
        int process_time = (rand() % 41) + 20; // El tiempo de vida del proceso.

        // Crea el proceso dependiendo del modo. (1 = Paginación y 2 = Segmentación)
        if (mode == 1)
        {
            int paginas = (rand() % 10) + 1; // calcula entre(1- 10)/
            procesito = create_process(++PID, 0, process_time, paginas, 0);
        }
        else
        {
            int segments = (rand() % 5) + 1;
            procesito = create_process(++PID, 1, process_time, segments, 0);
        }

        // Poner al proceso nuevo en bitácora.
        appendLog(procesito);

        printf("Proceso creado con PID: %d\n", procesito->pid);

        // Agregar el proceso a la memoria compartida secundaria.
        tmp_ptr[PID].pid = procesito->pid;
        tmp_ptr[PID].size = procesito->size;
        tmp_ptr[PID].state = procesito->state;
        tmp_ptr[PID].time = procesito->time;
        tmp_ptr[PID].type = procesito->type;

        pthread_create(&thread, NULL, allocateProcess, (void *)procesito); // se crea el thread
        fflush(stdout);
        int sleepTime = (rand() % 31) + 30; // sleeptime between (30 - 60) seconds
        sleep(sleepTime);
    }
}

int main(int argc, char **argv)
{
    printf("Ingrese el algoritmo con el que desear ejecutar las simulacion\n");
    printf("\t 1. Paginación\n");
    printf("\t 2. Segmentación\n");
    scanf("%d", &usr_choice);

    openSharedMemory(usr_choice);

    processCreator(usr_choice);

    // Detach a todas las memorias compartidas
    shmdt(shm_tertiary);
    shmdt(shm_primary);
    shmdt(shm_secondary);
}