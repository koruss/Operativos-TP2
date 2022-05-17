#ifndef PROCESO_H
#define PROCESO_H

#define SHM_KEY 0x1234

typedef struct _proceso{
    int pid;
    char type; // type of process P = pagination, S = Segmentation
    int time; // time
    int division; // division
    int subdivision; // subdivision
}PROCESO;

#endif