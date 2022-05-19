#ifndef PROCESO_H
#define PROCESO_H

#define SHM_KEY 0x1234
#define SHM2_KEY 0x1235
#define BUFF_SIZE_KEY 0x1236

typedef struct PROCESO{
    int pid;
    char type; // type of process P = pagination, S = Segmentation
    int time; // time
    int division; // division
    int subdivision; // subdivision
}PROCESO;

//PROCESO *create_process(int _pid, char _type, int _time, int _division, int _subdivision);


#endif