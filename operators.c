/*----------------------------------------------------------------------------*/
/* operators.c - Operadores Genéticos - Cruzamiento y Mutación                */
/*----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>

#include "sga.h"
#include "random.h"

void mutation(struct individual *critter)
// Muta cromosoma string con probabilidad pmutation, cuenta # de mutaciones = nrommut
// Muta cromosoma mutacion con probabilidad pmutation, cuenta # de mutaciones = nmutmut
{
   	int j, k, stop;
   	unsigned mask;
   	unsigned short aux;
#define MUTACION_1_BIT     0
#define MUTACION_MASCARA   1
#define TIPO_MUTACION      MUTACION_MASCARA

	if (tipo_elemento_cromosoma == BINARIO){
	   	if(flip(pmutation)) {
#if (TIPO_MUTACION ==	MUTACION_1_BIT)
			int jrommut, jmutmut, indice;
			unsigned temp;
	    	// Muta cromosoma string con probabilidad pmutation, cuenta # de mutaciones = nrommut
	      	jrommut = rnd(0 ,(lchrom - 1));// Mutacin entre 1 y l-1
			indice = 0;
	    	temp = 1;
	      	for(k = 0; k < chromsize; k++) {
	         	mask = 0;
	         	if(k == (chromsize-1)) 
	            	stop = lchrom - (k*UINTSIZE);
	         	else
	            	stop = UINTSIZE;
	         	for(j = 0; j < stop; j++) {
					if(jrommut == indice) {
	     				mask = mask|(temp<<j);
	     			   	nmutation++;
					   	j = stop;
	            	}//End if
	            	indice++;
	         	}//End for
	         	critter->chrom[k] = critter->chrom[k]^mask;
	      	}//End for
	
	    	// Muta cromosoma mutacion con probabilidad pmutation, cuenta # de mutaciones = nmutmut
	    	// Se muta sin restricciones, no importa si la mutacin sobre la pieza (giro) me da una pieza
	    	// que no cabe en la lmina, esto lo ve la funcin objetivo y no debera tomar esta pieza.
	      	jmutmut = rnd(0 ,(lchmut - 1));// Mutacin entre 1 y l-1
			indice = 0;
	    	temp = 1;
	      	for(k = 0; k < chmutsize; k++) {
	         	mask = 0;
	         	if(k == (chmutsize-1))
	            	stop = lchmut - (k*UINTSIZE);
	         	else
	            	stop = UINTSIZE;
	         	for(j = 0; j < stop; j++) {
					if(jmutmut == indice) {
	    				mask = mask|(temp<<j);
	     		      	mmutation++;
				   		j = stop;
	            	}//End if
					indice++;
	         	}//End for
	         	critter->chmut[k] = critter->chmut[k]^mask;
	      	}//End for
#elif (TIPO_MUTACION ==	MUTACION_MASCARA)
			mask = 1;
			//Mutación Uniforme en cromosoma String
			//Se genera máscara individuo mutación cromosoma string en forma aleatoria
	      	nmutation++;
	      	for(k = 0; k < chromsize; k++) {
	         	indcross.chrom[k] = 0;
	         	if(k == (chromsize-1))
	            	stop = lchrom - (k*UINTSIZE);
	         	else
	            	stop = UINTSIZE;
	         	for(j = 0; j < stop; j++) {
	            	indcross.chrom[k] = indcross.chrom[k]<<1;
	            	if(flip(0.5))
	               		indcross.chrom[k] = indcross.chrom[k]|mask;
	         	}//End for
				critter->chrom[k] = critter->chrom[k]^indcross.chrom[k];
	      	}//End for
	
	    	// Mutación Uniforme Cromosoma Rotación
			//Se genera máscara individuo mutación cromosoma string en forma aleatoria
	      	mmutation++;
	      	for(k = 0; k < chmutsize; k++) {
	         	indcross.chmut[k] = 0;
	         	if(k == (chmutsize-1))
	            	stop = lchmut - (k*UINTSIZE);
	         	else
	            	stop = UINTSIZE;
	         	for(j = 0; j < stop; j++) {
	            	indcross.chmut[k] = indcross.chmut[k]<<1;
	            	if(flip(0.5))
	               		indcross.chmut[k] = indcross.chmut[k]|mask;
	         	}//End for
				critter->chmut[k] = critter->chmut[k]^indcross.chmut[k];
	      	}//End for
#endif
	   	}//End if
	}//End if
   	else if (tipo_elemento_cromosoma == ENTEROS){
   		//Mutación corresponde a realizar una permutación dentro del cromosoma lista.
		for ( j = 0;j < chlistasize; j++){
			if ( flip( pmutation ) ){
				k = rnd( 1, chlistasize ) - 1;
				if ( j != k ){
					aux = critter->pusListaPiezas[ j ];     
					critter->pusListaPiezas[ j ] = critter->pusListaPiezas[ k ];
					critter->pusListaPiezas[ k ] = aux;
					nmutation++;
				}//End if
			}//End if
		}//End for
	}//End else if
}//End mutation

int iBuscaPieza( unsigned short uiPieza, unsigned short *puiPiezas )
//Busca el Valor de una Pieza dentro del cromosoma Lista y retorna la posición de esa Pieza
{
	int i;
	
	for ( i = 0; i < chlistasize; i++)
		if ( puiPiezas[ i ] == uiPieza )
			return( i );
	fprintf(outfp,"Error en iBuscaPieza\n");
  	exit(-1);
}//End iBuscaPieza

void IntercambiaPiezas( int i, int j, unsigned short *puiPiezas )
//Intercambia una pieza por otra dentro del mismo cromosoma
{
	unsigned int uiAuxiliar;

	uiAuxiliar = puiPiezas[ i ];
	puiPiezas[ i ] = puiPiezas[ j ];
	puiPiezas[ j ] = uiAuxiliar;
}//End IntercambiaPiezas

void xcrossover (unsigned *parent1,unsigned *parent2,unsigned *child1,unsigned *child2,
                 unsigned *mparent1,unsigned *mparent2,unsigned *mchild1,unsigned *mchild2)
// Función de Cruzamiento de 2 string padres, se transforma en 2 string hijos
// Cruzamiento del Cromosoma String
{
   	int j1, k, stop;
   	unsigned mask=1;

   	if(flip(pcross)) {
      	// Cruzamiento Cromosoma String
      	ncross++;
      	//Primero genero máscara individuo cruzamiento cromosoma string en forma aleatorio
      	for(k = 0; k < chromsize; k++) {
         	indcross.chrom[k] = 0;
         	if(k == (chromsize-1))
            	stop = lchrom - (k*UINTSIZE);
         	else
	            stop = UINTSIZE;
         	for(j1 = 1; j1 <= stop; j1++) {
            	indcross.chrom[k] = indcross.chrom[k]<<1;
            	if(flip(0.5))
               		indcross.chrom[k] = indcross.chrom[k]|mask;
         	}//End for
      	}//End for
   
      	// Cruzamiento Uniforme
      	// Luego con ese individuo voy viendo :
      	// Si bit máscara es cero
      	// 		hijo1 dejo bit padre2 y en hijo2 dejo bit padre1
      	// Si bit máscara es Uno
      	// 		hijo1 dejo bit padre1 y en hijo2 dejo bit padre2
      	for(k = 0; k < chromsize; k++) {
         	child1[k] = (parent1[k]&(~indcross.chrom[k]))|(parent2[k]&( indcross.chrom[k]));
         	child2[k] = (parent1[k]&( indcross.chrom[k]))|(parent2[k]&(~indcross.chrom[k]));
      	}//End for

      	// Cruzamiento Cromosoma Mutación
      	mcross++;
      	//Primero genero máscara individuo cruzamiento cromosoma mutación en forma aleatorio
      	for(k = 0; k < chmutsize; k++) {
         	indcross.chmut[k] = 0;
         	if(k == (chmutsize-1))
            	stop = lchmut - (k*UINTSIZE);
         	else
            	stop = UINTSIZE;
         	for(j1 = 1; j1 <= stop; j1++) {
            	indcross.chmut[k] = indcross.chmut[k]<<1;
            	if(flip(0.5))
               		indcross.chmut[k] = indcross.chmut[k]|mask;
         	}//End for
      	}//End for
      
      	// Cruzamiento Uniforme
      	// Luego con ese individuo voy viendo :
      	// Si bit máscara es cero
      	// 		hijo1 dejo bit padre2 y en hijo2 dejo bit padre1
      	// Si bit máscara es Uno
      	// 		hijo1 dejo bit padre1 y en hijo2 dejo bit padre2
      	for(k = 0; k < chmutsize; k++) {
         	mchild1[k] = (mparent1[k]&(~indcross.chmut[k]))|(mparent2[k]&( indcross.chmut[k]));
         	mchild2[k] = (mparent1[k]&( indcross.chmut[k]))|(mparent2[k]&(~indcross.chmut[k]));
      	}//End for
   	}//End if
   	else {
      	for(k = 0; k < chromsize; k++) {
         	child1[k] = parent1[k];
         	child2[k] = parent2[k];
      	}//End for
      	for(k = 0; k < chmutsize; k++) {
         	mchild1[k] = mparent1[k];
         	mchild2[k] = mparent2[k];
      	}//End for
   	}//End else
}//End xcrossover

int crossover (unsigned *parent1,unsigned *parent2,unsigned *child1,unsigned *child2,
               unsigned *mparent1,unsigned *mparent2,unsigned *mchild1,unsigned *mchild2,
               unsigned short *lparent1,unsigned short *lparent2,unsigned short *lchild1,unsigned short *lchild2)
// Función de Cruzamiento de 2 string padres, se transforma en 2 string hijos
// Cruzamiento del String Cromosoma
{
   	int j, jcross, kcross, k;
   	unsigned mask, temp;
	unsigned uInicio, uFin;

	if (tipo_elemento_cromosoma == BINARIO){
		//Realiza cruzamiento cromosoma cruzamiento y cromosoma rotación
	   	//Hace cruzamiento con probabilidad pcross de la población
	   	if(flip(pcross)) {
		   	//Cruzamiento Cromosoma String
	      	jcross = rnd(1 ,(lchrom - 1));// Cruzamiento entre 1 y l-1
			//jcross = (int) ceil((float) lchrom/ 2.0);// GMO
			ncross++;
	      	for(k = 1; k <= chromsize; k++) {
	         	if(jcross >= (k*UINTSIZE)) {
	            	child1[k-1] = parent1[k-1];
	            	child2[k-1] = parent2[k-1];
	         	}//end if
	         	else if((jcross < (k*UINTSIZE)) && (jcross > ((k-1)*UINTSIZE))) {
	            	mask = 1;
	            	for(j = 1; j <= (jcross-1-((k-1)*UINTSIZE)); j++) {
	            		temp = 1;
	               		mask = mask<<1;
	               		mask = mask|temp;
	            	}//End for
	            	child1[k-1] = (parent1[k-1]&mask)|(parent2[k-1]&(~mask));
	            	child2[k-1] = (parent1[k-1]&(~mask))|(parent2[k-1]&mask);
	         	}//End else if
	         	else {
	            	child1[k-1] = parent2[k-1];
	            	child2[k-1] = parent1[k-1];
	         	}//End else
	      	}//End for
	
			//Cruzamiento Cromosoma Mutacion
			kcross = jcross/bitsxcodigobinario;
			if(jcross%bitsxcodigobinario)	kcross++;
			mcross++;
	      	for(k = 1; k <= chmutsize; k++) {
	         	if(kcross >= (k*UINTSIZE)) {
	            	mchild1[k-1] = mparent1[k-1];
	            	mchild2[k-1] = mparent2[k-1];
	         	}//End if
	         	else if((kcross < (k*UINTSIZE)) && (kcross > ((k-1)*UINTSIZE))) {
	            	mask = 1;
	            	for(j = 1; j <= (kcross-1-((k-1)*UINTSIZE)); j++) {
	               		temp = 1;
	               		mask = mask<<1;
	               		mask = mask|temp;
	            	}//End for
	            	mchild1[k-1] = (mparent1[k-1]&mask)|(mparent2[k-1]&(~mask));
	            	mchild2[k-1] = (mparent1[k-1]&(~mask))|(mparent2[k-1]&mask);
	         	}//End else if
	         	else {
	            	mchild1[k-1] = mparent2[k-1];
	            	mchild2[k-1] = mparent1[k-1];
	         	}//End else
	      	}//End for
	   	}//End if
	   	else {
	      	for(k = 0; k < chromsize; k++) {
	         	child1[k] = parent1[k];
	         	child2[k] = parent2[k];
	      	}//End for
	      	for(k = 0; k < chmutsize; k++) {
	         	mchild1[k] = mparent1[k];
	         	mchild2[k] = mparent2[k];
	      	}//End for
	      	jcross = 0;
	   	}//End else
   	}//End if
	else if (tipo_elemento_cromosoma == ENTEROS){
		//Realiza cruzamiento de cromosoma Lista
		//Se utiliza cruzamiento PMX (Partially Mapped Crossover)
	    if(flip(pcross)){
			jcross = uInicio = rnd( 1, chlistasize - 1 );
			uFin = rnd( uInicio--, chlistasize ) - 1;
			//Copia lparent1 en lchild1 y lparent2 en lchild2	
			memcpy( lchild1, lparent1, sizeof( unsigned short ) * chlistasize );
			memcpy( lchild2, lparent2, sizeof( unsigned short ) * chlistasize );
			//Intercabia las Piezas desde uInicio y uFin
			for ( k = uInicio; k <= uFin; k++ ){
				IntercambiaPiezas( k, iBuscaPieza( lparent1[k], lchild2 ), lchild2 );
				IntercambiaPiezas( k, iBuscaPieza( lparent2[k], lchild1 ), lchild1 );
			}//End for
	    }//End if
	    else {
	        for(k = 0; k < chlistasize; k++) {
	            lchild1[k] = lparent1[k];
	            lchild2[k] = lparent2[k];
	        }//End for
	        jcross = 0;
	    }//End else
   	}//End else if
   	return(jcross);
}//End crossover
