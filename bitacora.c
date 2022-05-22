#include "bitacora.h"
FILE *fptr;


void getCurrentTime(char* time_string){
	time_t current_time;
	struct tm time_s;
	size_t tmp;

	current_time = time(NULL);
	time_s = *(localtime(&current_time));
	tmp = strftime(time_string, 30, "%b %d, %Y; %H:%M:%S", &time_s);
}

int createLog(int _mem_size){
	//time_t current_time;
	//struct tm time_s;
	//size_t tmp;
	char time_string[21];
	char current_line[200] = "Bitacora para el segundo proyecto programado.\n\n";
	char mem_size[50];

    if ((fptr = fopen(LOG_PATH,"w")) == NULL)
	{
		printf("Hubo un error al crear la bitacora\n");
		return -1;
	}

	
	// Preparar el string de tiempo actual.
	//current_time = time(NULL);
	//time_s = *(localtime(&current_time));
	//tmp = strftime(time_string, 30, "%b %d, %Y; %H:%M:%S", &time_s);

	// Pasar el entero de cantidad de memoria a un string.
	sprintf(mem_size, "%d", _mem_size);

	// Ingresar los datos al buffer del archivo.
	fputs(current_line, fptr);
	getCurrentTime(time_string);
	strncpy(current_line, time_string, sizeof(current_line));
	strcat(current_line, ": Se ha creado un espacio en memoria compartida con ");
	strcat(current_line, mem_size);
	strcat(current_line, " espacios en memoria.\n");
	fputs(current_line, fptr);
	
	if ( fclose(fptr) != 0){
	perror("Error cerrando el archivo");
	return -1; 
	}
	
	return 0;
}


/*
1= ready
2= searching
3= running(sleep)
4= terminated
5= dead(blocked)
*/

char actions[5][20] = { "READY ", "SEARCHING ", "RUNNING ", "TERMINATED ", "DEAD "};
char type[2][20] = {"Paginas: " ,"Segmentos: "};

void appendLog(PROCESO *proceso){
	FILE *fptr;
	char current_line[200];
	char pid[50];
	char espacio[50];
	char time_string[21];

	fptr = fopen(LOG_PATH, "a");
	if(fptr ==NULL){
		perror("Error abriendo el archivo para añadir a bitácora.");
	}
	getCurrentTime(time_string);
	strncpy(current_line,  time_string, sizeof(current_line));
	strcat(current_line, ": PID : ");
	sprintf(pid, "%d", proceso->pid);
	strcat(current_line,pid);
	strcat(current_line , " Accion: " );
	strcat(current_line, actions[proceso->state] );
	sprintf(espacio, "%d", proceso->size);
	strcat(current_line, type[proceso->type]);
	strcat(current_line,espacio);
	strcat(current_line,"\n");
	fputs(current_line, fptr);
	fclose(fptr);
}