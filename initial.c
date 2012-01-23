/*----------------------------------------------------------------------------*/
/* initial.c - funciones de inicialización o de configuración inicial         */
/*----------------------------------------------------------------------------*/

#include <sys/times.h>

#include "sga.h"
#include "app.h"
#include "random.h"
#include "utility.h"

void initpop(int tipo_problema)
// Inicializa la población inicial en forma aleatoria
{
   	int j, j1, k, stop;
   	unsigned mask = 1, numero_gen;
 
   	//Define por default que cada elemento del cromosoma del Algoritmo Genértico es del tipo binario
	tipo_elemento_cromosoma = BINARIO;
	//*** NEW
	//if(tipo_problema == 2) tipo_elemento_cromosoma = ENTEROS; //Tipo_Problema = Corte de Piezas Irregulares con cromosoma con ENTEROS =: cada elemento del cromosoma es del tipo ENTEROS
   	//***
   	
  	if (tipo_elemento_cromosoma == BINARIO){
		//Para cromosomas binarios (0,1)
		for(j = 0; j < popsize; j++) {
			// Inicializa cromosoma de Lista
	        for(k = 0; k < chlistasize; k++) {
	            oldpop[j].pusListaPiezas[k] = 0;
	        }//End for
			//********* Esto solo para irregulares binarios (no implementado) ***********
			//numero_gen = rnd( 1, chlistasize ) - 1; //Determina al azar numero_gen entre 0 y largo_cromosoma-1
	        /* El siguiente for coloca en la primera posicion el numero_gen y luego incrementa el valor de numero_gen en 1 	*/
	        /* Dado un cromosoma de 10 gen desde el 0 al 9 y el numero_gen = 7 												*/ 
	        /* pos0 pos1 pos2 pos3 pos4 pos5 pos6 pos7 pos8 pos9                											*/
	        /* 7   8    9    0    1    2    3    4    5    6																*/
	        //for(k = 0; k < chlistasize; k++) {
	        //    oldpop[j].pusListaPiezas[ k ] = numero_gen;
			//	numero_gen++;
			//	numero_gen=(numero_gen==chlistasize)?0:numero_gen;
	        //}//End for
	        //***************************************************************************
			
			// Inicializa cromosoma de string
	      	for(k = 0; k < chromsize; k++) {
	         	oldpop[j].chrom[k] = 0;
	         	if(k == (chromsize-1))
	            	stop = lchrom - (k*UINTSIZE);
	         	else
	            	stop = UINTSIZE;
	         	for(j1 = 1; j1 <= stop; j1++) {
	            	oldpop[j].chrom[k] = oldpop[j].chrom[k]<<1;
	            	if(flip(0.5))
	               		oldpop[j].chrom[k] = oldpop[j].chrom[k]|mask;
	         	}//End for
	      	}//End for
			// Inicializa cromosoma de mutación
	      	for(k = 0; k < chmutsize; k++) {
	        	oldpop[j].chmut[k] = 0;
	         	if(k == (chmutsize-1))
	            	stop = lchmut - (k*UINTSIZE);
	         	else
	            	stop = UINTSIZE;
	         	for(j1 = 1; j1 <= stop; j1++) {
	            	oldpop[j].chmut[k] = oldpop[j].chmut[k]<<1;
	            	if(flip(0.5))
	               		oldpop[j].chmut[k] = oldpop[j].chmut[k]|mask;
	         	}//End for
			}//End for
	      	oldpop[j].parent[0]  = 0; // Initializa información de los padres
	      	oldpop[j].parent[1]  = 0;
	      	oldpop[j].xsite      = 0;
	      	oldpop[j].msite      = 0;
	   	  	app_objfunc(tipo_problema, &(oldpop[j]));  // Evalua fitness inicial
	   	}//End for
   	}//End if
	else if (tipo_elemento_cromosoma == ENTEROS){
		//Para cromosomas donde cada gen puede tener un valor >= 0
	    for(j = 0; j < popsize; j++) {
			numero_gen = rnd( 1, chlistasize ) - 1; //Determina al azar numero_gen entre 0 y largo_cromosoma-1
	        //El siguiente for coloca en la primera posicion el numero_gen y luego incrementa el valor de numero_gen en 1
	        //Dado un cromosoma de 10 gen desde el 0 al 9 y el numero_gen = 7 
	        //pos0 pos1 pos2 pos3 pos4 pos5 pos6 pos7 pos8 pos9
	        //7	   8    9    0    1    2    3    4    5    6
	        for(k = 0; k < chlistasize; k++) {
	            oldpop[j].pusListaPiezas[ k ] = numero_gen;
				numero_gen++;
				numero_gen=(numero_gen==chlistasize)?0:numero_gen;
	        }//End for
			// Inicializa cromosoma de string
	      	for(k = 0; k < chromsize; k++) 
	         	oldpop[j].chrom[k] = 0;
			// Inicializa cromosoma de mutación 
	      	for(k = 0; k < chmutsize; k++)
	        	oldpop[j].chmut[k] = 0;
	        oldpop[j].parent[0] = 0; // Initializa información de los padres
	        oldpop[j].parent[1] = 0;
	        oldpop[j].xsite 	= 0;
	      	oldpop[j].msite     = 0;
	        app_objfunc(tipo_problema, &(oldpop[j]));  // Evalua fitness inicial
	    }//End for
	}//End else
}//End initpop

void initialize(void)
// Inicialización de variables
{
	// Se considera relacion uno a uno => El largo de los cromosomas = largo_cromosoma
	bitsxcodigobinario = 1;
	
   	// Definición del largo del cromosoma...
	lchrom = largo_cromosoma * bitsxcodigobinario;
	lchmut = largo_cromosoma;

   	// Define el tamaño del cromosoma string en términos de los bytes de la máquina,
   	// esto es, el largo del cromosoma en bits (lchrom) / (bits por bytes)
   	// chromsize debe ser conocido en malloc() para puntero chrom
   	chromsize = (lchrom/UINTSIZE);
   	if(lchrom%UINTSIZE) chromsize++;

   	// Define tamaño del cromosoma de mutación en términos de bytes de máquina
   	chmutsize = (lchmut/UINTSIZE);
   	if(lchmut%UINTSIZE) chmutsize++;

   	// Define tamaño del cromosoma de lista
	chlistasize = largo_cromosoma;
 
   	// Inicializa generador de números aleatorios
   	randomize();

   	// Inicializa contadores / variables globales
   	nmutation 			= 0;
   	ncross 				= 0;
   	mmutation 			= 0;
   	mcross 				= 0;
   	bestfit.fitness 	= fitness_inicial;
   	bestfit.generation 	= 0;
   	gen 				= 0;
}//End initialize

