/****************************************************************************************/
/*	   M.P.A.G.P.:   														   			*/
/*     MÁQUINA PARALELA BASADA EN ALGORITMOS GENÉTICOS PARALELOS               			*/
/*     QUE RESUELVE CUALQUIER PROBLEMA DE OPTIMIZACIÓN DE ALTA COOMPLEJIDAD    			*/
/*																		       			*/
/*     Sr. Fernando Romero Z.                                                  			*/
/*     Trabajo para optar al grado de Doctor en Ciencias de la Ingeniería,     			*/
/*     Mención Ingeniería Informática.          							   			*/
/****************************************************************************************/

/************************************ VERSION *******************************************/
/* 03/05/2008 - 16:51 hrs => Comienza con hacer separable problema PGA,		   			*/ 
/*  						 separando las funciones y variables que pertenecen			*/ 
/*							 al problema a resolver... 						   			*/
/* 27/05/2008 - 22:54 hrs => Continúa con separación...						   			*/
/* 09/06/2008 - 23:30 hrs => Comienza con reestructuración completa... 		   			*/
/* 21/06/2008 - 14:52 hrs => Implementa 3º problema (piezas irregulares secuencial)...  */
/* 13/09/2008 - 13:03 hrs => Implementa 3° problema (piezas irregulares paralelo)...    */
/****************************************************************************************/

/********************************** ADVERTENCIA A CONSIDERAR ****************************/
/* 20/07/2009 - 00:51 hrs => Para correr en cluster, se debe comentariar/descomentariar */
/*                           3 líneas de código en archivo random.c y luego compilar    */
/*                           con Makefile directamente en cluster                       */
/*     OJO, OJO, OJO, OJO => Las intancias deben terminar con <CR> en la última línea   */
/*                           de datos, por ejemplo :                                    */
/*										Nro. Línea	| Datos								*/ 
/*											1		| 600 200							*/
/*											2		| 3									*/
/*											3		| 20 51 3							*/
/*											4		| 10 14 6							*/
/*											5		| 16 33	9		(aquí debe ir <CR>)	*/
/*											6		| 									*/
/********************************** ADVERTENCIA A CONSIDERAR ****************************/


#include <stdio.h>
#include <string.h>
#include <limits.h> 
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>
#include <mpi.h>

#include "sga.h"
#include "generate.h"
#include "memory.h"
#include "statistic.h"
#include "utility.h"
#include "initial.h"
#include "app.h"
#include "app2.h"
#include "random.h"
#include "master.h"
#include "comunica.h"
   
int main(int argc,char *argv[])
{
   	//Inicia MPI
   	inicializa_mpi(argc,argv);
   	
  	// Obtiene rutas de trabajo y crea archivos generales
  	if(inicializa_archivos(argc, argv, rank) == -1) exit(-1); 
  	   
   	#ifdef _PRINT_MIGRACION_
   		printf("#Bienvenido a MAPAPOC (MÁquina PAralela para Problemas de Optimización Combinatoria) en Rank %d... [OK]\n", rank);
    #endif
      
   	if(rank != 0){
      	//Inicializa semilla aleatoria
      	inicializa_semilla();
   
      	//Espera que nodo0 entregue dato runmax (cantidad de archivos a procesar)
      	for(;;){
         	MPI_Iprobe(nodo0, MPI_ANY_TAG, MPI_COMM_WORLD, &flag1, &status);
         	if(flag1 == true){
         		if (status.MPI_TAG == MSJ_RUNMAX){
               		MPI_Recv(&runmax, 1, MPI_INT, nodo0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
               		flag1 = 0;
               		#ifdef _PRINT_MIGRACION_
                  		printf("Rank = %d Recibió desde Rank = 0 Mensaje RUNMAX = %d (cantidad máxima de instancias a resolver)...\n", rank, runmax);
               		#endif
               		break;
            	}//End else if
         	}//End if
      	}//End for
   
      	for(run=1;run<=runmax;run++) {
      		#ifdef _PRINT_MIGRACION_
         		printf("Rank = %d Antes de generar semilla aleatoria...\n", rank);
      		#endif
         	//Nueva semilla aleatoria
         	for(rank_seed = 1; rank_seed <= rank; rank_seed++) {
            	do {
               		randomseed = nueva_semilla();
            	}while(randomseed == 0);
         	}//End for
      		#ifdef _PRINT_MIGRACION_
         		printf("Rank = %d Después de generar semilla aleatoria...\n", rank);
      		#endif
         
         	//Espera que nodo0 entregue información relevante para nodox
         	for(;;){
            	MPI_Iprobe(nodo0, MPI_ANY_TAG, MPI_COMM_WORLD, &flag1, &status);
            	if(flag1 == true){
               		if (status.MPI_TAG == MSJ_LINEA_IN){
                  		MPI_Recv(linea_in, 100, MPI_CHAR, nodo0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                  		flag1 = 0;
                  		//Asigna variables globales desde linea_in recibida desde Rank 0
                 		//sscanf(linea_in,"%d %s %d %s %d %f %f %f %d %s %f", &tipo_problema, nomarch, &popsize, answer, &maxgen, &pcross, &pmutation, &pind_env_rec, &tasa_migracion, answer_mod_mig, &randomseed);
						sscanf(linea_in,"%d %s %d %s %d %f %f %f %f %d %s %f", &tipo_problema, nomarch, &popsize, answer, &maxgen, &pcross, &pmutation, &pind_env, &pind_rec, &tasa_migracion, answer_mod_mig, &randomseed);                 		
						randomseed = randomseed + (rank * 0.001);
						//printf("#Rank = %d estableción semilla : %f\n", rank, randomseed);
                  		#ifdef _PRINT_MIGRACION_
                     		printf("Rank = %d Recibió desde Rank = 0 Mensaje LINEA_IN (PARAMETROS GLOBALES)...\n", rank);
                  		#endif
               		}//End if
               		else if (status.MPI_TAG == MSJ_CANT_CHAR_A_REC) {
                  		//cantidad_char_a_recibir es la variable que indica cuantos enteros recibirá nodox desde Rank 0
                  		MPI_Recv(&cantidad_char_a_recibir, 1, MPI_INT, nodo0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                  		flag1 = 0;
                  		#ifdef _PRINT_MIGRACION_
                     		printf("Rank = %d Recibió desde Rank = 0 Mensaje MSJ_CANT_CHAR_A_REC...\n", rank);
                  		#endif
                  		break;
               		}//End else if
               		else if (status.MPI_TAG == MSJ_ERROR_ARCHIVO_INSTANCIA) {
                  		MPI_Recv(0, 0, MPI_INT, nodo0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                  		flag1 = 0;
                  		cantidad_char_a_recibir = -1; //setea a -1 para indicar que hubo error al leer archivo instancia
                  		#ifdef _PRINT_MIGRACION_
                     		printf("Rank = %d Recibió desde Rank = 0 Mensaje MSJ_ERROR_ARCHIVO_INSTANCIA...\n", rank);
                  		#endif
                  		break;
               		}//End else if
            	}//End if
         	}//End for
      
         	//Setea variable printstrings
         	if(strncmp(answer,"n",1) == 0) printstrings = 0;
         	else printstrings = 1;
   
         	//Setea variables dependiendo de la cantidad de workers 
         	popsize = popsize / workers;
         	if(popsize%2) popsize++;
         	//n_ind_a_enviar = (int) popsize * pind_env_rec;  //% de la subpoblación se envía
         	//n_ind_a_recibir = (int) popsize * pind_env_rec; //% de la subpoblación se recibe
		n_ind_a_enviar = (int) popsize * pind_env;  //% de la subpoblación se envía
         	n_ind_a_recibir = (int) popsize * pind_rec; //% de la subpoblación se recibe
         	//tasa_migracion = maxgen / workers; //tasa de migración depende de la cantidad de generaciones
         	#ifdef _PRINT_MIGRACION_
	            printf("Rank = %d tiene popsize %d, n_ind_a_enviar %d, n_ind_a_recibir %d, tasa_migracion %d...\n", rank, popsize, n_ind_a_enviar, n_ind_a_recibir, tasa_migracion);
         	#endif
         
         	// nro. que identifica que migración está ocurriendo
         	n_migracion = 0;
   
         	//Inicializa contador de segundos de comunicación 
         	time_comm = 0.0;
         
         	//Setea variable modelo_migracion
         	if(strncmp(answer_mod_mig,"A",1) == 0) modelo_migracion = 0; // Migración Asíncrona
         	else modelo_migracion = 1; //Migración Síncrona
      
			#ifdef _PRINT_MIGRACION_
				printf("Rank %d debería recibir %d caracteres representando la instancia a resolver...\n", rank, cantidad_char_a_recibir);
			#endif
	 
         	if(cantidad_char_a_recibir > 0) {
         		
            	//Lee enteros con información del archivo instancia  
            	if(lee_char_y_genera_achivo_instancia_tmp(tipo_problema, cantidad_char_a_recibir)) {
            
					#ifdef _PRINT_MIGRACION_
						printf("Espere, MAPAPOC en Rank %d está procesando archivo %s...\n", rank, nomarch);
					#endif
					
					//Actualiza Resultados Estadísticos Evolutivos para cada problema particular
				   	fprintf(evofp, "\n\nCorrida : %d, PROBLEMA : %s\n", run, nomarch);
				   	fprintf(evofp, "Migracion Generacion Minimo Maximo Media DesvEstandar TiempoTranscurrido GenMejor\n");
				   	// Inicia la cuenta de Segundos
				   	time_start = MPI_Wtime();
               
	               	// Rutina de inicialización de variables globales
    	           	initialize();
               
        	       	// Ubica espacio malloc para las estructuras de datos globales
            	   	initmalloc();
      	
        	       	// Define tipos de Individuos a Enviar y Recibir
            	   	Build_type_pop(envpop, &message_type_send, n_ind_a_enviar);
               		Build_type_pop(recpop, &message_type_receive, n_ind_a_recibir);
               
               		// Initializa la población de individuos y determina datos estadísticos
               		// y mejor individuo de la población
               		initpop(tipo_problema);
               		statistics(oldpop);
	      
    	           	//Avisa a nodo0 que esté listo para comenzar evolución...
               		time_send = MPI_Wtime();
               		MPI_Isend(0, 0, MPI_INT, nodo0, MSJ_LISTO_PARA_COMENZAR, MPI_COMM_WORLD, &request);
               		for(;;){
                  		MPI_Test(&request, &flag2, &status);
                  		if(flag2 == true){
                     		time_send = MPI_Wtime() - time_send;
                     		time_comm += time_send;
                     		#ifdef _PRINT_MIGRACION_
                        		printf("Envió desde Rank = %d a Rank = 0 Mensaje Listo para comenzar...\n", rank);
                     		#endif
                     		break;
                  		}//End If
               		}//End for
                  
               		//Espera que nodo0 dé la partida para comenzar evolución...
               		for(;;){
                  		MPI_Iprobe(nodo0, MPI_ANY_TAG, MPI_COMM_WORLD, &flag1, &status);
                  		if(flag1 == true){
                     		if (status.MPI_TAG == MSJ_COMIENCE){
                        		MPI_Recv(0, 0, MPI_INT, nodo0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                        		flag1 = 0;
                        		#ifdef _PRINT_MIGRACION_
                           			printf("Rank = %d Recibió desde Rank = 0 Mensaje de COMINECE, comienza evolución...\n", rank);
		                        #endif
        		                break;
                			}//End if
                  		}//End if
               		}//End for
                        
               		//Setea contador de generaciones para Migrar en Modelo de Migración Asíncrona
               		if (modelo_migracion == 0) cuenta_gen_para_migracion=1;
	               
    	           	for(gen=0; gen<maxgen; gen++) {
        	         	if(printstrings == 1) fprintf(outfp,"\nGENERATION %d->%d\n",gen,maxgen);
      
                  		// Crea una nueva generación
                  		generation(tipo_problema);
      
                  		// Efectúa estadísticas sobre nueva población y obtiene mejor individuo
                  		statistics(newpop);
                  
                  		if (modelo_migracion == 0) 
                     		//Establece comunicación Asincrona con Coordinador
                     		comunicacion_asincrona_con_coordinador();         
                  		else
                     		//Establece comunicación sincrona con Coordinador
                     		comunicacion_sincrona_con_coordinador();         
                  
                  		// Avanza de Generación
                  		temppop = oldpop;
                  		oldpop = newpop;
                  		newpop = temppop;
               		}//End for
      
               		// Libera memoria temporal
               		freeall();
      
               		//Libera memoria tipos creados por MPI
               		MPI_Type_free(&message_type_send);
               		MPI_Type_free(&message_type_receive);
            
            	}//End if
            	else
               		printf("!!! ADVERTENCIA ¡¡¡ Rank %d no procesó archivo de instancia ya que hubo problemas al leer enteros, generar o leer tmp...\n", rank);
         	}//End if
         	else 
            	printf("!!! ADVERTENCIA ¡¡¡ Rank %d no procesó archivo de instancia ya que Rank 0 tuvo problemas al leerlo...\n", rank);
        	
	      	// Libera variables del problema  
	      	app_free(tipo_problema);

	        //Mensaje de término de procesamiento de archivo actual se envía a Rank = 0
	        MPI_Isend(&time_comm, 1, MPI_DOUBLE, nodo0, MSJ_TERMINO, MPI_COMM_WORLD, &request);
	   		for(;;){
	         	MPI_Test(&request, &flag2, &status);
	            if(flag2 == true){
	               	#ifdef _PRINT_MIGRACION_
	                	printf("Envió desde Rank = %d a Rank = 0 Mensaje de Término, archivo %s...\n", rank, nomarch);
	               	#endif
	               	break;
	            }//End If
	        }//End for
         
	        //Espera que nodo0 dé el OK para poder finalizar el procesamiento del archivo actual
	        for(;;){
	          	MPI_Iprobe(nodo0, MPI_ANY_TAG, MPI_COMM_WORLD, &flag1, &status);
	            if(flag1 == true){
	            	if (status.MPI_TAG == MSJ_TERMINO_CONFIRMADO){
	                	MPI_Recv(0, 0, MPI_INT, nodo0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	                  	flag1 = 0;
	                  	#ifdef _PRINT_MIGRACION_
	                     	printf("Rank = %d Recibió desde Rank = 0 Mensaje de TERMINO_CONFIRMADO, pasa siguiente archivo o termina...\n", rank);
	                  	#endif
	                  	break;
	               	}//End if
	            }//End if
	      	}//End for
	        #ifdef _PRINT_MIGRACION_
	        	printf("#Corrida %d, Archivo %s procesado por Rank %d...   [OK]\n", run, nomarch, rank);
	        #endif
	  	}//End for
   
	    if(runmax == 0){
	    	//Que runmax = 0 significa que la consistencia del archivo arrojó un error 
	        //=> rank debe mandar mensaje al Coordinador que va ha terminar el proceso...
	        //Mensaje de término de procesamiento de archivo actual se envía a Rank = 0
	        MPI_Isend(&time_comm, 1, MPI_DOUBLE, nodo0, MSJ_TERMINO, MPI_COMM_WORLD, &request);
	        for(;;){
	        	MPI_Test(&request, &flag2, &status);
	            if(flag2 == true){
	             	#ifdef _PRINT_MIGRACION_
	                	printf("Envió desde Rank = %d a Rank = 0 Mensaje de Término, archivo %s...\n", rank, nomarch);
	               	#endif
	               	break;
	        	}//End If
	        }//End for
	        printf("Proceso en Rank %d detenido ya que Rank 0 informa error en archivo de entrada...   [OK]\n", rank);
	 	}//End if
  	}//End if
   	else {
      	//Rank = 0 => Coordinador
   		printf("#Bienvenido a Máquina Paralela para Problemas de Optimización Combinatoria, espere por favor...\n");
      	
      	// Revisa consistencia del archivo de entrada in.txt
      	runmax = consistenciaarchivo(workers);
      	
      	//Envía runmax a cada Rank
      	envia_a_cada_rank(MSJ_RUNMAX);
   
      	for(run=1;run<=runmax;run++) {
         	//Lee archivo en infp con parametros (todos ya chequeados)
         	//OJO => Cada AG tendrá popsize/workers individuos => Población Total = popsize          	
			//fscanf(infp,"%d %s %d %s %d %f %f %f %d %s %f", &tipo_problema, nomarch, &popsize, answer, &maxgen, &pcross, &pmutation, &pind_env_rec, &tasa_migracion, answer_mod_mig, &randomseed);
   			fscanf(infp,"%d %s %d %s %d %f %f %f %f %d %s %f", &tipo_problema, nomarch, &popsize, answer, &maxgen, &pcross, &pmutation, &pind_env, &pind_rec, &tasa_migracion, answer_mod_mig, &randomseed);
         	
			//Inicializa contador de segundos de comunicación 
         	time_comm = 0.0;
         
         	//Llena con información linea_in para ser enviada a cada Rank
         	//sprintf(linea_in,"%d %s %d %s %d %f %f %f %d %s %f\n", tipo_problema, nomarch, popsize, answer, maxgen, pcross, pmutation, pind_env_rec, tasa_migracion, answer_mod_mig, randomseed);
			sprintf(linea_in,"%d %s %d %s %d %f %f %f %f %d %s %f", tipo_problema, nomarch, popsize, answer, maxgen, pcross, pmutation, pind_env, pind_rec, tasa_migracion, answer_mod_mig, randomseed);         	

       		#ifdef _PRINT_MIGRACION_
       			printf("Espere, MAPAPOC en Rank %d está enviando archivo %s a todos los rank...\n", rank, nomarch);
       		#endif

        	//Envía linea_in a cada Rank
      		envia_a_cada_rank(MSJ_LINEA_IN);
      	
         	//Setea variable printstrings
         	if(strncmp(answer,"n",1) == 0) printstrings = 0;
         	else printstrings = 1;
      
         	//Setea variables dependiendo de la cantidad de workers 
         	popsize = popsize / workers;
         	if(popsize%2) popsize++;
         	//n_ind_a_enviar = (int) popsize * pind_env_rec;  //% de la subpoblación se envía
         	//n_ind_a_recibir = (int) popsize * pind_env_rec; //% de la subpoblación se recibe
			n_ind_a_enviar = (int) popsize * pind_env;  //% de la subpoblación se envía
         	n_ind_a_recibir = (int) popsize * pind_rec; //% de la subpoblación se recibe
         	//tasa_migracion = maxgen / workers; //tasa de migración debende de la cantidad de generaciones
               
         	//Setea variable modelo_migracion
         	if(strncmp(answer_mod_mig,"A",1) == 0) modelo_migracion = 0; //Migración Asíncrona
         	else modelo_migracion = 1; //Migración Síncrona
         
         	if(app_leearchivo(tipo_problema, nomarch, rank)) {     
            	if(almacena_archivo_instancia(nomarch)) {
	            	//Determina cantidad_char_a_recibir por cada rank que coordinador enviará. Todos estos char determinan los datos del problema a resolver
	            	cantidad_char_a_recibir = cantidad_char_que_determinan_instancia_del_problema;
	            	
	            	//Envía a cada Rank cantidad de char a recibir del problema
	            	envia_a_cada_rank(MSJ_CANT_CHAR_A_REC); 
	            
	            	//Envía a cada Rank un grupo de char que determinan la estructura de una instancia del problema
	            	envia_a_cada_rank(MSJ_CHAR_ARCHIVO_INSTANCIA);
	            	
	        		// Rutina de inicialización de variables globales
	           		// nro. individuos a enviar desde coord. a nodo AG
	           		coord_ind_a_env  = n_ind_a_recibir;  
	           		// nro. individuos a recibir desde cada nodo AG a coord
	           		coord_ind_a_rec = n_ind_a_enviar; 
	           		// nro. individuos total recibidos => Población Global del Coordinador
	           		coord_ind_global = workers * n_ind_a_enviar; 
	           		// nro. que identifica qué migración está ocurriendo
	           		n_migracion = 1;  
	           		#ifdef _PRINT_MIGRACION_
	           			printf("Espere, MAPAPOC en Rank %d está procesando archivo %s...\n", rank, nomarch);
	              		printf("Rank = %d tiene popsize %d, coord_ind_a_env %d, coord_ind_a_rec %d, coord_ind_global % d, tasa_migracion %d...\n", rank, popsize, coord_ind_a_env, coord_ind_a_rec, coord_ind_global, tasa_migracion);
	           		#endif
	     
				   	//Actualiza Resultados Estadísticos Evolutivos para cada problema particular
				   	fprintf(evofp, "\n\nCorrida : %d, PROBLEMA : %s\n", run, nomarch);
				   	fprintf(evofp, "Migracion Generacion Minimo Maximo Media DesvEstandar TiempoTranscurrido\n");
				   	// Inicia la cuenta de Segundos
				   	time_start = MPI_Wtime();
	           
	           		//Rutina de inicialización de variables globales
	           		initialize();
	           
	           		//Dimensiona poblaciones de individuos que recibe y envía...
	           		initmallocMaster(coord_ind_a_rec, coord_ind_a_env, coord_ind_global);
	     
	           		//Define tipos de Individuos a Enviar y Recibir
	           		Build_type_pop(envpop, &message_type_send, coord_ind_a_env);
	           		Build_type_pop(recpop, &message_type_receive, coord_ind_a_rec);
	  
	           		if (modelo_migracion == 0) 
	              		//Establece comunicación Asincrona con cada rank (AG)
		              		comunicacion_asincrona_con_cada_rank();         
	           		else
	              		//Establece comunicación sincrona con cada rank (AG)
	              		comunicacion_sincrona_con_cada_rank();         
	              
				  	// Calcula cantidad de segundos que demoró en cada Algoritmo Genético
				  	time_end = MPI_Wtime() - time_start;
				  	time_consumation.elapsed_time = time_end;
				  	time_consumation.comm_time = time_comm;
				  	time_consumation.cpu_time = time_end - time_comm;
				  	
				  	//Graba datos en archivo de resultados del algoritmo
					genera_resultados_algoritmo(run, tipo_problema, nomarch, &time_consumation);
				
					//Graba datos en archivo de resultados del problema
					app_genera_resultados_problema(run, tipo_problema, nomarch);
	
				  	//IMPRIME SALIDA PARA PROGRAMA PARAMILS
				  	printf("RunsExecuted = 1\n");
				  	printf("CPUTime_Mean = %f\n", time_consumation.elapsed_time);
				  	printf("BestSolution_Mean = %f\n", bestfit.fitness);

				  	//Genera la Salida hacia archivo de Resultados LAYOUT
				  	app2_objfunc(tipo_problema, nomarch, bestfit, run);
				  	
	           		//Libera memoria del Coordinador
	           		freeallMaster(coord_ind_a_rec, coord_ind_a_env, coord_ind_global);
	           
	           		//Libera memoria tipos creados por MPI
	           		MPI_Type_free(&message_type_send);
	           		MPI_Type_free(&message_type_receive);
	         	}//End if
	         	else {
	            	printf("!!! ERROR ¡¡¡ Rank 0, en datos del archivo de piezas <%s> => Archivo no procesado...\n", nomarch);
	            	//Envía a cada Rank que ARCHIVO_INSTANCIA está con algún error
	            	envia_a_cada_rank(MSJ_ERROR_ARCHIVO_INSTANCIA); 
	         	}//End else
         	}//End if
         	else {
            	printf("!!! ERROR ¡¡¡ Rank 0, en datos del archivo de piezas <%s> => Archivo no procesado...\n", nomarch);
            	//Envía a cada Rank que ARCHIVO_INSTANCIA está con algún error
            	envia_a_cada_rank(MSJ_ERROR_ARCHIVO_INSTANCIA); 
         	}//End else
	      	// Libera variables del problema  
	      	app_free(tipo_problema);

   			#ifdef _PRINT_MIGRACION_
         		printf("#Corrida %d, Archivo %s procesado por Coordinador...   [OK]\n", run, nomarch);
         	#else
         		printf("#Corrida %d, Archivo %s procesado...   [OK]\n", run, nomarch);
         	#endif
      	}//End for
      	if(runmax == 0){
         	//Que runmax = 0 significa que la consistencia del archivo arrojó un error 
         	printf("Archivo %s NO procesado por Rank %d debido a error...   [OK]\n", nomarch, rank);
      	}//End if
   	}//End else    

	//Cierra archivos
	cierra_archivos();
	
   	//Rutina para MPI
   	MPI_Finalize();
 
   	#ifdef _PRINT_MIGRACION_
   		printf("Terminó rank = %d...\n", rank);
    #endif
   
	exit(0);
}//End main
