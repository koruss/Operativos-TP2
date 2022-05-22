#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <signal.h>
#include "proceso.h"

int main(int argc, char **argv)
{
	int *shm_tertiary;
	int shmid, shm2id, shm3id, usr_size, buffer_size;

	// Conseguir el id de la memoria compartida.
    shm3id = shmget(BUFF_SIZE_KEY, sizeof(int)*TERC_MEM_SIZE, 0777);
    // Attach a la memoria compartida que contiene la cantidad ingresada por el usuario.
    shm_tertiary = (int*) shmat(shm3id, NULL, 0);
    usr_size = shm_tertiary[0];
	buffer_size = sizeof(int) * usr_size * 3;

	// Conseguir el id de la memoria compartida.
	shmid = shmget(SHM_KEY, buffer_size , 0777);
    shm2id = shmget(SHM2_KEY, sizeof(PROCESO)*SEC_MEM_SIZE, 0777);

	if(DEBUG){
		printf("Shared memory IDs: %d, %d, %d\n", shmid, shm2id, shm3id);
		printf("Contenido de la memoria compartida terciaria:\n");
		for (int i = 0; i < TERC_MEM_SIZE; i++)
		{
			printf(" %d\n", shm_tertiary[i]);
		}
	}

	// Validar la conexiones a las memorias compartidas.
	if (shmid == -1 || shm2id == -1 || shm3id == -1)
	{
		perror("Shared memory");
		return 1;
	}

	// Finalizar los programas abiertos.
	if (shm_tertiary[2] < 1 || kill(shm_tertiary[2], SIGTERM) != 0){
		printf("No se ha encontrado un inicializador para finalizar.\n");
	}else{
		printf("Programa inicializador finalizado.\n");
	}

	if (shm_tertiary[3] < 1 || kill(shm_tertiary[3], SIGTERM) != 0){
		printf("No se ha encontrado un espía para finalizar.\n");
	}else{
		printf("Programa espía finalizado.\n");
	}

	if (shm_tertiary[4] < 1 || kill(shm_tertiary[4], SIGTERM) != 0){
		printf("No se ha encontrado un generador de procesos para finalizar.\n");
	}else{
		printf("Programa generador de procesos finalizado.\n");
	}

	// Detach de la memoria compartida.
    shmdt(shm_tertiary);

	// Cerrar las memorias compartidas.
	if (-1 == (shmctl(shmid, IPC_RMID, NULL))){
		perror("shmctl-IPC_RMID");
		return 1;
	}
	if (-1 == (shmctl(shm2id, IPC_RMID, NULL))){
		perror("shmctl-IPC_RMID");
		return 1;
	}
	if (-1 == (shmctl(shm3id, IPC_RMID, NULL))){
		perror("shmctl-IPC_RMID");
		return 1;
	}

	printf("La memoria compartida ha sido liberada exitosamente.\n");

	// Cerrar el semáforo
	if(sem_unlink(SEM_NAME) > -1){
		printf("El semáforo ha sido cerrado.\n");
	}

	printf("Cerrando el programa...\n\n");
	return 0;
}
