#ifndef _UTILITY_H_
#define _UTILITY_H_

int    	consistenciaarchivo(int workers);
void   	datosmalos(char *string);
int    	inicializa_archivos(int argc,char *argv[], int rank);
void 	cierra_archivos(void);
void    genera_resultados_algoritmo(int corrida, int tipo_problema, char *nombrearchivo, cpu_consummed_time_t *t);  

#endif

