#include "proceso.h"

PROCESO *create_process(int _pid, int _type, int _time, int _size, int _state){
    PROCESO *proceso = (PROCESO *)malloc(sizeof(PROCESO));
    proceso->pid = _pid;
    proceso->type = _type;
    proceso->time = _time;
    proceso->size = _size;
    proceso->state = _state;
    return proceso;
}

void print_proc(PROCESO *proceso){
    printf("PID: %d\n Tipo: %d\n Tiempo: %d\n",proceso->pid, proceso->type, proceso->time);
    printf(" Tamaño: %d\n", proceso->size);
    printf(" Estado: %d\n", proceso->state);
}