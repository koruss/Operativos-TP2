#include <stdio.h>
#include <sys/shm.h>
#include <string.h>
#include "proceso.h"

int main (int argc, char **argv)
{

/*-------------The crit Region memory----------------------*/
        PROCESO* mcompartida1;
        int cantidad;
        printf("Ingrese la cantida de memoria\t");
        scanf("%d", &cantidad);

        int mem1 = 0;
        int buffer_size = sizeof(PROCESO)*cantidad;

        // Permisos de 777 para la memoria compartida.  
        mem1 = shmget(SHM_KEY,buffer_size,0777 | IPC_CREAT);
        mcompartida1 = (PROCESO *) shmat(mem1,NULL,0);
        // Poner todo el buffer de la memoria compartida en 0.
        memset(mcompartida1, 0, buffer_size);

        printf (" %d espacios de memoria compartida creados exitosamente\n", cantidad); 

/*----------------------------------------------------------------*/

        // The memory that will hold the process in the different states 
        PROCESO* mcompartida2;
        int mem2 = 0;
        mem2 = shmget(SHM2_KEY,sizeof(PROCESO)*cantidad*1000,0777 | IPC_CREAT);
        mcompartida2 = (PROCESO *) shmat(mem2,NULL,0);
    

/*---------------------------------------------------------------*/
        // The shared memory that will hold the size of the memory requested by the user.
        int *shmp_buf;

        int buf_size_mem = 0;
        buf_size_mem = shmget(BUFF_SIZE_KEY, sizeof(int), 0777 | IPC_CREAT);
        shmp_buf = (int *)shmat(buf_size_mem,NULL,0);

        memcpy(shmp_buf, &cantidad, sizeof(int));
        //shmp_buf = cant
        printf("La memoria en inicializador : %d \n", *shmp_buf);
        //shmp_buf = sprintf(cantidad, sizeof(int), "%d");

        return 0;


}
