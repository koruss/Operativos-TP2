#include <stdio.h>
#include <sys/shm.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include "bitacora.h"
#include "proceso.h"

// Inicializa la memoria principal con todos los valores en -1.
void init_main_mem(int _shmid, int _usr_size)
{
	int(*mem_ptr)[3];
	// Attach a la memoria compartida.
	mem_ptr = (int(*)[3])shmat(_shmid, NULL, 0);
	for (int i = 0; i < _usr_size; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			mem_ptr[i][j] = -1;
		}
	}

	if (DEBUG)
	{
		printf("Contenido de la memoria compartida primaria:\n");
		for (int i = 0; i < _usr_size ; i++)
		{
			printf(" %d, %d, %d\n", mem_ptr[i][0], mem_ptr[i][1], mem_ptr[i][2]);
		}
	}

	// Detach de la memoria compartida.
	shmdt(mem_ptr);
}

void init_sec_mem(int _shm2id){
	PROCESO *mem_ptr;
	// Attach a la memoria compartida.
	mem_ptr = (PROCESO *)shmat(_shm2id, NULL, 0);
	// Asignar toda la memoria compartida a -1
	for (int i = 0; i < SEC_MEM_SIZE; i++)
	{
		mem_ptr[i].pid = -1;
		mem_ptr[i].size = -1;
		mem_ptr[i].state = -1;
		mem_ptr[i].time = -1;
		mem_ptr[i].type = -1;
	}

	if (DEBUG)
	{
		printf("Contenido de la memoria compartida secundaria:\n");
		for (int i = 0; i < SEC_MEM_SIZE; i++)
		{
			printf(" %d, ", mem_ptr[i].pid);
			printf(" %d, ", mem_ptr[i].size);
			printf(" %d, ", mem_ptr[i].state);
			printf(" %d, ", mem_ptr[i].time);
			printf(" %d\n", mem_ptr[i].type);
		}
	}

	// Detach de la memoria compartida.
	shmdt(mem_ptr);
}

void init_terc_mem(int _shm3id, int _usr_size)
{
	int *mem_ptr;

	// Attach a la memoria compartida.
	mem_ptr = (int *)shmat(_shm3id, NULL, 0);
	// Asignar toda la memoria compartida a -1
	for (int i = 0; i < TERC_MEM_SIZE; i++)
	{
		mem_ptr[i] = 0;
	}

	mem_ptr[0] = _usr_size;
	mem_ptr[2] = getpid();

	if (DEBUG)
	{
		printf("Contenido de la memoria compartida terciaria:\n");
		for (int i = 0; i < TERC_MEM_SIZE; i++)
		{
			printf(" %d\n", mem_ptr[i]);
		}
	}

	// Detach de la memoria compartida.
	shmdt(mem_ptr);
}

int main(int argc, char **argv)
{
	sem_t *sem;
	int *shm_size_buf;
	int shmid, shm2id, shm3id, usr_size, buffer_size;

	printf("Ingrese la cantidad de p??ginas o segmentos a ser asignados: ");
	scanf("%d", &usr_size);

	// El tama??o del buffer principal va a ser un arreglo de arreglos. Con el
	// tama??o especificado por el usuario por 3 enteros (pid, cantidad de
	// p??ginas/segmentos, n??mero de p??gina/segmento)
	buffer_size = sizeof(int) * usr_size * 3;

	// Memoria principal
	// Esta contiene todos los procesos que se est??n ejecutando actualmente.
	shmid = shmget(SHM_KEY, buffer_size, 0777 | IPC_CREAT); // Permisos de 777 (rwxrwxrwx) para la memoria compartida.
	init_main_mem(shmid, usr_size);

	// Memoria secundaria
	// Esta memoria contiene los structs de los procesos durante el periodo de
	// ejecuci??n del creador de procesos.
	shm2id = shmget(SHM2_KEY, sizeof(PROCESO) * SEC_MEM_SIZE, 0777 | IPC_CREAT);
	init_sec_mem(shm2id);

	// Memoria terciaria
	// Esta contiene el tama??o de memoria ingresado por el usuario, para que los
	// otros programas puedan saber el tama??o especificado para la memoria
	// compartida.
	/*
	 * 0: Tama??o en memoria, especificado por el usuario
	 * 1: Modo de operaci??n, con valores de 1 para paginaci??n y 2 para segmentaci??n.
	 * 2: PID del inicializador.
	 * 3: PID del esp??a.
	 * 4: PID del pp.
	 */
	shm3id = shmget(BUFF_SIZE_KEY, sizeof(int) * TERC_MEM_SIZE, 0777 | IPC_CREAT);
	init_terc_mem(shm3id, usr_size);

	if (DEBUG)
	{
		printf("Shared memory IDs: %d, %d, %d\n", shmid, shm2id, shm3id);
	}

	if (shmid == -1 || shm2id == -1 || shm3id == -1)
	{
		perror("Shared memory");
		return 1;
	}

	printf("Se han creado %d espacios de memoria compartida exitosamente.\n", usr_size);

	// Abrir el sem??foro.
	if ((sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0644, 1)) < 0)
	{
		printf("Error al crear el semaforo %s.\n", SEM_NAME);
		return -1;
	}

	printf("Se ha creado el sem??foro %s exitosamente.\n", SEM_NAME);

	if (createLog(usr_size) > -1)
		printf("Se ha creado el archivo de bit??cora exitosamente\n");

	printf("Cerrando el programa...\n\n");

	return 0;
}
