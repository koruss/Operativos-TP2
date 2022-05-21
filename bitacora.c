#include "bitacora.h"

int createLog(int _mem_size){
	FILE *fptr;
	time_t current_time;
	struct tm time_s;
	size_t tmp;
	char time_string[21];
	char current_line[200] = "Bitacora para el segundo proyecto programado.\n\n";
	char mem_size[50];

    if ((fptr = fopen(LOG_PATH,"w")) == NULL)
	{
		printf("Hubo un error al crear la bitacora\n");
		return -1;
	}
	
	// Preparar el string de tiempo actual.
	current_time = time(NULL);
	time_s = *(localtime(&current_time));
	tmp = strftime(time_string, 30, "%b %d, %Y; %H:%M:%S", &time_s);

	// Pasar el entero de cantidad de memoria a un string.
	sprintf(mem_size, "%d", _mem_size);

	// Ingresar los datos al buffer del archivo.
	fputs(current_line, fptr);
	strncpy(current_line, time_string, sizeof(current_line));
	strcat(current_line, ": Se ha creado un espacio en memoria compartida con ");
	strcat(current_line, mem_size);
	strcat(current_line, " espacios en memoria.\n");
	fputs(current_line, fptr);
	
	// Cerrar el archivo de bitácora.
	fclose(fptr);
	
	return 0;
}

void appendLog(){

}