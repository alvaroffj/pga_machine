/*----------------------------------------------------------------------------*/
/* app2_ng.c - rutinas para PCP NO Guillotina                                 */
/*----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "sga.h"
#include "var_pcp.h"

void app2_agregalistap_ng(int x, int y, int anc, int alt, char tipo, int t_pieza)
// Agrega un elemento al final de la lista de pérdidas, su parámetro
// inicial es un puntero apuntando al nodo final de la lista
{
   	TListaPE2 *LAux2;

   	if((LAux2 = (TListaPE2*) malloc(sizeof(TListaPE2))) == NULL)
      	nomemory("Laux2 en app2_agregalistap_ng");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, App2_ng.c, 35, LAux2, %d\n", sizeof(TListaPE2));
#endif

   	LAux2->ancho = anc;
   	LAux2->alto = alt;
   	LAux2->xini = x;
   	LAux2->yini = y;
   	LAux2->tipo = tipo; 	// 'E'= Pérdida Externa,'P'=Pérdida Interna, 'G'=Ganacia
   	LAux2->pieza = t_pieza; // t_pieza = 0 => Identifica pérdida,
   	LAux2->prox = LPer2;    // t_pieza > 0 => Id tipo de pieza con orientación normal de la pieza,
   	LPer2 = LAux2;	   		// t_pieza < 0 => Id tipo de pieza orientación invertida de la pieza.
   	TotalPie++;             // Incrementa el número de piezas (ganancias + pérdidas) totales.
}//End app2_agregalistap_ng

void app2_genera_layout_ng(int tipo_problema, char *nombrearchivo, cpu_consummed_time_t *t, TListaPE2 *Bus2, int run_actual)
//Función que genera el layout de salida
{
	char strtot[32768], straux[100];
	
	//Primero coloca parámetros importantes 
	fprintf(outfp,"32 %d %d %d\n", TotalPie, AnchoPl, AltoPl); //32 parmetros => 31 + 1 que indica Total de piezas + perdidas generadas... 
  	strtot[0] = 0;
    if(tipo_problema == 1) 
    	sprintf(straux,"TIPO_PROBLEMA                  : %s\n", "P.C.P No Guillotina");
    else 
    	sprintf(straux,"TIPO_PROBLEMA                  : %s\n", "NO DEFINIDO...");
    strcat(strtot, straux);
	sprintf(straux,"ARCHIVO_PROBLEMA               : %s\n", nombrearchivo);
	strcat(strtot, straux);
  	sprintf(straux,"CORRIDA EN ARCHIVO REPORTE     : %d\n", run_actual);
  	strcat(strtot, straux);
	sprintf(straux,"CANTIDAD_PIEZAS                : %d\n", NumPie);
	strcat(strtot, straux);
	sprintf(straux,"CANTIDAD_TIPOS_PIEZAS          : %d\n", cantidadtipospiezas);
	strcat(strtot, straux);
	sprintf(straux,"ANCHO_LAMINA (unidades)        : %d\n", AnchoPl);
	strcat(strtot, straux);
	sprintf(straux,"LARGO_LAMINA (unidades)        : %d\n", AltoPl);
	strcat(strtot, straux);
	sprintf(straux,"MAXIMO_GENERACIONES            : %d\n", maxgen);
	strcat(strtot, straux);
	sprintf(straux,"PESO_PERDIDA  (%%)              : %f\n", 100.0 * peso_perdida);
	strcat(strtot, straux);
	sprintf(straux,"PESO_DISTANCIA  (%%)            : %f\n", 100.0 * peso_distancia);
	strcat(strtot, straux);
	sprintf(straux,"PESO_DIGREGACION (%%)           : %f\n", 100.0 * peso_digregacion);
	strcat(strtot, straux);
	sprintf(straux,"TAMANO_POBLACION (ind.)        : %d\n", popsize);
	strcat(strtot, straux);
	sprintf(straux,"PROBABILIDAD_CRUZAMIENTO (%%)   : %f\n", 100.0 * pcross);
	strcat(strtot, straux);
	sprintf(straux,"PROBABILIDAD_MUTACION (%%)      : %f\n", 100.0 * pmutation);
	strcat(strtot, straux);
	sprintf(straux,"MS_CALIDAD (%%)                 : %f\n", 100.0 * bestfit.PEval.calidad);
	strcat(strtot, straux);
	sprintf(straux,"MS_FITNESS                     : %f\n", bestfit.fitness);
	strcat(strtot, straux);
	sprintf(straux,"MS_PERDIDA (%%)                 : %f\n", 100.0 * bestfit.PEval.c_perdidareal);
	strcat(strtot, straux);
	sprintf(straux,"MS_DISTANCIA (%%)               : %f\n", 100.0 * bestfit.PEval.c_distancia);
	strcat(strtot, straux);
	sprintf(straux,"MS_DIGREGACION (%%)             : %f\n", 100.0 * bestfit.PEval.c_digregacion);
	strcat(strtot, straux);
	sprintf(straux,"MS_AREA_OCUPADA (%%)            : %f\n", 100.0 * bestfit.PEval.areaocupada);
	strcat(strtot, straux);
	sprintf(straux,"MS_NUMERO_PIEZAS_COLOCADAS     : %d\n", bestfit.PEval.piezas);
	strcat(strtot, straux);
	sprintf(straux,"MS_NUMERO_PERDIDAS_GENERADAS   : %d\n", bestfit.PEval.n_perdidas);
	strcat(strtot, straux);
	sprintf(straux,"WORKER_MEJOR_INDIVIDUO         : %d\n", bestfit.generation);
	strcat(strtot, straux);
	sprintf(straux,"TIEMPO_CPU (Segundos)          : %f\n", t->cpu_time);
	strcat(strtot, straux);
	sprintf(straux,"TIEMPO_COMM (Segundos)         : %f\n", t->comm_time);
	strcat(strtot, straux);
	sprintf(straux,"TIEMPO_TOTAL (Segundos)        : %f\n", t->elapsed_time);
	strcat(strtot, straux);
  	sprintf(straux,"NUM_INDIVIDUOS_A_ENVIAR        : %d\n", n_ind_a_enviar);
  	strcat(strtot, straux);
  	sprintf(straux,"NUM_INDIVIDUOS_A_RECIBIR       : %d\n", n_ind_a_recibir);
  	strcat(strtot, straux);
  	sprintf(straux,"TASA_MIGRACION(EN GENERACIONES): %d\n", tasa_migracion);
  	strcat(strtot, straux);
  	if(modelo_migracion == 0)
     	sprintf(straux,"MODELO_MIGRACION               : ASINCRONO\n");
  	else
     	sprintf(straux,"MODELO_MIGRACION               : SINCRONO\n");
  	strcat(strtot, straux);
  	sprintf(straux,"CANTIDAD AG(CPU) CONSIDERADO(S): %d\n", workers);
  	strcat(strtot, straux);
	sprintf(straux,"TOTAL PIEZAS+PERDIDAS HECHAS   : %d\n", TotalPie);
	strcat(strtot, straux);
	fprintf(outfp,"%s@\n", strtot);

	//Finalmente detalla pieza a pieza
	fprintf(outfp,"  TIPO_PIEZA\t   POS_X_INI\t   POS_Y_INI\t   POS_X_FIN\t   POS_Y_FIN\tESTADO_PIEZA\tAREA_OCUPADA\t\n");
	while(Bus2 != NULL) {
   		fprintf(outfp,"%12d\t%12d\t%12d\t%12d\t%12d\t%12c\t%12d\t\n",
	   	Bus2->pieza, Bus2->xini, Bus2->yini, Bus2->xini+Bus2->ancho, Bus2->yini+Bus2->alto,
		Bus2->tipo, Bus2->ancho * Bus2->alto);
  		Bus2 = Bus2->prox;
 	}//End while	
 	fprintf(outfp,"\n");
}//End app2_genera_layout_ng

void app2_funceval_ng(int tipo_problema, char *nomarch, TNodoAP *piezas, int run_actual)
//función evaluación para desplegar al mejor individuo
{
   	int         i, siguiente=1;
   	int         Acan = 0, Acal = 0, Acpi = 0, Nuan, Nual, Nupi;
   	int         PieInc = NumPie;
   	TListaPE2   *Bus2;
   	int 		Arreglo[AnchoPl], Arr1, Arr2, Arreglo_donde_empieza, Arreglo_valor;
   	int 		Patron_sirve, Arreglo_min_altura;
   
   	// Inicializa la Lista de Pérdidas o Ganacias
   	LPer2 = NULL;
   	TotalPie = 0;

   	// Inicializa Arreglo que contiene tantos registros como ancho tenga la placa
   	for(i=0;i<AnchoPl;i++){
      	Arreglo[i]=0;
   	}//End for

   	//Me estoy saltando las piezas de ancho y largo = 0
   	for(i=0;i<NumPie;i++){
      	Acan = piezas[i].ancho;
      	Acal = piezas[i].alto;
      	Acpi = piezas[i].numero;
      	if((Acan != 0) && (Acal != 0) && (Acan <= AnchoPl) && (Acal <= AltoPl)) {
         	app2_agregalistap_ng(0, 0, Acan, Acal, 'G', Acpi);
		 	//Coloca la pieza abajo a la izquierda
		 	for(Arr1=0;Arr1<Acan;Arr1++){
	     		Arreglo[Arr1]=Acal;
 	   	 	}//End for
         	siguiente = i+1;
         	i = NumPie;
      	}//End if
      	else
         	PieInc--;
   	}//End for

   	if(siguiente<NumPie){
      	for(i=siguiente;i<NumPie;i++) {
         	Nuan = piezas[i].ancho;
         	Nual = piezas[i].alto;
         	Nupi = piezas[i].numero;
         	if((Nuan != 0) && (Nual != 0) && (Nuan <= AnchoPl) && (Nual <= AltoPl)){
				Arreglo_donde_empieza=-1; 
		    	Arreglo_min_altura=AltoPl;
		    	Arreglo_valor=0;
		    	for(Arr1=0;Arr1<AnchoPl;Arr1++){
		    		if ((Arr1+Nuan) <= AnchoPl) {
						Arreglo_valor=Arreglo[Arr1];
						if (Arreglo_valor+Nual <= AltoPl) { 
							Patron_sirve=-1; //-1 => Sì sirve por default
				    		for(Arr2=0;Arr2<Nuan;Arr2++){
				    			if (Arreglo[Arr1+Arr2] != Arreglo_valor) {
				    				Patron_sirve=0;
				    				Arr2= Nuan;
				    			}//End if
				    		}//End for
				    		if (Patron_sirve == -1) {
								if (Arreglo_valor+Nual < Arreglo_min_altura) {
									Arreglo_donde_empieza = Arr1;
									Arreglo_min_altura = Arreglo_valor+Nual;
								}//End if
								else if ((Arreglo_valor+Nual == Arreglo_min_altura) && (Arreglo_donde_empieza == -1)) {
									Arreglo_donde_empieza = Arr1;
								}//End else
				    		}//End if  
						}//End if
		    		}//End if
		   		}//End for
		    	if (Arreglo_donde_empieza == -1) {
		    		//Pieza no sirve
			   		PieInc--; // Pieza no sirve.
		    	}
		    	else {
		    		//Pieza sirve
		   			// Agrega la nueva pieza debajo del patrón y a la izquierda
                	app2_agregalistap_ng(Arreglo_donde_empieza, Arreglo[Arreglo_donde_empieza], Nuan, Nual, 'G', Nupi);
					for(Arr1=Arreglo_donde_empieza;Arr1<Arreglo_donde_empieza+Nuan;Arr1++){
		    			Arreglo[Arr1] = Arreglo[Arr1] + Nual; 
		   			}//End for
		    	}//End else            
         	}//End if
         	else PieInc--;         // Pieza excede el area disponible en la lámina.
      	}//End for
   	}//End if
   	Bus2 = LPer2;

	// LLENA archivo de layout para el problema particular
	app2_genera_layout_ng(tipo_problema, nomarch, &time_consumation, Bus2, run_actual);
}//End app2_funceval

void app2_objfunc_ng(int tipo_problema, char *nomarch, struct bestever *bestcritter, int run_actual)
// Función Objetivo, transforma el string cromosoma en un arreglo de piezas
{
   	unsigned mask=1, tp, tp1, bitpos;
   	int      j, k, stop;
   	TNodoAP  *piezaschromo; // variable que apunta a todas las piezas del cromosoma generado

   	if((piezaschromo = (TNodoAP*) malloc(NumPie*sizeof(TNodoAP))) == NULL)
      	nomemory("piezaschromo en app2_objfunc_ng");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, App2_ng.c, 365, piezaschromo, %d\n", NumPie*sizeof(TNodoAP));
#endif

   	//Asigna cada pieza del arreglo piezasproblema en arregla piezaschromo dependiendo del string chrom
   	for(k = 0; k < chromsize; k++) {
   		if(k == (chromsize-1))
      		stop = lchrom-(k*UINTSIZE);
   		else
      	stop = UINTSIZE;
   		tp  = bestcritter->chrom[k];
   		tp1 = bestcritter->chmut[k];
   		for(j = 0; j < stop; j++) {
      		bitpos = j + UINTSIZE*k;
         	// Entra al if si el bit actual es 1
      		if(tp&mask) {
            	//Asigna pieza desde arreglo piezasproblema
         		if(tp1&mask) {
               		// Efectúa rotación de piezas
            		piezaschromo[bitpos].ancho       = piezasproblema[bitpos].alto;
            		piezaschromo[bitpos].alto        = piezasproblema[bitpos].ancho;
            		piezaschromo[bitpos].numero      = -1 * piezasproblema[bitpos].numero;
         		}//End if
         		else {
               		// No efectua rotacion de piezas
            		piezaschromo[bitpos].ancho       = piezasproblema[bitpos].ancho;
            		piezaschromo[bitpos].alto        = piezasproblema[bitpos].alto;
            		piezaschromo[bitpos].numero      = piezasproblema[bitpos].numero;
         		}//End else
         		piezaschromo[bitpos].cantidadpiezas  = 1;
      		}//End if
      		else {
            	//Asigna solo ceros => pieza no se considera
         		piezaschromo[bitpos].ancho           = 0;
         		piezaschromo[bitpos].alto            = 0;
         		piezaschromo[bitpos].numero          = 0;
         		piezaschromo[bitpos].cantidadpiezas  = 1;
      		}//End else
      		tp = tp>>1;
      		tp1 = tp1>>1;
   		}//End for
   	}//End for
   	app2_funceval_ng(tipo_problema, nomarch, piezaschromo, run_actual);
   	free(piezaschromo);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, app2_ng.c, 417, piezaschromo\n");
#endif
}//End app2_objfunc_ng
