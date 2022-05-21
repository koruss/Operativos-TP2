#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include "proceso.h"

int main(int argc, char **argv)
{
	int shmid, shm2id, shm_size_id, cantidad, crit_reg_size;
	int *size_buf;

    // Attach a la memoria compartida que contiene el tamaño ingresado por el usuario.
    shm_size_id = shmget(BUFF_SIZE_KEY, sizeof(int), 0777 | IPC_CREAT);
    size_buf = (int*) shmat(shm_size_id, NULL, 0);
    cantidad = *size_buf;
    crit_reg_size = sizeof(PROCESO)*cantidad;
    shmdt(size_buf);

	shmid = shmget(SHM_KEY, crit_reg_size , 0777 | IPC_CREAT);
    shm2id = shmget(SHM2_KEY, crit_reg_size*1000 , 0777 | IPC_CREAT);

	if(DEBUG){
		printf("%d, %d, %d", shmid, shm2id, shm_size_id);
	}

	if (shmid == -1 || shm2id == -1 || shm_size_id == -1)
	{
		perror("Shared memory");
		return 1;
	}
    
	printf("Proceso finalizador: Identificador de la memoria compartida %d\n", shmid);
	printf("Proceso finalizador: Liberando la memoria compartida...\n", shmid);

	if (-1 == (shmctl(shmid, IPC_RMID, NULL)))
		perror("shmctl-IPC_RMID");
	if (-1 == (shmctl(shm2id, IPC_RMID, NULL)))
		perror("shmctl-IPC_RMID");
	if (-1 == (shmctl(shm_size_id, IPC_RMID, NULL)))
		perror("shmctl-IPC_RMID");

	printf("Proceso finalizador: Cerrando...\n", shmid);
	return 0;
}
