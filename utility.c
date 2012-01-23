/*----------------------------------------------------------------------------*/
/* utility.c - Rutinas de Utilidad                                            */
/*----------------------------------------------------------------------------*/
 
#include <sys/times.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#include "sga.h"

void datosmalos(char *string)
// Imprime en outfp cuando existen datos err�neos
{
   	fprintf(outfp,"Error en datos de entrada : %s!!\n",string);
}//End datosmalos

int consistenciaarchivo(int workers)
// Rutina que chequea la consistencia del archivo de parametros de entrada
{
   	FILE  *fpp;
   	int   cont = 0;
   	char  nombre_archivo[100], arch_instancia[200];

	while (fscanf(infp,"%d %s %d %s %d %f %f %f %f %d %s %f", &tipo_problema, nomarch, &popsize, answer, &maxgen, &pcross, &pmutation, &pind_env, &pind_rec, &tasa_migracion, answer_mod_mig, &randomseed) != EOF) {
      	if(tipo_problema<0) {
         	datosmalos("Tipo de Problema");
         	return 0;
      	}//End if
      	sprintf(nombre_archivo,"%s%s",ruta_instancias, nomarch);
      	if ((fpp = fopen(nombre_archivo,"r"))== NULL) {
         	sprintf(arch_instancia,"Nombre de archivo de instancia: %s no existe...", nomarch);
         	datosmalos(arch_instancia);	 
         	return 0;
      	} else
         	fclose(fpp);
      	popsize = popsize / workers;
      	if(popsize%2) popsize++;
      	if((popsize<=0) || ((popsize%2) != 0)) {
         	datosmalos("Tamaño de la Población");
         	return 0;
      	}//End if
      	if((strncmp(answer,"y",1) != 0) && (strncmp(answer,"n",1) != 0)) {
         	datosmalos("Imprime reporte informativo");
         	return 0;
      	}//End if
      	if(maxgen < 1) {
         	datosmalos("Número máximo de generaciones");
         	return 0;
      	}//End if
      	if((pcross < 0.0) || (pcross > 1.0)) {
         	datosmalos("Probabilidad cruzamiento");
         	return 0;
      	}//End if
      	if((pmutation < 0.0) || (pmutation > 1.0)) {
         	datosmalos("Probabilidad mutación");
         	return 0;
      	}//End if
      	if((pind_env < 0.0) || (pind_env > 1.0)) {
         	datosmalos("Porcentaje de Individuos a Enviar");
         	return 0;
      	}//End if
		if((pind_rec < 0.0) || (pind_rec > 1.0)) {
         	datosmalos("Porcentaje de Individuos a Recibir");
         	return 0;
      	}//End if

      	n_ind_a_enviar = (int) popsize * pind_env;  //% de la subpoblación se envía
      	n_ind_a_recibir = (int) popsize * pind_rec; //% de la subpoblación se recibe
      
      	if((n_ind_a_enviar<0) || (n_ind_a_enviar>popsize)) {
         	datosmalos("Número de Individuos a Enviar");
         	return 0;
      	}//End if
      	if((n_ind_a_recibir<0) || (n_ind_a_recibir>popsize)) {
         	datosmalos("Número de individuos a Recibir");
         	return 0;
      	}//End if
      	if((tasa_migracion<=0) || (tasa_migracion>maxgen)) {
         	datosmalos("Tasa de Migración");
         	return 0;
      	}//End if
      	if((strncmp(answer_mod_mig,"A",1) != 0) && (strncmp(answer_mod_mig,"S",1) != 0)) {
         	datosmalos("Modelo de Migracion Asincrono o Sincrono");
         	return 0;
      	}//End if
      	//Verifica semilla
      	if((randomseed<=0) || (randomseed>( 1-(workers * 0.001) ))) {
         	datosmalos("Número de semilla");
         	return 0;
      	}//End if
      	cont++;
   	}//End while
   	rewind(infp);
   	return cont;
}//End consistenciaarchivo

int inicializa_archivos(int argc,char *argv[], int rank)
//Inicializa archivos globales
{
   	//Asigna ruta de instancias y resultados
  	sprintf(ruta_instancias,"%s", "instancias/"); //Ruta donde se encuentras las instancias a resolver
  	sprintf(ruta_resultados,"%s", "resultados/"); //Ruta donde se dejarán los archivos de resultados

	if(rank == 0) {
	   	//Determina input desde argumentos de la función main()
	   	numfiles = argc - 1;
	   	switch(numfiles) {
	      	case 2:
            	if((infp = fopen(argv[1],"r")) == NULL) { /* abre archivo de entrada in.txt */
               		fprintf(stderr,"!!! Error, no puede abrir archivo de entrada %s \n",argv[1]);
               		return -1;
	            }//End if 
	         	sprintf(arch_salida,"%s%s",ruta_resultados, argv[2]); //Ruta del archivo de salida (layout)
	  	        if((outfp = fopen(arch_salida,"w")) == NULL) {
	               fprintf(stderr,"!!! Error, no puede abrir archivo de salida %s \n",arch_salida);
	               return -1;
	            }
	         	break;
	      	default:
	         	fprintf(stderr,"Uso: mpirun -v -c [AG+Coord] [input file] [output file]\n");
	         	return -1;
	   	}//End switch

	  	sprintf(arch_reporte_pro,"%s%s", ruta_resultados, "reporte.pro"); //Archivo de reporte de variables importantes por corrida, relacionada por cada problema a resolver 
		//Crea un nuevo archivo de reporte de variables importantes de cada corrida de los problemas a resolver
	  	if ((rprofp = fopen(arch_reporte_pro,"w")) == NULL) {
	   		fprintf(stderr,"\nNo se pudo crear archivo %s\n", arch_reporte_pro);
	  		return -1;
	  	}//End if
	  	 
	  	sprintf(arch_reporte_alg,"%s%s", ruta_resultados, "reporte.alg"); //Archivo de reporte de variables importantes por corrida, relacionada con el algoritmo genético a resolver 
		//Crea un nuevo archivo de reporte de variables importantes del Algorito Genético
	  	if ((ralgfp = fopen(arch_reporte_alg,"w")) == NULL) {
	   		fprintf(stderr,"\nNo se pudo crear archivo %s\n", arch_reporte_alg);
	  		return -1;
	  	}//End if
	}//End if
  	
  	sprintf(arch_evolucion,"%sevol-n%d.alg", ruta_resultados, rank); //Archivo con evolución del Algoritmo Genético por cada corrida 
	//Crea un nuevo archivo de evolución
  	if ((evofp = fopen(arch_evolucion,"w")) == NULL) {
   		fprintf(stderr,"\nNo se pudo crear archivo %s\n", arch_evolucion);
  		return -1;
  	}//End if

   	//Inicializa variables que determinan si encabezados de archivos de resultados están impresas o no
   	encabezado_resultado_algoritmo = 0;
   	encabezado_resultado_problema  = 0;
   	
   	return 0;
}//End inicializa_archivos

void cierra_archivos(void) 
//Cierra Archivos importantes
{
	if(rank == 0) {
		fclose(infp); 	//Cierra archivo de entrada
		fclose(outfp); 	//Cierra archivo de salida
	   	fclose(rprofp); //Cierra archivo de reporte de variables importantes de las corridas de cada problema
	   	fclose(ralgfp); //Cierra archivo de reporte de variables importantes del Agloritmo 
	}//end if
	
	fclose(evofp);	//Cierra archivo de evolución del algoritmo genético	 
}//End cierra_archivos

void genera_resultados_algoritmo(int corrida, int tipo_problema, char *nombrearchivo, cpu_consummed_time_t *t)
//Función que imprime los resultados del algoritmo
{
	char modelo_mig[10]; //modelo A:Asíncrono, S:Síncrono de migración
	
  	if(modelo_migracion == 0)
     	sprintf(modelo_mig,"%s", "ASINCRONO");
  	else
    	sprintf(modelo_mig,"%s", "SINCRONO");

	if(encabezado_resultado_algoritmo == 0) {
		//IMPRIME RESULTADOS DEL ALGOIORIMO GENÉTICO
      	fprintf(ralgfp,"CORRIDA;TIPO_PROBLEMA;ARCHIVO_PROBLEMA;TIEMPO_CPU;TIEMPO_COMM;TIEMPO_TOTAL;");
      	fprintf(ralgfp,"MAXIMO_GENERACIONES;TAMANO_POBLACION;PROBABILIDAD_CRUZAMIENTO;");
      	fprintf(ralgfp,"PROBABILIDAD_MUTACION;WORKER_MEJOR_INDIVIDUO;");
      	fprintf(ralgfp,"NUM_IND_A_ENVIAR;NUM_IND_A_RECIBIR;TASA_MIGRACION;MODELO_MIGRACION;WORKERS\n");
      	encabezado_resultado_algoritmo = 1;//Con ésto no imprime más el encabezado
	}//End if 
	//IMPRIME RESULTADOS DEL ALGORIMO GENÉTICO
  	fprintf(ralgfp,"%d %d %s %f %f %f %d %d %f %f %d %d %d %d %s %d\n", 
	//CORRIDA;TIPO_PROBLEMA;ARCHIVO_PROBLEMA;TIEMPO_CPU;TIEMPO_IO;TIEMPO_TOTAL;MAXIMO_GENERACIONES;      			
  			corrida, tipo_problema, nombrearchivo, t->cpu_time, t->comm_time, t->elapsed_time, maxgen,
  	//TAMANO_POBLACION;PROBABILIDAD_CRUZAMIENTO;PROBABILIDAD_MUTACION;WORKER_MEJOR_INDIVIDUO;"
  			popsize, (100.0 * pcross), (100.0 * pmutation), bestfit.generation,	
	//NUM_IND_A_ENVIAR;NUM_IND_A_RECIBIR;TASA_MIGRACION;MODELO_MIGRACION;WORKERS"
  			n_ind_a_enviar, n_ind_a_recibir, tasa_migracion, modelo_mig, workers);  	
}//End genera_resultados_algoritmo
