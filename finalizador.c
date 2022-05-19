#include <stdio.h>
#include <sys/shm.h>
#include "proceso.h"

int main(int argc, char **argv)
{
	int shmid;
	shmid = shmget(SHM_KEY, sizeof(PROCESO) * 100, 0777 | IPC_CREAT);
	if (shmid == -1)
	{
		perror("Shared memory");
		return 1;
	}
	printf("Proceso finalizador: Identificador de la memoria compartida %d\n", shmid);
	printf("Proceso finalizador: Liberando la memoria compartida...\n", shmid);

	if (-1 == (shmctl(shmid, IPC_RMID, NULL)))
	{
		perror("shmctl-IPC_RMID");
	}
	printf("Proceso finalizador: Cerrando...\n", shmid);
	return 0;
}
