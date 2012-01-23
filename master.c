/*----------------------------------------------------------------------------*/
/* master.h - rutinas relacionadas con el coordinador		                  */
/*----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>

#include "sga.h"
#include "random.h"
#include "utility.h"
#include "memory.h"
 
void almacena_ind_recibidos_en_pop_global(struct individual *recpop, int coord_ind_a_recibir, int nodo)
//Almacena la población recibida (recpop) desde nodox (nodo) en población global (glopop)
//La ubicación de la información del nodo es almacenada según el valor del nodo 
//La fórmula de almacenamiento es : (workers * (nodo - 1)) + j , con j:0...coord_ind_a_recibir-1
{
   	int i, j;
   
   	for(j = 0; j < coord_ind_a_recibir; j++) {
      	for(i = 0; i < chromsize; i++) // Actualiza Cromosoma String
         	glopop[((coord_ind_a_recibir * (nodo - 1)) + j)].chrom[i] = recpop[j].chrom[i];
      	for(i = 0; i < chmutsize; i++)// Actualiza Cromosoma Mutación
         	glopop[((coord_ind_a_recibir * (nodo - 1)) + j)].chmut[i] = recpop[j].chmut[i];
      	for(i = 0; i < chlistasize; i++)// Actualiza Cromosoma Lista
         	glopop[((coord_ind_a_recibir * (nodo - 1)) + j)].pusListaPiezas[i] = recpop[j].pusListaPiezas[i];
      	glopop[((coord_ind_a_recibir * (nodo - 1)) + j)].fitness      = recpop[j].fitness;
      	glopop[((coord_ind_a_recibir * (nodo - 1)) + j)].PEval        = recpop[j].PEval;
      	glopop[((coord_ind_a_recibir * (nodo - 1)) + j)].xsite        = recpop[j].xsite;
      	glopop[((coord_ind_a_recibir * (nodo - 1)) + j)].msite        = recpop[j].msite;
      	glopop[((coord_ind_a_recibir * (nodo - 1)) + j)].parent[0]    = recpop[j].parent[0];
      	glopop[((coord_ind_a_recibir * (nodo - 1)) + j)].parent[1]    = recpop[j].parent[1];
   	}//End for
}//End almacena_ind_recibidos_en_pop_global

struct individual *selecciona_individuos_a_enviar(struct individual *glopop, 
                   int coord_ind_globales, int coord_ind_a_enviar)
//De la población de individuos global selecciona los mejores o al azar
//envpop corresponde a la población de individuos seleccionada listos para enviar a los AGs
{
   	int i, j;
   	int FitnessComparaMaster(struct individual *Indi, struct individual *Indj);

#define SELECCIONA_MEJORES_INDIVIDUOS	0
#define SELECCIONA_INDIVIDUOS_AL_AZAR  	1
#define TIPO_SELECCION_A_DISTRIBUIR    	SELECCIONA_MEJORES_INDIVIDUOS

#if (TIPO_SELECCION_A_DISTRIBUIR == SELECCIONA_MEJORES_INDIVIDUOS)
   	//1do. Ordena mediante un qsort la población glopop
   	//2ro. Obtiene de la glopop los mejores ind_a_seleccionar = Z individuos = coord_ind_a_enviar
   
   	//1do. Ordena mediante un qsort la glopop
   	//Deja primero los individuos con menor fitness en forma ascendente con respecto al fitness
   	qsort(glopop, coord_ind_globales, sizeof(struct individual), (void *) FitnessComparaMaster);

   	//2ro. Obtiene de la glopop los mejores Z individuos (coord_ind_a_enviar)
   	for(j = 0; j < coord_ind_a_enviar; j++) {
      	for(i = 0; i < chromsize; i++) // Actualiza Cromosoma String
         	envpop[j].chrom[i] = glopop[j].chrom[i];
      	for(i = 0; i < chmutsize; i++)// Actualiza Cromosoma Mutación
         	envpop[j].chmut[i] = glopop[j].chmut[i];
      	for(i = 0; i < chlistasize; i++)// Actualiza Cromosoma Lista
         	envpop[j].pusListaPiezas[i] = glopop[j].pusListaPiezas[i];
      	envpop[j].fitness      = glopop[j].fitness;
      	envpop[j].PEval        = glopop[j].PEval;
      	envpop[j].xsite        = glopop[j].xsite;
      	envpop[j].msite        = glopop[j].msite;
      	envpop[j].parent[0]    = glopop[j].parent[0];
      	envpop[j].parent[1]    = glopop[j].parent[1];
   	}//End for
#elif (TIPO_SELECCION_A_DISTRIBUIR == SELECCIONA_INDIVIDUOS_AL_AZAR)
   	int k;
   
   	//Obtiene individuos a enviar al azar
   	for(j = 0; j < coord_ind_a_enviar; j++) {
      	k = rnd(0, coord_ind_globales - 1);
      	for(i = 0; i < chromsize; i++) // Actualiza Cromosoma String
         	envpop[j].chrom[i] = glopop[k].chrom[i];
      	for(i = 0; i < chmutsize; i++)// Actualiza Cromosoma Mutación
        	envpop[j].chmut[i] = glopop[k].chmut[i];
      	for(i = 0; i < chlistasize; i++)// Actualiza Cromosoma Lista
         	envpop[j].pusListaPiezas[i] = glopop[j].pusListaPiezas[i];
      	envpop[j].fitness      = glopop[k].fitness;
      	envpop[j].PEval        = glopop[k].PEval;
      	envpop[j].xsite        = glopop[k].xsite;
      	envpop[j].msite        = glopop[k].msite;
      	envpop[j].parent[0]    = glopop[k].parent[0];
      	envpop[j].parent[1]    = glopop[k].parent[1];
   	}//End for
#endif
   	return (envpop);
}//End selecciona_individuos_a_enviar

int FitnessComparaMaster(struct individual *Indi, struct individual *Indj)
//Función que compara Individuos utilizando como comparación el fitness de cada uno
{
   	if(Indi->fitness > Indj->fitness)
      	return(1);
   	if(Indi->fitness < Indj->fitness)
      	return(-1);
   	return(0);
}//End FitnessComparaMaster

void nomemoryMaster(char *string)
// Imprime error de memoria y sale del programa con error
{
  	fprintf(outfp,"malloc: out of memory en %s!!\n",string);
  	exit(-1);
}//End nomemoryMaster

void initmallocMaster(int coord_ind_a_recibir, int coord_ind_a_enviar, int coord_ind_globales)
// Localiza memoria para estructuras de envio y recepción en Coordinador
{
   	unsigned nbytes;
   	int j;
   
   	// Memoria para población de individuos a enviar desde Coordinador a cada AG
   	nbytes = coord_ind_a_enviar*sizeof(struct individual);
   	if((envpop = (struct individual *) malloc(nbytes)) == NULL)
      	nomemoryMaster("envpop");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, master.c, 29, envpop, %d\n", nbytes);
#endif
   	// Memoria para cromosoma string de la población de individuos a enviar desde Coordinador a cada AG
   	nbytes = chromsize*sizeof(unsigned);
   	for(j = 0; j < coord_ind_a_enviar; j++) {
      	if((envpop[j].chrom = (unsigned *) malloc(nbytes)) == NULL)
         	nomemoryMaster("envpop chromosome string");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, master.c, 37, envpop[j].chrom, %d\n", nbytes);
#endif
   	}//End for

   	// Memoria para cromosoma mutación de la población de individuos a enviar desde Cordinador a cada AG
   	nbytes = chmutsize*sizeof(unsigned);
   	for(j = 0; j < coord_ind_a_enviar; j++) {
      	if((envpop[j].chmut = (unsigned *) malloc(nbytes)) == NULL)
         	nomemoryMaster("envpop chromosome mutation");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, master.c, 50, envpop[j].chmut, %d\n", nbytes);
#endif
   	}//End for
   
   	// Memoria para cromosoma lista de la población de individuos a enviar desde Cordinador a cada AG
   	nbytes = chlistasize*sizeof(unsigned short);
   	for(j = 0; j < coord_ind_a_enviar; j++) {
      	if((envpop[j].pusListaPiezas = (unsigned short *) malloc(nbytes)) == NULL)
         	nomemoryMaster("envpop chromosome lista");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, master.c, 147, envpop[j].pusListaPiezas, %d\n", nbytes);
#endif
   	}//End for
   
   	// Memoria para población de individuos a recibir en Coordinador desde cada AG
   	nbytes = coord_ind_a_recibir * sizeof(struct individual);
   	if((recpop = (struct individual *) malloc(nbytes)) == NULL)
      	nomemoryMaster("recpop");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, master.c, 29, recpop, %d\n", nbytes);
#endif

   	// Memoria para cromosoma string de la población de individuos a recibir en Coordiandor desde cada AG
   	nbytes = chromsize*sizeof(unsigned);
   	for(j = 0; j < coord_ind_a_recibir; j++) {
      	if((recpop[j].chrom = (unsigned *) malloc(nbytes)) == NULL)
         	nomemoryMaster("recpop chromosome string");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, master.c, 37, recpop[j].chrom, %d\n", nbytes);
#endif
   	}//End for

   	// Memoria para cromosoma mutación de la población de individuos a recibir en Coordinador desde cada AG
   	nbytes = chmutsize*sizeof(unsigned);
   	for(j = 0; j < coord_ind_a_recibir; j++) {
      	if((recpop[j].chmut = (unsigned *) malloc(nbytes)) == NULL)
         	nomemoryMaster("recpop chromosome mutation");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, master.c, 50, recpop[j].chmut, %d\n", nbytes);
#endif
   	}//End for

   	// Memoria para cromosoma lista de la población de individuos a recibir en Coordinador desde cada AG
   	nbytes = chlistasize*sizeof(unsigned short);
   	for(j = 0; j < coord_ind_a_recibir; j++) {
      	if((recpop[j].pusListaPiezas = (unsigned short *) malloc(nbytes)) == NULL)
         	nomemoryMaster("recpop chromosome lista");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, master.c, 187, recpop[j].pusListaPiezas, %d\n", nbytes);
#endif
   	}//End for
   
   	// Memoria para población global de individuos recibidos en Coordinador
   	nbytes = coord_ind_globales * sizeof(struct individual);
   	if((glopop = (struct individual *) malloc(nbytes)) == NULL)
		nomemoryMaster("glopop");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, master.c, 29, glopop, %d\n", nbytes);
#endif

   	// Memoria para cromosoma string de la población global de individuos recibidos en Coordiandor
   	nbytes = chromsize*sizeof(unsigned);
   	for(j = 0; j < coord_ind_globales; j++) {
      	if((glopop[j].chrom = (unsigned *) malloc(nbytes)) == NULL)
         	nomemoryMaster("glopop chromosome string");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, master.c, 37, glopop[j].chrom, %d\n", nbytes);
#endif
   	}//End for

   	// Memoria para cromosoma mutación de la población global de individuos recibidos en Coordinador
   	nbytes = chmutsize*sizeof(unsigned);
   	for(j = 0; j < coord_ind_globales; j++) {
      	if((glopop[j].chmut = (unsigned *) malloc(nbytes)) == NULL)
         	nomemoryMaster("glopop chromosome mutation");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, master.c, 50, glopop[j].chmut, %d\n", nbytes);
#endif
   	}//End for
   
   	// Memoria para cromosoma lista de la población global de individuos recibidos en Coordinador
   	nbytes = chlistasize*sizeof(unsigned short);
   	for(j = 0; j < coord_ind_globales; j++) {
      	if((glopop[j].pusListaPiezas = (unsigned short *) malloc(nbytes)) == NULL)
         	nomemoryMaster("glopop chromosome lista");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, master.c, 225, glopop[j].pusListaPiezas, %d\n", nbytes);
#endif
   	}//End for
   
   	// Memoria para cromosoma string, mutación y lista del mejor individuo de la población
   	nbytes = chromsize*sizeof(unsigned);
   	if((bestfit.chrom = (unsigned *) malloc(nbytes)) == NULL)
      	nomemoryMaster("bestfit chromosome string");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, master.c, 42, bestfit.chrom, %d\n", nbytes);
#endif
   	nbytes = chmutsize*sizeof(unsigned);
   	if((bestfit.chmut = (unsigned *) malloc(nbytes)) == NULL)
      	nomemoryMaster("bestfit chromosome mutation");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, master.c, 55, bestfit.chmut, %d\n", nbytes);
#endif
   	nbytes = chlistasize*sizeof(unsigned short);
   	if((bestfit.pusListaPiezas = (unsigned short *) malloc(nbytes)) == NULL)
      	nomemoryMaster("bestfit chromosome lista");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, master.c, 243, bestfit.pusListaPiezas, %d\n", nbytes);
#endif
}//End initmallocMaster

void freeallMaster(int coord_ind_a_recibir, int coord_ind_a_enviar, int coord_ind_globales)
// Rutina que libera toda el espacio de memoria localizado dinámicamente
{
   	int i;
  
   	//Libera envpop
   	for(i = 0; i < coord_ind_a_enviar; i++) {
      	free(envpop[i].chrom);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, master.c, 80, envpop[i].chrom\n");
#endif
   		free(envpop[i].chmut);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, master.c, 81, envpop[i].chmut\n");
#endif
   		free(envpop[i].pusListaPiezas);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, master.c, 267, envpop[i].pusListaPiezas\n");
#endif
   	}//End for
   	free(envpop);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, master.c, 98, envpop\n");
#endif

   	//Libera recpop
   	for(i = 0; i < coord_ind_a_recibir; i++) {
      	free(recpop[i].chrom);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, master.c, 80, recpop[i].chrom\n");
#endif
   		free(recpop[i].chmut);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, master.c, 81, recpop[i].chmut\n");
#endif
   		free(recpop[i].pusListaPiezas);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, master.c, 287, recpop[i].pusListaPiezas\n");
#endif
   	}//End for
   	free(recpop);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, master.c, 98, recpop\n");
#endif

   	//Libera glopop
   	for(i = 0; i < coord_ind_globales; i++) {
      	free(glopop[i].chrom);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, master.c, 80, glopop[i].chrom\n");
#endif
		free(glopop[i].chmut);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, master.c, 81, glopop[i].chmut\n");
#endif
		free(glopop[i].pusListaPiezas);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, master.c, 307, glopop[i].pusListaPiezas\n");
#endif
   	}//End for
   	free(glopop);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, master.c, 98, glopop\n");
#endif

   	//Libera bestfit
   	free(bestfit.chrom);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, master.c, 100, bestfit.chrom\n");
#endif
   	free(bestfit.chmut);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, master.c, 101, bestfit.chmut\n");
#endif
   	free(bestfit.pusListaPiezas);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, master.c, 326, bestfit.pusListaPiezas\n");
#endif

	//Libera char_instancia_problema
	free(char_instancia_problema);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, master.c, 279, char_instancia_problema\n");
#endif
}//End freeallMaster

void statisticsMaster(struct individual *glopop, int coord_ind_globales, int workers, int coord_ind_a_recibir)
// Calcula Estadística de la población global del Coordinador
{
   	int i, j, k=-1;
   	float sqrtf(float);

   	min = glopop[0].fitness;
   	max = glopop[0].fitness;
   	med = 0.0;
   	varianza = 0.0;

   	// Loop para max, min
   	for(j = 0; j < coord_ind_globales; j++) {
      	if(glopop[j].fitness > max) max = glopop[j].fitness;         // Nuevo máximo
      	if(glopop[j].fitness < min) min = glopop[j].fitness;         // nuevo mánimo
      	med = med + glopop[j].fitness; //Suma de fitness
      	varianza = varianza + (glopop[j].fitness * glopop[j].fitness); //calculo de la varianza
      	// Nuevo mejor individuo global
      	if(glopop[j].fitness < bestfit.fitness) {
                time_mejor = MPI_Wtime() - time_start;
                bestfit.fitness = glopop[j].fitness;
         	k = j; //Quiere decir que se actualizó mejor individuo
      	}//End if
   	}//End for
   	med = (float) (med / (float) coord_ind_globales);
   	varianza = ((float) (varianza / (float) coord_ind_globales)) - (med * med);
 
   	//Calcula la Desviación Estándar de la población
   	if(varianza < 0.0) varianza = (-1.0) * varianza; //Sólo se aceptan valores +
   	desvestandar = sqrtf(varianza);
   
   	//Si actualizó mejor individuo se pasan los datos restantes del mejor individuo.
   	if(k >= 0) {
      	for(i = 0; i < chromsize; i++) // Actualiza Cromosoma String
         	bestfit.chrom[i] = glopop[k].chrom[i];
      	for(i = 0; i < chmutsize; i++)// Actualiza Cromosoma Mutación
         	bestfit.chmut[i] = glopop[k].chmut[i];
      	for(i = 0; i < chlistasize; i++)// Actualiza Cromosoma Lista
         	bestfit.pusListaPiezas[i] = glopop[k].pusListaPiezas[i];
      	bestfit.fitness    = glopop[k].fitness;
      	bestfit.PEval      = glopop[k].PEval;
      	//generation en Coordinador no tiene sentido se utiliza la variable
      	//para almacenar el nodo donde se obtiene el mejor individuo actual
      	//de la población global actual del Coordinador
      	for(i = 1; i <= workers; i++){ // Actualiza Cromosoma String
        	 if( (k - (coord_ind_a_recibir * (i - 1))) >=0 && (k - (coord_ind_a_recibir * (i - 1))) <= (coord_ind_a_recibir - 1)){
            	//gen equivale al nodo donde se obtiene el mejor individuo 
            	//actual de la población global actual del Coordinador
            	gen = i;
            	break;
         	}//End if
      	}//End for
      	bestfit.generation = gen;
   	}//End if
   
   	//Calcula Timepo actual transcurrido
   	time_middle = MPI_Wtime() - time_start;
   
   	// Escribe en archivo de Estaditicas para problema particular
   	fprintf(evofp, "%d %d %.2f %.2f %.2f %.2f %.3f\n", n_migracion, gen, min, max, med, desvestandar, time_middle);
}//End statistics

int almacena_archivo_instancia(char *nombrearchivo)
//Función que almacena en arreglo archivo instancia
{
	FILE 		*fp; //puntero que identifca el archivo de instancia
	char 		nombre_archivo[100];
	int 		cont_archivo;

	//Válido sólo para RANK=0 (Coordinador) 
	sprintf(nombre_archivo, "%s%s", ruta_instancias, nombrearchivo);
   	if((fp = fopen(nombre_archivo,"r"))== NULL){
		fprintf(outfp,"error al leer archivo %s\n",nombre_archivo);
		return 0;
   	}//End if
	//Se posiciona al final del archivo de instancia
	fseek(fp, 0L, SEEK_END);

	//Determina cuantos char representan la instancia a resolver. 
	//Este valor será enviado a cada rank de modo que sepan cuantos char deben recibir de modo de reconstruir instancia en el rank.
	cantidad_char_que_determinan_instancia_del_problema = ftell(fp); 
	if(cantidad_char_que_determinan_instancia_del_problema <= 0){
		fclose(fp);
		fprintf(outfp,"error al leer archivo %s\n",nombre_archivo);
		return 0;
   	}//End if

#ifdef _PRINT_MIGRACION_
	printf("Rank %d ha determinado que archivo %s contiene %d caracteres...\n", rank, nomarch, cantidad_char_que_determinan_instancia_del_problema);
#endif
	
   	//Solicita memoria para char_instancia_problema
   	if((char_instancia_problema = (char*) malloc((cantidad_char_que_determinan_instancia_del_problema)*sizeof(char))) == NULL)
      	nomemory("char_instancia_problema en app_lecturaarchivo_ir");
#ifdef _DEBUG_MALLOC_
	printf("Malloc, App_ir.c, 250, char_instancia_problema, %d\n", (cantidad_char_que_determinan_instancia_del_problema)*sizeof(char));
#endif
		//Se posiciona al inicio del archivo de instancia
	//fseek( fp, 0, 0);
	fseek( fp, 0L, SEEK_SET);
	cont_archivo=0;
	//Lee archivo caracter por caracter y lo asigna a char_instancia_pŕoblema
	while(!feof(fp) && cont_archivo< cantidad_char_que_determinan_instancia_del_problema-1){
		char_instancia_problema[cont_archivo]=getc(fp);

//#ifdef _PRINT_MIGRACION_
//	printf("Rank %d ha determinado que caracter %d sea '%s'...\n", rank, cont_archivo, &char_instancia_problema[cont_archivo]);
//#endif

		cont_archivo++;
	}//End while
	//Cierra archivo instancia
	cantidad_char_que_determinan_instancia_del_problema--; // no considera el último caracter del archivo original
	fclose(fp);
	return -1;
}//End almacena_archivo_instancia 

int genera_archivo_temporal(char *nombre_archivo, char *chars1, int cant_char_a_rec)
//Función que genera archivo temporal de instancia de modo que sea leido por cada rank
{
   	FILE *fr;
   	int i;

	if ((fr = fopen(nombre_archivo,"w")) == NULL){
    	fprintf(outfp,"\nNo se pudo crear archivo %s\n", nombre_archivo);
      	return 0;
   	}//End if

	//Escribe cada caracter  
	for ( i = 0; i < cant_char_a_rec; i++ ){	
		putc(chars1[i], fr);
	}//End for
   
   	//Cierra archivo temporal 
   	fclose(fr);
   	
   	return -1;
}//End genera_archivo_temporal

