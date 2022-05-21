#ifndef BITACORA_H
#define BITACORA_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include "proceso.h"

#define LOG_PATH "./bitacora.log"

struct _IO_FILE;
typedef struct _IO_FILE FILE;

int createLog(int _mem_size);
void appendLog(PROCESO *proceso);

#endif