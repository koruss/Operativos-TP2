#include "proceso.h"

PROCESO *create_process(int _pid, char _type, int _time, int _division, int _subdivision){
    PROCESO *proceso = (PROCESO *)malloc(sizeof(PROCESO));
    proceso->pid = _pid;
    proceso->type = _type;
    proceso->time = _time;
    proceso->division = _division;
    proceso->subdivision = _subdivision;
    return proceso;


}