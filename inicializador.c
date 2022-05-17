#include <stdio.h>
#include <sys/shm.h>
#include "proceso.h"

#define SHM_KEY 0x1234

int main (int argc, char **argv)
{
        int cantidad;
        printf("Ingrese la cantida de memoria\t");
        scanf("%d", &cantidad);

        FILE *archivocompartido; //puntero del archivo
        //key_t clavecompartida; // la llave que se usa para acceder al archivo

        int mem = 0;
        char *mcompartida = NULL;

        archivocompartido = fopen("/tmp/memFisica","w+"); //abre el archivo
        //clavecompartida = ftok("/tmp/acompartido",33); //

        mem = shmget(SHM_KEY,sizeof(PROCESO)*cantidad,0777 | IPC_CREAT);// 0777 se refiere a los permisos
        mcompartida = (char *) shmat(mem,NULL,0);

        printf (" %d bytes de memoria compartida creados exitosamente\n", cantidad);
        return 0;       
}
