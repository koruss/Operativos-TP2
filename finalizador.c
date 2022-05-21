#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include "proceso.h"

int main(int argc, char **argv)
{
	int *shm_size_buf;
	int shmid, shm2id, shm_size_id, size_t, buffer_size;

	// Conseguir el id de la memoria compartida.
    shm_size_id = shmget(BUFF_SIZE_KEY, sizeof(int), 0777 | IPC_CREAT);
    // Attach a la memoria compartida que contiene la cantidad ingresada por el usuario.
    shm_size_buf = (int*) shmat(shm_size_id, NULL, 0);
    size_t = *shm_size_buf;
	buffer_size = sizeof(int) * size_t * 3;
	// Detach de la memoria compartida.
    shmdt(shm_size_buf);

	// Conseguir el id de la memoria compartida.
	shmid = shmget(SHM_KEY, buffer_size , 0777 | IPC_CREAT);
    shm2id = shmget(SHM2_KEY, sizeof(PROCESO)*SEC_MEM_SIZE, 0777 | IPC_CREAT);

	if(DEBUG){
		printf("Shared memory IDs: %d, %d, %d\n", shmid, shm2id, shm_size_id);
	}

	if (shmid == -1 || shm2id == -1 || shm_size_id == -1)
	{
		perror("Shared memory");
		return 1;
	}

	printf("La memoria compartida ha sido liberada exitosamente.\n");

	if (-1 == (shmctl(shmid, IPC_RMID, NULL)))
		perror("shmctl-IPC_RMID");
	if (-1 == (shmctl(shm2id, IPC_RMID, NULL)))
		perror("shmctl-IPC_RMID");
	if (-1 == (shmctl(shm_size_id, IPC_RMID, NULL)))
		perror("shmctl-IPC_RMID");

	printf("Cerrando el programa...\n\n");
	return 0;
}
