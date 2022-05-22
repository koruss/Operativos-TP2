#ifndef PROCESO_H
#define PROCESO_H

#include <stdio.h>
#include <stdlib.h>

#define DEBUG 0
#define SHM_KEY 0x1234
#define SHM2_KEY 0x1235
#define BUFF_SIZE_KEY 0x1236
#define SEC_MEM_SIZE 1000
#define TERC_MEM_SIZE 5
#define SEM_NAME "sem_tp2"

/* Estados posibles para un proceso:
* 0 = ready
* 1 = searching
* 2 = running(sleep)
* 3 = terminated
* 4 = dead(blocked)
*/

typedef struct PROCESO{
    int pid;    // ID del proceso
    int type;   // 0 = Paginación, 1 = Segmentación
    int time;   // Tiempo de ejecución del proceso
    int state;  // Estado del proceso
    int size;   // Cantidad de páginas o segmentos del proceso
}PROCESO;

PROCESO *create_process(int _pid, int _type, int _time, int _size, int _state);

void print_proc(PROCESO *process);

#endif