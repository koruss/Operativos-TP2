#ifndef PROCESO_H
#define PROCESO_H

typedef struct PROCESO{
    int pid;
    char type; // type of process P = pagination, S = Segmentation
    int time; // time
    int division; // division
    int subdivision; // subdivision
}PROCESO;

//PROCESO *create_process(int _pid, char _type, int _time, int _division, int _subdivision);


#endif