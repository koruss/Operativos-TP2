#include <stdio.h>
#include <sys/shm.h>
#include <string.h>
#include "bitacora.h"
#include "bitacora.c"
#include "proceso.h"

int main(int argc, char **argv)
{
	int *shm_size_buf;
	int shmid, shm2id, shm_size_id, size_t, buffer_size;

	printf("Ingrese la cantidad de páginas o segmentos a ser asignados: ");
	scanf("%d", &size_t);

	// El tamaño del buffer principal va a ser un arreglo de arreglos. Con la el
	// tamaño especificado por el usuario por 3 enteros (pid, cantidad de
	// páginas, número de página/segmento)
	buffer_size = sizeof(int) * size_t * 3;

	// Memoria principal
	// Esta contiene todos los procesos que se están ejecutando actualmente.
	shmid = shmget(SHM_KEY, buffer_size, 0777 | IPC_CREAT); // Permisos de 777 (rwxrwxrwx) para la memoria compartida.

	// Memoria secundaria
	// Esta memoria contiene los structs de los procesos durante el periodo de
	// ejecución del creador de procesos.
	shm2id = shmget(SHM2_KEY, sizeof(PROCESO)*SEC_MEM_SIZE, 0777 | IPC_CREAT);

	// Memoria terciaria
	// Esta contiene el tamaño de memoria ingresado por el usuario, para que los
	// otros programas puedan saber el tamaño especificado para la memoria
	// compartida.
	shm_size_id = shmget(BUFF_SIZE_KEY, sizeof(int), 0777 | IPC_CREAT);
	// Attach a la memoria compartida.
	shm_size_buf = (int *)shmat(shm_size_id, NULL, 0);
	// Copiar la cantidad ingresada por el usuario a la memoria compartida.
	memcpy(shm_size_buf, &size_t, sizeof(int));
	// Detach de la memoria compartida.
    shmdt(shm_size_buf);
	
	if (shmid == -1 || shm2id == -1 || shm_size_id == -1)
	{
		perror("Shared memory");
		return 1;
	}

	printf("Se han creado %d espacios de memoria compartida exitosamente.\n", size_t);

	if(createLog(size_t) > -1)
		printf("Se ha creado el archivo de bitácora exitosamente\n");

	printf("Cerrando el programa...\n\n");
	

	return 0;
}
