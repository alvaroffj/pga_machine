/*----------------------------------------------------------------------------*/
/* comunica.c - rutinas de comunicación Asincrona y Síncrona con Coordinador  */
/*----------------------------------------------------------------------------*/

#include <string.h>

#include "memory.h"
#include "sga.h"
#include "statistic.h"
#include "master.h"
#include "app.h"

void envia_mensaje(int tipo_mensaje, char *texto_mensaje)
//Rutina que permite enviar un mensaje a todos los nodos
{
 	for(nodox = 1; nodox < numtasks; nodox++){  
        //Enviando mensaje a nodox  => tipo_mensaje
    	time_send = MPI_Wtime();
    	MPI_Isend(0, 0, MPI_INT, nodox, tipo_mensaje, MPI_COMM_WORLD, &request);
    	for(;;){
       		MPI_Test(&request, &flag2, &status);
       		if(flag2 == true){
          		time_send = MPI_Wtime() - time_send;
          		time_comm += time_send;
          		#ifdef _PRINT_MIGRACION_
             		printf("Envió desde Rank = 0 a Rank = %d Mensaje %s...\n", nodox, texto_mensaje);
          		#endif
          		break;
       		}//End if
    	}//End for
 	}//End for
}//End envia_mensaje

void comunicacion_asincrona_con_coordinador(void) 
//Realiza comunicación Asincrona desde cada Rank con Coordiandor
{
  	espera_enviar_individuos = false;
   	do {
      	//Revisa si sebe recibir algún mensaje desde Coordinador
      	MPI_Iprobe(nodo0, MPI_ANY_TAG, MPI_COMM_WORLD, &flag1, &status);
      	if(flag1 == true){
         	if (status.MPI_TAG == MSJ_ENVIE_INDIVIDUOS){
            	MPI_Recv(0, 0, MPI_INT, nodo0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            	flag1 = 0;
            	#ifdef _PRINT_MIGRACION_
               		printf("Rank = %d Recibió desde Rank = 0 Mensaje ENVIE_INDIVIDUOS, enviado ahora...\n", rank);
            	#endif
            	//Obtiene los Y individuos a enviar (selecciona los mejores o al azar)
            	pop_a_enviar = obtiene_individuos_a_enviar(oldpop, newpop);
            	time_send = MPI_Wtime();
            	if (gen == maxgen - 1)
               		//Quiere decir que nodox llegó a la última generación => envía por última vez los mejores ind.
               		MPI_Isend(pop_a_enviar, 1, message_type_send, nodo0, MSJ_DATOS_ULTIMA_VEZ, MPI_COMM_WORLD, &request);
            	else            
               		MPI_Isend(pop_a_enviar, 1, message_type_send, nodo0, MSJ_DATOS, MPI_COMM_WORLD, &request);
            	for(;;){
               		MPI_Test(&request, &flag2, &status);
               		if(flag2 == true){
                  		time_send = MPI_Wtime() - time_send;
                  		time_comm += time_send;
                  		#ifdef _PRINT_MIGRACION_
                     		if (gen == maxgen - 1)
                        		//Quiere decir que nodox llegó a la última generación => envía por última vez los mejores ind.
                        		printf("Envió a Rank = 0 desde Rank = %d Mensaje de Datos Ultima Vez, gen = %d, queda a la espera de Mensaje SIGA...\n", rank, gen);
                     		else
                        		printf("Envió a Rank = 0 desde Rank = %d Mensaje de Datos, gen = %d, queda a la espera de Mensaje con Población a Insertar...\n", rank, gen);
                  		#endif
                  		//Espera hasta recibir el MSJ_DATOS_COORDINADOR y MSJ_SIGA desde nodo0
                  		for(;;){
                     		MPI_Iprobe(nodo0, MPI_ANY_TAG, MPI_COMM_WORLD, &flag1, &status);
                     		if(flag1 == true){
                        		if (status.MPI_TAG == MSJ_SIGA){
	                           		MPI_Recv(0, 0, MPI_INT, nodo0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	                           		flag1 = 0;
	                           		#ifdef _PRINT_MIGRACION_
	                              		printf("Rank = %d Recibió desde Rank = 0 Mensaje SIGA, sigue evolución...\n", rank);
	                           		#endif
	                           		break;
	                        	}//End if
	                        	else if (status.MPI_TAG == MSJ_DATOS_COORDINADOR) {
	                           		//Recibe los individuos desde Coordinador y los inserta en Población Local del AG
	                           		MPI_Recv(recpop, 1, message_type_receive, nodo0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	                           		flag1=0;
	                           		//Inserta individuos recibidos en población actual (newpop) del AG
	                           		inserta_individuos_en_poblacion(recpop, newpop, oldpop);
	                           		#ifdef _PRINT_MIGRACION_
	                              		printf("Rank = %d Recibió desde Rank = 0 Mensaje con Individuos a Insertar y los insertó...\n", rank);
	                           		#endif
	                        	}//End else if
	                        	else if (status.MPI_TAG == MSJ_TERMINE_EVOLUCION){
	                           		MPI_Recv(0, 0, MPI_INT, nodo0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	                           		flag1 = 0;
	                           		#ifdef _PRINT_MIGRACION_
	                              		printf("Rank = %d Recibió desde Rank = 0 Mensaje TERMINE_EVOLUCION...\n", rank);
	                           		#endif
	                           		gen = maxgen;
	                           		//Esta variable se setea en true sólo para que no entre al if siguiente y continúe con la evolución
	                           		espera_enviar_individuos = true;
	                        	}//End else if
                    		}//End if
                  		}//End for
                  		break;
               		}//End if
            	}//End for
            	//Reinicia Contador de Generaciones para migrar.
            	cuenta_gen_para_migracion = 0;
            	//Incrementa contador de nro. migración actual
            	n_migracion++;
         	}//End else if
      	}//End if
      	//Si debe enviar datos => los envía
      	if(((((cuenta_gen_para_migracion % tasa_migracion) == 0) && (cuenta_gen_para_migracion > 0)) || (gen == maxgen - 1) || (bestfit.fitness == 0.0)) && (espera_enviar_individuos == false)) {  
         	//Setea variable que indica a Rank que espere hasta que llege mensaje de envío de ind. al Coordinador
         	espera_enviar_individuos = true;
         	//Avisa a nodo0 que éste llegó al valor de tasa_migracion y que debe realizarse ahora la migracion
         	time_send = MPI_Wtime();
         	MPI_Isend(0, 0, MPI_INT, nodo0, MSJ_LISTO_PARA_MIGRAR, MPI_COMM_WORLD, &request);
         	for(;;){
            	MPI_Test(&request, &flag2, &status);
            	if(flag2 == true){
               		time_send = MPI_Wtime() - time_send;
               		time_comm += time_send;
               		#ifdef _PRINT_MIGRACION_
                  		if (bestfit.fitness == 0.0)
                     		printf("Envió desde Rank = %d a Rank = 0 Mensaje Listo para Migrar en generación [%d] ya que bestfit.fitness = 0.0...\n", rank, gen);
		                else if (gen == maxgen - 1)
                     		printf("Envió desde Rank = %d a Rank = 0 Mensaje Listo para Migrar por última vez en generación [%d]...\n", rank, gen);
                  		else 
                     		printf("Envió desde Rank = %d a Rank = 0 Mensaje Listo para Migrar en generación [%d]...\n", rank, gen);
               		#endif
               		break;
            	}//End If
         	}//End for
      	}//End if
      	//Ya envió individuos al Coordinador => Continúa evolución
      	if (cuenta_gen_para_migracion == 0) espera_enviar_individuos = false;
   	} while (espera_enviar_individuos);
   
   	//Incrementa contador de Generaciones
   	cuenta_gen_para_migracion++ ;
}//End comunicacion_asincrona_con_coordinador

void comunicacion_asincrona_con_cada_rank(void)
//Establece comunicación Asíncrona con cada rank (AG)
{
   	int workers_actuales[workers + 1]; //Lleva cuales Rank están activos

   	//Setea todos los workers_actuales como activos
   	for(nodox = 1; nodox < numtasks; nodox++){  
      	//Enviando mensaje a nodox  => COMIENCE
      	workers_actuales[nodox] = true;
   	}//End for
   
   	//Esta variable debe usarse sólo si se desea terminar la evolución antes de maxgen
   	//LISTO_PARA_TERMINAR_EVOLUCION = 3 * (numtasks - 1); //Valor 3 es un parámetro cualquiera
   	for(;;){
      	MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag1, &status);
      	if(flag1 == true){
         	nodox = status.MPI_SOURCE;
         	if ((status.MPI_TAG == MSJ_DATOS) || (status.MPI_TAG == MSJ_DATOS_ULTIMA_VEZ)){
				#ifdef _PRINT_MIGRACION_
               		printf("Rank = 0 va ahora a recibir desde Rank = %d Mensaje DATOS...\n", nodox);
            	#endif
            	MPI_Recv(recpop, 1, message_type_receive, nodox, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            	#ifdef _PRINT_MIGRACION_
               		printf("Rank = 0 recibió con éxito desde Rank = %d Mensaje DATOS...\n", nodox);
            	#endif
            	flag1=0;
            	if (status.MPI_TAG == MSJ_DATOS)
	               	LISTO_PARA_SEGUIR --;
            	else { 
               		LISTO_PARA_SEGUIR_ULTIMA_VEZ --;
               		//Esto porque ya este nodo no sigue más en la carrera evolutiva...
               		LISTO_PARA_SEGUIR --;
               		workers_actuales[nodox] = false;
            	}//End else   
            	//*****************************************************
            	//ALMACENA LA POBLACION RECIBIDA EN LA POBLACION GLOBAL
            	//*****************************************************
            	almacena_ind_recibidos_en_pop_global(recpop, coord_ind_a_rec, nodox);
            	//Esta variable debe usarse sólo si se desea terminar la evolución antes de maxgen
            	//LISTO_PARA_TERMINAR_EVOLUCION --;
         	} else if (status.MPI_TAG == MSJ_LISTO_PARA_COMENZAR){
            	MPI_Recv(0, 0, MPI_INT, nodox, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            	flag1 = 0;
            	#ifdef _PRINT_MIGRACION_
               		printf("Rank = 0 Recibió desde Rank = %d Mensaje Listo para Comenzar...\n", nodox);
            	#endif
            	LISTO_PARA_COMENZAR --;
         	} else if (status.MPI_TAG == MSJ_LISTO_PARA_MIGRAR){
            	MPI_Recv(0, 0, MPI_INT, nodox, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            	flag1 = 0;
            	#ifdef _PRINT_MIGRACION_
               		printf("Rank = 0 Recibió desde Rank = %d Mensaje Listo para Migrar, enviando mensajes para que Rank's Migren...\n", nodox);
            	#endif
            	if (LISTO_PARA_MIGRAR_TODOS_LOS_RANK == 0)	LISTO_PARA_MIGRAR_TODOS_LOS_RANK = 1;
         	} else if (status.MPI_TAG == MSJ_TERMINO){
	            MPI_Recv(&time_comm_rank, 1, MPI_DOUBLE, nodox, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            	time_comm += time_comm_rank;
            	flag1 = 0;
            	#ifdef _PRINT_MIGRACION_
               		printf("Rank = 0 Recibió desde Rank = %d Mensaje de Término (Tiempo Comm = %f), Rank = 0 confirmaró término...\n", nodox, time_comm_rank);
            	#endif
            	LISTO_PARA_TERMINAR --;
         	}//End else if
      	}//End if
      	if(LISTO_PARA_COMENZAR == 0){
         	LISTO_PARA_COMENZAR --; //Decrementa listo_para_comenzar quedando en un valor negativo => no mas envío de COMIENCE
			//Envía mensaje MSJ_COMIENCE
			envia_mensaje(MSJ_COMIENCE, "COMIENCE");
         	//Setea variable para que pueda recibir despues una sola vez este mensaje
         	LISTO_PARA_MIGRAR_TODOS_LOS_RANK = 0;
      	}//End if
      	if(LISTO_PARA_SEGUIR == 0) {
         	LISTO_PARA_SEGUIR = 0;
         	//Setea todos los workers_actuales como activos
         	for(nodox = 1; nodox < numtasks; nodox++){  
            	if (workers_actuales[nodox] == true) 
               		LISTO_PARA_SEGUIR++; //Variable queda lista para recibir mensajes de nuevo de los ranks que quedan activos
         	}//End for
         	if (LISTO_PARA_SEGUIR == 0) LISTO_PARA_SEGUIR--;
         	//Obtiene estadísticas de la Población Global del Coordinador
         	statisticsMaster(glopop, coord_ind_global, workers, coord_ind_a_rec);
         	//Incrementa el contador que identifica en qué número de migración se encuentra el Coordinador
         	n_migracion++; 
         	//***************************************************************************************
         	//ENVIA MEJORES INDIVUDOS O SELECCIONADOS AL AZAR A CADA AG ANTES DE QUE SIGAN PROCESANDO
         	//***************************************************************************************
         	pop_a_enviar = selecciona_individuos_a_enviar(glopop, coord_ind_global, coord_ind_a_env);
         	//A cada AG le envía la población pop_a_enviar
         	for(nodox = 1; nodox < numtasks; nodox++){  
            	if (workers_actuales[nodox] == true) {
               		//Enviando mensaje Datos del Coordinador al nodox 
               		time_send = MPI_Wtime();
               		MPI_Isend(pop_a_enviar, 1, message_type_send, nodox, MSJ_DATOS_COORDINADOR, MPI_COMM_WORLD, &request);
               		for(;;){
                  		MPI_Test(&request, &flag2, &status);
                  		if(flag2 == true){
                     		time_send = MPI_Wtime() - time_send;
                     		time_comm += time_send;
                     		#ifdef _PRINT_MIGRACION_
                        		printf("Envió desde Rank = 0 a Rank = %d Mensaje con Población seleccionada desde Coordinador...\n", nodox);
                     		#endif
                     		break;
                  		}//End if
               		}//End for
            	}//End if
         	}//End for
         	//Si el fitness del mejor individuo de la pop global = 0  => envía mensaje a rank's para terminar
         	if (bestfit.fitness == 0.0){
				//Envía mensaje MSJ_TERMINE_EVOLUCION
				envia_mensaje(MSJ_TERMINE_EVOLUCION, "MSJ_TERMINE_EVOLUCION ya que se encontró bestfit.fitness = 0");
            	//Envía Mensaje a Cada AG para que siga con la evolución y pueda finalizar evolución...
				envia_mensaje(MSJ_SIGA, "MSJ_SIGA (para poder finalizar evolución)");            	
            	//Esta variable la setea en -1 para que no se envíe de nuevo el Mensaje MSJ_SIGA, 
            	//debido a que se enviaba 2 veces el MSJ_SIGA si estoy en la última generación y se obtiene fitness = 0.
            	//Con LISTO_PARA_SEGUIR_ULTIMA_VEZ = -1 MSJ_SIGA se enviará sólo una vez.
            	LISTO_PARA_SEGUIR_ULTIMA_VEZ = -1;
         	}//End if
         	else {
            	//Ahora le envía Mensaje a Cada AG que no ha llegado a la última 
            	//generación para que siga con la evolución...
            	for(nodox = 1; nodox < numtasks; nodox++){  
               		if (workers_actuales[nodox] == true) {
                  		//Enviando mensaje a nodox => OK SIGA
                  		time_send = MPI_Wtime();
                  		MPI_Isend(0, 0, MPI_INT, nodox, MSJ_SIGA, MPI_COMM_WORLD, &request);
                  		for(;;){
                     		MPI_Test(&request, &flag2, &status);
                     		if(flag2 == true){
                        		time_send = MPI_Wtime() - time_send;
                        		time_comm += time_send;
                        		#ifdef _PRINT_MIGRACION_
                           			printf("Envió desde Rank = 0 a Rank = %d Mensaje SIGA...\n", nodox);
		                        #endif
        		                break;
                     		}//End if
                  		}//End for
               		}//End if
            	}//End for
         	}//End else
         	//Setea variable para que pueda recibir despues una sola vez este mensaje
         	LISTO_PARA_MIGRAR_TODOS_LOS_RANK = 0;
      	}//End if
      	if(LISTO_PARA_MIGRAR_TODOS_LOS_RANK == 1){
         	LISTO_PARA_MIGRAR_TODOS_LOS_RANK = -1 ; //Valor negativo para que sea seteada cuando corresponda
         	for(nodox = 1; nodox < numtasks; nodox++){  
            	//Enviando mensaje a nodox  => MIGREN TODOS LOS AG ACTIVOS AHORA
            	if (workers_actuales[nodox] == true) {
               		time_send = MPI_Wtime();
               		MPI_Isend(0, 0, MPI_INT, nodox, MSJ_ENVIE_INDIVIDUOS, MPI_COMM_WORLD, &request);
               		for(;;){
                  		MPI_Test(&request, &flag2, &status);
                  		if(flag2 == true){
                     		time_send = MPI_Wtime() - time_send;
                     		time_comm += time_send;
                     		#ifdef _PRINT_MIGRACION_
                        		printf("Envió desde Rank = 0 a Rank = %d Mensaje MSJ_ENVIE_INDIVIDUOS...\n", nodox);
                     		#endif
                     		break;
                  		}//End if
               		}//End for
            	}//End for
         	}//End for
      	}//End if
      	if(LISTO_PARA_SEGUIR_ULTIMA_VEZ == 0) {
         	LISTO_PARA_SEGUIR_ULTIMA_VEZ --; //Se decrementa para que no intervenga más esta variable
         	//Obtiene estadísticas de la Población Global del Coordinador
         	statisticsMaster(glopop, coord_ind_global, workers, coord_ind_a_rec);
         	//Incrementa el contador que identifica en qué número de migración se encuentra el Coordinador
         	n_migracion++; 
         	//Ahora le envía Mensaje a Cada AG de modo que siga con la evolución...
			//Envía mensaje MSJ_SIGA para finalizar evolución
			envia_mensaje(MSJ_SIGA, "MSJ_SIGA para finalizar evolución");
      	}//End if
      	if(LISTO_PARA_TERMINAR_EVOLUCION == 0) {
         	LISTO_PARA_TERMINAR_EVOLUCION --; //Se decrementa para que no intervenga más esta variable
			//Envía mensaje MSJ_TERMINE_EVOLUCION
			envia_mensaje(MSJ_TERMINE_EVOLUCION, "MSJ_TERMINE_EVOLUCION");
      	}//End if
      	if(LISTO_PARA_TERMINAR == 0) {
         	//Inicializa de nuevo las variables para poder procesar un nuevo archivo, si corresponde.
         	LISTO_PARA_TERMINAR  = numtasks - 1;
         	LISTO_PARA_COMENZAR  = numtasks - 1;
         	LISTO_PARA_SEGUIR    = numtasks - 1;
         	LISTO_PARA_SEGUIR_ULTIMA_VEZ = numtasks - 1;
         	//Envía confirmación de término a cada nodox
			//Envía mensaje MSJ_TERMINO_CONFIRMADO
			envia_mensaje(MSJ_TERMINO_CONFIRMADO, "MSJ_TERMINO_CONFIRMADO");
         	break; //Finaliza nodo0 para archivo actual
      	}//End if
   	}//End for infinito
}//End comunicacion_asincrona_con_cada_rank

void comunicacion_sincrona_con_coordinador(void) 
//Realiza comunicación sincrona con Coordiandor
{
   	//Si debe enviar datos => los envía
   	if((((gen % tasa_migracion) == 0) && (gen > 0)) || (gen == maxgen - 1)) {  
     	//Obtiene los Y individuos a enviar (selecciona los mejores o al azar)
     	pop_a_enviar = obtiene_individuos_a_enviar(oldpop, newpop);
     	time_send = MPI_Wtime();
     	if (gen == maxgen - 1)
        	//Quiere decir que nodox llegó a la última generación => envía por última vez los mejores ind.
        	MPI_Isend(pop_a_enviar, 1, message_type_send, nodo0, MSJ_DATOS_ULTIMA_VEZ, MPI_COMM_WORLD, &request);
     	else            
        	MPI_Isend(pop_a_enviar, 1, message_type_send, nodo0, MSJ_DATOS, MPI_COMM_WORLD, &request);
     	for(;;){
         	MPI_Test(&request, &flag2, &status);
         	if(flag2 == true){
            	time_send = MPI_Wtime() - time_send;
            	time_comm += time_send;
            	#ifdef _PRINT_MIGRACION_
               		if (gen == maxgen - 1)
                  		//Quiere decir que nodox llegó a la última generación => envía por última vez los mejores ind.
                  		printf("Envió a Rank = 0 desde Rank = %d Mensaje de Datos Ultima Vez, gen = %d, queda a la espera de Mensaje SIGA...\n", rank, gen);
               		else
                  		printf("Envió a Rank = 0 desde Rank = %d Mensaje de Datos, gen = %d, queda a la espera de Mensaje con Población a Insertar...\n", rank, gen);
            	#endif
            	//Espera hasta recibir el MSJ_DATOS_COORDINADOR y MSJ_SIGA desde nodo0
            	for(;;){
               		MPI_Iprobe(nodo0, MPI_ANY_TAG, MPI_COMM_WORLD, &flag1, &status);
               		if(flag1 == true){
                  		if (status.MPI_TAG == MSJ_SIGA) {
                     		MPI_Recv(0, 0, MPI_INT, nodo0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                     		flag1 = 0;
                     		#ifdef _PRINT_MIGRACION_
                        		printf("Rank = %d Recibió desde Rank = 0 Mensaje SIGA, sigue evolución...\n", rank);
                     		#endif
                     		break;
                  		}//End if
                  		else if (status.MPI_TAG == MSJ_DATOS_COORDINADOR) {
                     		//Recibe los individuos desde Coordinador y los inserta en Población Local del AG
                     		MPI_Recv(recpop, 1, message_type_receive, nodo0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                     		flag1=0;
                     		//Inserta individuos recibidos en población actual (newpop) del AG
                     		inserta_individuos_en_poblacion(recpop, newpop, oldpop);
                     		#ifdef _PRINT_MIGRACION_
                        		printf("Rank = %d Recibió desde Rank = 0 Mensaje con Individuos a Insertar y los insertó...\n", rank);
                     		#endif
                  		}//End else
                  		else if (status.MPI_TAG == MSJ_TERMINE_EVOLUCION) {
                     		MPI_Recv(0, 0, MPI_INT, nodo0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                     		flag1 = 0;
                     		#ifdef _PRINT_MIGRACION_
                        		printf("Rank = %d Recibió desde Rank = 0 Mensaje TERMINE_EVOLUCION...\n", rank);
                     		#endif
                     		gen = maxgen;
                  		}//End else if
               		}//End if
            	}//End for
            	//Incrementa contador de nro. migración actual
            	n_migracion++;
            	break;
         	}//End if
      	}//End for
   	}//End if
}//End comunicacion_sincrona_con_coordinador

void comunicacion_sincrona_con_cada_rank(void)
//Establece comunicación síncrona con cada rank (AG)
{
   	//Esta variable debe usarse sólo si se desea terminar la evolución antes de maxgen
   	//LISTO_PARA_TERMINAR_EVOLUCION = 3 * (numtasks - 1); //Valor 3 es un parámetro cualquiera
   	for(;;){
      	MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag1, &status);
      	if(flag1 == true){
         	nodox = status.MPI_SOURCE;
         	if ((status.MPI_TAG == MSJ_DATOS) || (status.MPI_TAG == MSJ_DATOS_ULTIMA_VEZ)){
            	MPI_Recv(recpop, 1, message_type_receive, nodox, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            	flag1=0;
            	if (status.MPI_TAG == MSJ_DATOS)
               		LISTO_PARA_SEGUIR --;
            	else
               		LISTO_PARA_SEGUIR_ULTIMA_VEZ --;
            	//*****************************************************
            	//ALMACENA LA POBLACION RECIBIDA EN LA POBLACION GLOBAL
            	//*****************************************************
            	almacena_ind_recibidos_en_pop_global(recpop, coord_ind_a_rec, nodox);
            	//Esta variable debe usarse sólo si se desea terminar la evolución antes de maxgen
            	//LISTO_PARA_TERMINAR_EVOLUCION --;
         	} else if (status.MPI_TAG == MSJ_LISTO_PARA_COMENZAR){
            	MPI_Recv(0, 0, MPI_INT, nodox, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            	flag1 = 0;
            	#ifdef _PRINT_MIGRACION_
               		printf("Rank = 0 Recibió desde Rank = %d Mensaje Listo para Comenzar...\n", nodox);
            	#endif
            	LISTO_PARA_COMENZAR --;
         	} else if (status.MPI_TAG == MSJ_TERMINO){
            	MPI_Recv(&time_comm_rank, 1, MPI_DOUBLE, nodox, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	            time_comm += time_comm_rank;
    	        flag1 = 0;
        	    #ifdef _PRINT_MIGRACION_
            	   printf("Rank = 0 Recibió desde Rank = %d Mensaje de Término (Tiempo Comm = %f), Rank = 0 confirmará termino...\n", nodox, time_comm_rank);
            	#endif
            	LISTO_PARA_TERMINAR --;
         	}//End else if
      	}//End if
      	if(LISTO_PARA_COMENZAR == 0){
         	LISTO_PARA_COMENZAR --; //Decrementa listo_para_comenzar quedando en un valor negativo => no mas envío de COMIENCE
			//Envía mensaje MSJ_COMIENCE
			envia_mensaje(MSJ_COMIENCE, "MSJ_COMIENCE");
      	}//End if
      	if(LISTO_PARA_SEGUIR == 0) {
        	LISTO_PARA_SEGUIR = numtasks - 1; //Variable queda lista para recibir mensajes de nuevo de los ranks
         	//Obtiene estadísticas de la Población Global del Coordinador
         	statisticsMaster(glopop, coord_ind_global, workers, coord_ind_a_rec);
         	//Incrementa el contador que identifica en qué número de migración se encuentra el Coordinador
         	n_migracion++; 
         	//***************************************************************************************
         	//ENVIA MEJORES INDIVUDOS O SELECCIONADOS AL AZAR A CADA AG ANTES DE QUE SIGAN PROCESANDO
         	//***************************************************************************************
         	pop_a_enviar = selecciona_individuos_a_enviar(glopop, coord_ind_global, coord_ind_a_env);
         	//A cada AG le envía la población pop_a_enviar
         	for(nodox = 1; nodox < numtasks; nodox++){  
            	//Enviando mensaje a nodox 
            	time_send =MPI_Wtime();
            	MPI_Isend(pop_a_enviar, 1, message_type_send, nodox, MSJ_DATOS_COORDINADOR, MPI_COMM_WORLD, &request);
            	for(;;){
               		MPI_Test(&request, &flag2, &status);
               		if(flag2 == true){
                  		time_send = MPI_Wtime() - time_send;
                  		time_comm += time_send;
                  		#ifdef _PRINT_MIGRACION_
                     		printf("Envió desde Rank = 0 a Rank = %d Mensaje con Población seleccionada desde Coordinador...\n", nodox);
                  		#endif
                  		break;
               		}//End if
            	}//End for
         	}//End for
         	//Si el fitness del mejor individuo de la pop global = 0  => envía mensaje a rank's para terminar
         	if (bestfit.fitness == 0.0){
				//Envía mensaje MSJ_TERMINE_EVOLUCION
				envia_mensaje(MSJ_TERMINE_EVOLUCION, "MSJ_TERMINE_EVOLUCION ya que se encontró bestfit.fitness = 0");
            	//Envía Mensaje a Cada AG para que siga con la evolución y pueda finalizar evolución...
				//Envía mensaje MSJ_SIGA
				envia_mensaje(MSJ_SIGA, "MSJ_SIGA (para poder finalizar evolución)");
            	//Esta variable la setea en -1 para que no se envíe de nuevo el Mensaje MSJ_SIGA, 
            	//debido a que se enviaba 2 veces el MSJ_SIGA si estoy en la última generación y se obtiene fitness = 0.
            	//Con LISTO_PARA_SEGUIR_ULTIMA_VEZ = -1 MSJ_SIGA se enviará sólo una vez.
            	LISTO_PARA_SEGUIR_ULTIMA_VEZ = -1;
         	}//End if
         	else {
            	//Ahora le envía Mensaje a Cada AG que no ha llegado a la última 
	            //generación para que siga con la evolución...
				//Envía mensaje MSJ_SIGA
				envia_mensaje(MSJ_SIGA, "MSJ_SIGA");
         	}//End else
      	}//End if
      	if(LISTO_PARA_SEGUIR_ULTIMA_VEZ == 0) {
        	LISTO_PARA_SEGUIR_ULTIMA_VEZ --; //Se decrementa para que no intervenga más esta variable
         	//Obtiene estadísticas de la Población Global del Coordinador
         	statisticsMaster(glopop, coord_ind_global, workers, coord_ind_a_rec);
        	//Incrementa el contador que identifica en qué número de migración se encuentra el Coordinador
         	n_migracion++; 
         	//Ahora le envía Mensaje a Cada AG de modo que siga con la evolución...
			//Envía mensaje MSJ_SIGA
			envia_mensaje(MSJ_SIGA, "MSJ_SIGA para finalizar evolución");
      	}//End if
      	if(LISTO_PARA_TERMINAR_EVOLUCION == 0) {
         	LISTO_PARA_TERMINAR_EVOLUCION --; //Se decrementa para que no intervenga más esta variable
			//Envía mensaje MSJ_TERMINE_EVOLUCION
			envia_mensaje(MSJ_TERMINE_EVOLUCION, "MSJ_TERMINE_EVOLUCION");
      	}//End if
      	if(LISTO_PARA_TERMINAR == 0) {
         	//Inicializa de nuevo las variables para poder procesar un nuevo archivo, si corresponde.
         	LISTO_PARA_TERMINAR  = numtasks - 1;
         	LISTO_PARA_COMENZAR  = numtasks - 1;
         	LISTO_PARA_SEGUIR    = numtasks - 1;
         	LISTO_PARA_SEGUIR_ULTIMA_VEZ = numtasks - 1;
         	//Envía confirmación de término a cada nodox
			//Envía mensaje MSJ_TERMINO_CONFIRMADO
			envia_mensaje(MSJ_TERMINO_CONFIRMADO, "MSJ_TERMINO_CONFIRMADO");
         	break; //Finaliza nodo0 para archivo actual
      	}//End if
   	}//End for infinito
}//End comunicacion_sincrona_con_cada_rank

void Build_type_pop(struct individual *pop, MPI_Datatype *message_type_pop, int size_pop)
//Define estructura de datos para el Tipo MPI
{
	#define CANT_VARIABLES_MPI	16 

   	int block_lengths[(CANT_VARIABLES_MPI * size_pop)],i;
   	MPI_Aint  displacements[(CANT_VARIABLES_MPI * size_pop)];
   	MPI_Aint  addresses[(CANT_VARIABLES_MPI * size_pop) + 1];
   	MPI_Datatype typelist[(CANT_VARIABLES_MPI * size_pop)];
   
   	for(i=0; i<size_pop; i++){
      	typelist[(CANT_VARIABLES_MPI * i) + 0] 		 = MPI_UNSIGNED;
      	typelist[(CANT_VARIABLES_MPI * i) + 1] 		 = MPI_FLOAT;
   
      	typelist[(CANT_VARIABLES_MPI * i) + 2] 		 = MPI_FLOAT;
      	typelist[(CANT_VARIABLES_MPI * i) + 3] 		 = MPI_FLOAT;
      	typelist[(CANT_VARIABLES_MPI * i) + 4] 		 = MPI_FLOAT;   
      	typelist[(CANT_VARIABLES_MPI * i) + 5] 		 = MPI_INT;
      	typelist[(CANT_VARIABLES_MPI * i) + 6] 		 = MPI_FLOAT;
      	typelist[(CANT_VARIABLES_MPI * i) + 7] 		 = MPI_FLOAT;
      	typelist[(CANT_VARIABLES_MPI * i) + 8] 		 = MPI_FLOAT;
      	typelist[(CANT_VARIABLES_MPI * i) + 9] 		 = MPI_INT;
      	typelist[(CANT_VARIABLES_MPI * i) + 10] 	 = MPI_DOUBLE;
   
      	typelist[(CANT_VARIABLES_MPI * i) + 11] 	 = MPI_INT;
      	typelist[(CANT_VARIABLES_MPI * i) + 12] 	 = MPI_INT;
      	typelist[(CANT_VARIABLES_MPI * i) + 13] 	 = MPI_INT;
      	typelist[(CANT_VARIABLES_MPI * i) + 14]      = MPI_UNSIGNED;
      	typelist[(CANT_VARIABLES_MPI * i) + 15]      = MPI_UNSIGNED_SHORT;
   
      	block_lengths[(CANT_VARIABLES_MPI * i) + 0]  = chromsize;
      	block_lengths[(CANT_VARIABLES_MPI * i) + 1]  = 1;
   
      	block_lengths[(CANT_VARIABLES_MPI * i) + 2]  = 1;
     	block_lengths[(CANT_VARIABLES_MPI * i) + 3]  = 1;
      	block_lengths[(CANT_VARIABLES_MPI * i) + 4]  = 1;
     	block_lengths[(CANT_VARIABLES_MPI * i) + 5]  = 1;
      	block_lengths[(CANT_VARIABLES_MPI * i) + 6]  = 1;
      	block_lengths[(CANT_VARIABLES_MPI * i) + 7]  = 1;
      	block_lengths[(CANT_VARIABLES_MPI * i) + 8]  = 1;
      	block_lengths[(CANT_VARIABLES_MPI * i) + 9]  = 1;
      	block_lengths[(CANT_VARIABLES_MPI * i) + 10] = 1;
   
      	block_lengths[(CANT_VARIABLES_MPI * i) + 11] = 1;
      	block_lengths[(CANT_VARIABLES_MPI * i) + 12] = 1;
      	block_lengths[(CANT_VARIABLES_MPI * i) + 13] = 2;
      	block_lengths[(CANT_VARIABLES_MPI * i) + 14] = chmutsize;
      	block_lengths[(CANT_VARIABLES_MPI * i) + 15] = chlistasize;
   	}//End for
   
   	MPI_Address(pop, &addresses[0]);
   	for(i=0; i<size_pop; i++){
      	MPI_Address(pop[i].chrom, &addresses[(CANT_VARIABLES_MPI * i) + 1]);
      	MPI_Address(&(pop[i].fitness), &addresses[(CANT_VARIABLES_MPI * i) + 2]);
   
     	MPI_Address(&(pop[i].PEval.perdida), &addresses[(CANT_VARIABLES_MPI * i) + 3]);
      	MPI_Address(&(pop[i].PEval.areaocupada), &addresses[(CANT_VARIABLES_MPI * i) + 4]);
      	MPI_Address(&(pop[i].PEval.calidad), &addresses[(CANT_VARIABLES_MPI * i) + 5]);
      	MPI_Address(&(pop[i].PEval.piezas), &addresses[(CANT_VARIABLES_MPI * i) + 6]);
      	MPI_Address(&(pop[i].PEval.c_distancia), &addresses[(CANT_VARIABLES_MPI * i) + 7]);
      	MPI_Address(&(pop[i].PEval.c_perdidareal), &addresses[(CANT_VARIABLES_MPI * i) + 8]);
      	MPI_Address(&(pop[i].PEval.c_digregacion), &addresses[(CANT_VARIABLES_MPI * i) + 9]);
      	MPI_Address(&(pop[i].PEval.n_perdidas), &addresses[(CANT_VARIABLES_MPI * i) + 10]);
      	MPI_Address(&(pop[i].PEval.var1), &addresses[(CANT_VARIABLES_MPI * i) + 11]);
   
      	MPI_Address(&(pop[i].xsite), &addresses[(CANT_VARIABLES_MPI * i) + 12]);
      	MPI_Address(&(pop[i].msite), &addresses[(CANT_VARIABLES_MPI * i) + 13]);
      	MPI_Address(&(pop[i].parent), &addresses[(CANT_VARIABLES_MPI * i) + 14]);
      	MPI_Address(pop[i].chmut, &addresses[(CANT_VARIABLES_MPI * i) + 15]);
      	MPI_Address(pop[i].pusListaPiezas, &addresses[(CANT_VARIABLES_MPI * i) + 16]);
   	}//End for
      
   	// compute the relativ displacement between the first data's
   	// of each struct member 
   	displacements[0] = addresses[1]-addresses[0];
   	for(i=1; i<(CANT_VARIABLES_MPI * size_pop); i++)
     	displacements[i] = displacements[i-1] + (addresses[i+1] - addresses[i]);
   
   	// Now define structured type and commit it 
   	MPI_Type_struct((CANT_VARIABLES_MPI * size_pop), block_lengths, displacements, typelist, message_type_pop);
   	MPI_Type_commit(message_type_pop);
}//End Build_type_pop

int lee_char_y_genera_achivo_instancia_tmp(int tipo_problema, int cant_char_a_rec){
   	char *chars;
   	int  estado_arch;
   	char nombre_archivo[100];
  
   	estado_arch = true;
   	
   	//Dimensiona estructura chars con caracteres a recibir desde rank0
   	if((chars = (char*) malloc(cant_char_a_rec*sizeof(char))) == NULL)
      	nomemory("chars en lee_char_y_genera_achivo_instancia_tmp");
#ifdef _DEBUG_MALLOC_
	printf("Malloc, comunica.c, 612, chars, %d\n", (cant_char_a_rec)*sizeof(char));
#endif
   	
   	//Espera que nodo0 entregue char con datos de archivo instancia
   	for(;;){
      	MPI_Iprobe(nodo0, MPI_ANY_TAG, MPI_COMM_WORLD, &flag1, &status);
      	if(flag1 == true){
         	if (status.MPI_TAG == MSJ_CHAR_ARCHIVO_INSTANCIA){
            	MPI_Recv(chars, cant_char_a_rec, MPI_CHAR, nodo0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            	flag1 = 0;
            	#ifdef _PRINT_MIGRACION_
               		printf("Rank = %d Recibió (%d caracteres) desde Rank = 0 del archivo instancia...\n", rank, cant_char_a_rec);
            	#endif
            	break;
         	}//End if
         	else if (status.MPI_TAG == MSJ_ERROR_ARCHIVO_INSTANCIA) {
            	MPI_Recv(0, 0, MPI_INT, nodo0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            	flag1 = 0;
            	estado_arch = false;
            	#ifdef _PRINT_MIGRACION_
               		printf("Rank = %d Recibió desde Rank = 0 Mensaje MSJ_ERROR_ARCHIVO_INSTANCIA...\n", rank);
            	#endif
            	break;
         	}//End else if
      	}//End if
   	}//End for
  
   	//Si archivo no tiene problemas => continua
   	if (estado_arch == true) {
	   	//Genera a partir de los chars archivo instancia temporal que leerá cada Rank
	   	sprintf(nombre_archivo, "%sn%d-%s", ruta_resultados, rank, "tmp.txt");
	   
	   	if(genera_archivo_temporal(nombre_archivo, chars, cant_char_a_rec)){
	   		// Archivo temporal generado
		  	if(app_leearchivo(tipo_problema, nombre_archivo, rank)) 
		    	//No hay error al leer archivo instancia temporal
		      	estado_arch = true;
		   	else
		      	//hay error al leer archivo instancia temporal
		      	estado_arch = false;
	   	}//End if
	   	else
	    	//hay error al generar archivo instancia temporal
	      	estado_arch = false;
   	}//End if
      	
	//Libera chars
	free(chars);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, comunica.c, 650, chars\n"); 
#endif
   	
   	if (estado_arch == true)
		return -1;
	else
		return 0;
		
}//End lee_enteros_y_genera_achivo_instancia_tmp

void inicializa_mpi(int argc,char *argv[])
//Rutina que inicializa variables MPI
{
	//Rutinas para MPI
   	MPI_Init(&argc,&argv);
   	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
   	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   	nodo0 = 0;
   
   	//Definición de variables globales relacionadas con el paso de mensaje.
   	workers = numtasks - 1;
   	LISTO_PARA_TERMINAR  = numtasks - 1;
   	LISTO_PARA_COMENZAR  = numtasks - 1;
   	LISTO_PARA_SEGUIR    = numtasks - 1;
   	LISTO_PARA_SEGUIR_ULTIMA_VEZ = numtasks - 1;
   	LISTO_PARA_TERMINAR_EVOLUCION = -1;
}//End inicaliza_mpi

void envia_a_cada_rank(int mensaje)
//Rutina que envía datos a cada rank > 0
{
	switch(mensaje) {
	case MSJ_RUNMAX:
		//Envía runmax a rank>0
    	for(nodox = 1; nodox < numtasks; nodox++){  
         	//Enviando runmax a nodox 
         	MPI_Isend(&runmax, 1, MPI_INT, nodox, MSJ_RUNMAX, MPI_COMM_WORLD, &request);
         	for(;;){
	            MPI_Test(&request, &flag2, &status);
            	if(flag2 == true){
               		#ifdef _PRINT_MIGRACION_
                  		printf("Envió desde Rank = 0 a Rank = %d Mensaje MSJ_RUNMAX...\n", nodox);
               		#endif
               		break;
            	}//End if
	       	}//End for
      	}//End for
		break;
	case MSJ_LINEA_IN:
		//Envía linea_in a cada Rank
     	for(nodox = 1; nodox < numtasks; nodox++){  
        	//Enviando linea_in a nodox 
        	time_send = MPI_Wtime();
        	MPI_Isend(linea_in, 100, MPI_CHAR, nodox, MSJ_LINEA_IN, MPI_COMM_WORLD, &request);
        	for(;;){
           		MPI_Test(&request, &flag2, &status);
           		if(flag2 == true){
              		time_send = MPI_Wtime() - time_send;
              		time_comm += time_send;
              		#ifdef _PRINT_MIGRACION_
                 		printf("Envió desde Rank = 0 a Rank = %d Mensaje MSJ_LINEA_IN...\n", nodox);
              		#endif
              		break;
           		}//End if
        	}//End for
     	}//End for		
		break;
	case MSJ_CANT_CHAR_A_REC: 
		//Envía a cada Rank cantidad de enteros a recibir del problema
    	for(nodox = 1; nodox < numtasks; nodox++){  
       		//Enviando cantidad de enteros a nodox 
       		time_send = MPI_Wtime();
	       		MPI_Isend(&cantidad_char_a_recibir, 1, MPI_INT, nodox, MSJ_CANT_CHAR_A_REC, MPI_COMM_WORLD, &request);
       		for(;;){
          		MPI_Test(&request, &flag2, &status);
          		if(flag2 == true){
             		time_send = MPI_Wtime() - time_send;
             		time_comm += time_send;
             		#ifdef _PRINT_MIGRACION_
                		printf("Envió desde Rank = 0 a Rank = %d Mensaje MSJ_CANT_CHAR_A_REC...\n", nodox);
             		#endif
             		break;
          		}//End if
       		}//End for
    	}//End for
		break;
	case MSJ_CHAR_ARCHIVO_INSTANCIA:
	   	//Envía a cada Rank char con archivo instancia
	   	for(nodox = 1; nodox < numtasks; nodox++){  
	      	//Enviando enteros a nodox 
	     	time_send = MPI_Wtime();
	      	MPI_Isend(char_instancia_problema, cantidad_char_que_determinan_instancia_del_problema, MPI_CHAR, nodox, MSJ_CHAR_ARCHIVO_INSTANCIA, MPI_COMM_WORLD, &request);
	      	for(;;){
	         	MPI_Test(&request, &flag2, &status);
	         	if(flag2 == true){
	            	time_send = MPI_Wtime() - time_send;
	            	time_comm += time_send;
	            	#ifdef _PRINT_MIGRACION_
	               		printf("Envía desde Rank = 0 a Rank = %d MSJ_CHAR_ARCHIVO_INSTANCIA con valores char's del archivo instancia a procesar...\n", nodox);
	            	#endif
	           		break;
	         	}//End if
	      	}//End for
	   	}//End for
		break;
	case MSJ_ERROR_ARCHIVO_INSTANCIA:
		//Envía a cada Rank que hay problemas para leer archivo instancia
		//Envía mensaje MSJ_ERROR_ARCHIVO_INSTANCIA
		envia_mensaje(MSJ_ERROR_ARCHIVO_INSTANCIA, "MSJ_ERROR_ARCHIVO_INSTANCIA");
		break;	
	}//End switch
}//End envia_a_cada_rank

