#include "proceso.h"

/*
1= ready
2= searching
2= running(sleep)
3= terminated
4= dead
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