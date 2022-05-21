#include "proceso.h"

/*
1= ready
2= searching
3= running(sleep)
4= terminated
5= dead(blocked)
*/

PROCESO *create_process(int _pid, char _type, int _time, int _division, int _subdivision,int _state){
    PROCESO *proceso = (PROCESO *)malloc(sizeof(PROCESO));
    proceso->pid = _pid;
    proceso->type = _type;
    proceso->time = _time;
    proceso->division = _division;
    proceso->subdivision = _subdivision;
    proceso->state = _state;
    return proceso;
}

void toString(PROCESO *proceso){
    printf("PID: %d\n Tipo: %s\n Tiempo: %d\n",proceso->pid, proceso->type, proceso->time);
}