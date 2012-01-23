/*----------------------------------------------------------------------------*/
/* memory.c - Rutinas de administración de memoria para código M.P.A.G.P.     */
/*----------------------------------------------------------------------------*/

#include <stdlib.h>

#include "sga.h"

void nomemory(char *string_mem)
// Imprime error de memoria y sale del programa con error
{
  	fprintf(outfp,"malloc: out of memory en %s!!\n",string_mem);
  	exit(-1);
}//End nomemory

void initmalloc(void)
// Localiza memoria para estructuras de datos globales
{
   	unsigned nbytes;
   	int j;
	
	//************** MEMORIA PARA oldpop **************
   	// Memoria para antigua población de individuos
   	nbytes = popsize*sizeof(struct individual);
   	if((oldpop = (struct individual *) malloc(nbytes)) == NULL)
      	nomemory("oldpop");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, memory.c, 28, oldpop, %d\n", nbytes);
#endif
   	// Memoria para cromosoma string de la población antigua
   	nbytes = chromsize*sizeof(unsigned);
   	for(j = 0; j < popsize; j++) {
      	if((oldpop[j].chrom = (unsigned *) malloc(nbytes)) == NULL)
         	nomemory("oldpop chromosome string");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, memory.c, 36, oldpop[j].chrom, %d\n", nbytes);
#endif
   	}//End for
   	// Memoria para cromosoma mutación de la población antigua
   	nbytes = chmutsize*sizeof(unsigned);
   	for(j = 0; j < popsize; j++) {
      	if((oldpop[j].chmut = (unsigned *) malloc(nbytes)) == NULL)
         	nomemory("oldpop chromosome mutation");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, memory.c, 45, oldpop[j].chmut, %d\n", nbytes);
#endif
   	}//End for
   	// Memoria para cromosoma Lista de la población antigua
   	nbytes = chlistasize*sizeof(unsigned short);
   	for(j = 0; j < popsize; j++) {
      	if((oldpop[j].pusListaPiezas = (unsigned short *) malloc(nbytes)) == NULL)
		   	nomemory("oldpop chromosome Lista");
#ifdef _DEBUG_MALLOC_		
   printf("Malloc, memory.c, 54, oldpop[j].pusListaPiezas, %d\n", nbytes);
#endif
   	}//End for
   
   
	//************** MEMORIA PARA newpop **************
   	// Memoria para nueva población de individuos
   	nbytes = popsize*sizeof(struct individual);
   	if((newpop = (struct individual *) malloc(nbytes)) == NULL)
      	nomemory("newpop");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, memory.c, 65, newpop, %d\n", nbytes);
#endif
   	// Memoria para cromosoma string de la población nueva
   	nbytes = chromsize*sizeof(unsigned);
   	for(j = 0; j < popsize; j++) {
      	if((newpop[j].chrom = (unsigned *) malloc(nbytes)) == NULL)
         	nomemory("newpop chromosome string");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, memory.c, 73, newpop[j].chrom, %d\n", nbytes);
#endif
   	}//End for
   	// Memoria para cromosoma mutación de la población nueva
   	nbytes = chmutsize*sizeof(unsigned);
   	for(j = 0; j < popsize; j++) {
      	if((newpop[j].chmut = (unsigned *) malloc(nbytes)) == NULL)
         	nomemory("newpop chromosome mutation");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, memory.c, 82,newpop[j].chmut, %d\n", nbytes);
#endif
   	}//End for
   	// Memoria para cromosoma Lista de la población nueva
   	nbytes = chlistasize*sizeof(unsigned short);
   	for(j = 0; j < popsize; j++) {
      	if((newpop[j].pusListaPiezas = (unsigned short *) malloc(nbytes)) == NULL)
			nomemory("newpop chromosome Lista");
#ifdef _DEBUG_MALLOC_		
   printf("Malloc, memory.c, 91,newpop[j].pusListaPiezas, %d\n", nbytes);
#endif
   	}//End for
   

	//************** MEMORIA PARA bestfit **************
   	// Memoria para cromosoma string y mutación del mejor individuo de la población
   	nbytes = chromsize*sizeof(unsigned);
   	if((bestfit.chrom = (unsigned *) malloc(nbytes)) == NULL)
      	nomemory("bestfit chromosome string");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, memory.c, 102, bestfit.chrom, %d\n", nbytes);
#endif
   	nbytes = chmutsize*sizeof(unsigned);
   	if((bestfit.chmut = (unsigned *) malloc(nbytes)) == NULL)
      	nomemory("bestfit chromosome mutation");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, memory.c, 108, bestfit.chmut, %d\n", nbytes);
#endif
   	nbytes = chlistasize*sizeof(unsigned short);
   	if((bestfit.pusListaPiezas = (unsigned short *) malloc(nbytes)) == NULL)
	   	nomemory("bestfit chromosome Lista");
#ifdef _DEBUG_MALLOC_		
   printf("Malloc, memory.c, 114, bestfit.pusListaPiezas, %d\n", nbytes);
#endif
      

	//************** MEMORIA PARA indcross **************
   	// Memoria para cromosoma string y mutación del individuo utilizado para la cruza
   	nbytes = chromsize*sizeof(unsigned);
   	if((indcross.chrom = (unsigned *) malloc(nbytes)) == NULL)
      	nomemory("indcross chromosome string");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, memory.c, 124, indcross.chrom, %d\n", nbytes);
#endif
   	nbytes = chmutsize*sizeof(unsigned);
   	if((indcross.chmut = (unsigned *) malloc(nbytes)) == NULL)
      	nomemory("bestfit chromosome mutation");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, memory.c, 130, indcross.chmut, %d\n", nbytes);
#endif
   	nbytes = chlistasize*sizeof(unsigned short);
   	if((indcross.pusListaPiezas = (unsigned short *) malloc(nbytes)) == NULL)
      	nomemory("bestfit chromosome Lista");
#ifdef _DEBUG_MALLOC_		
   printf("Malloc, memory.c, 136, indcross.pusListaPiezas, %d\n", nbytes);
#endif
   

	//************** MEMORIA PARA envpop **************
   	// Memoria para población de individuos a enviar 
   	nbytes = n_ind_a_enviar*sizeof(struct individual);
   	if((envpop = (struct individual *) malloc(nbytes)) == NULL)
      	nomemory("envpop");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, memory.c, 146, envpop, %d\n", nbytes);
#endif
   	// Memoria para cromosoma string de la población de individuos a enviar 
   	nbytes = chromsize*sizeof(unsigned);
   	for(j = 0; j < n_ind_a_enviar; j++) {
      	if((envpop[j].chrom = (unsigned *) malloc(nbytes)) == NULL)
         	nomemory("envpop chromosome string");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, memory.c, 154, envpop[j].chrom, %d\n", nbytes);
#endif
   	}//End for
   	// Memoria para cromosoma mutación de la población de individuos a enviar 
   	nbytes = chmutsize*sizeof(unsigned);
   	for(j = 0; j < n_ind_a_enviar; j++) {
      	if((envpop[j].chmut = (unsigned *) malloc(nbytes)) == NULL)
         	nomemory("envpop chromosome mutation");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, memory.c, 163, envpop[j].chmut, %d\n", nbytes);
#endif
   	}//End for
   	// Memoria para cromosoma lista de la población de individuos a enviar 
   	nbytes = chlistasize*sizeof(unsigned short);
   	for(j = 0; j < n_ind_a_enviar; j++) {
      	if((envpop[j].pusListaPiezas = (unsigned short *) malloc(nbytes)) == NULL)
         	nomemory("envpop chromosome Lista");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, memory.c, 172, envpop[j].pusListaPiezas, %d\n", nbytes);
#endif
   	}//End for
   

	//************** MEMORIA PARA recpop **************
   	// Memoria para población de individuos a recibir
   	nbytes = n_ind_a_recibir*sizeof(struct individual);
   	if((recpop = (struct individual *) malloc(nbytes)) == NULL)
      	nomemory("recpop");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, memory.c, 183, recpop, %d\n", nbytes);
#endif
   	// Memoria para cromosoma string de la población de individuos a recibir
   	nbytes = chromsize*sizeof(unsigned);
   	for(j = 0; j < n_ind_a_recibir; j++) {
      	if((recpop[j].chrom = (unsigned *) malloc(nbytes)) == NULL)
         	nomemory("recpop chromosome string");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, memory.c, 191, recpop[j].chrom, %d\n", nbytes);
#endif
   	}//End for
   	// Memoria para cromosoma mutación de la población de individuos a recibir
   	nbytes = chmutsize*sizeof(unsigned);
   	for(j = 0; j < n_ind_a_recibir; j++) {
      	if((recpop[j].chmut = (unsigned *) malloc(nbytes)) == NULL)
         	nomemory("recpop chromosome mutation");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, memory.c, 200, recpop[j].chmut, %d\n", nbytes);
#endif
   	}//End for
   	// Memoria para cromosoma lista de la población de individuos a enviar 
   	nbytes = chlistasize*sizeof(unsigned short);
   	for(j = 0; j < n_ind_a_recibir; j++) {
      	if((recpop[j].pusListaPiezas = (unsigned short *) malloc(nbytes)) == NULL)
         	nomemory("recpop chromosome Lista");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, memory.c, 209, recpop[j].pusListaPiezas, %d\n", nbytes);
#endif
   	}//End for

}//End initmalloc

void freeall(void)
// Rutina que libera toda el espacio de memoria localizado din�micamente
{
   	int i;
  
	//************** LIBERA oldpop **************
   	for(i = 0; i < popsize; i++) {
      	free(oldpop[i].chrom);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, memory.c, 224, oldpop[i].chrom\n");
#endif
   		free(oldpop[i].chmut);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, memory.c, 228, oldpop[i].chmut\n");
#endif
		free(oldpop[i].pusListaPiezas);
#ifdef _DEBUG_MALLOC_		
   printf("Free, memory.c, 232, oldpop[i].pusListaPiezas\n");
#endif
   	}//End for
   	free(oldpop);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, memory.c, 237, oldpop\n");
#endif

	//************** LIBERA newpop **************
   	for(i = 0; i < popsize; i++) {
      	free(newpop[i].chrom);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, memory.c, 244, newpop[i].chrom\n");
#endif
      	free(newpop[i].chmut);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, memory.c, 248, newpop[i].chmut\n");
#endif
      	free(newpop[i].pusListaPiezas);
#ifdef _DEBUG_MALLOC_		
   printf("Free, memory.c, 252, newpop[i].pusListaPiezas\n");
#endif
	}//End for
   	free(newpop);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, memory.c, 257, newpop\n");
#endif
   
	//************** LIBERA bestfit **************
   	free(bestfit.chrom);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, memory.c, 263, bestfit.chrom\n");
#endif
   	free(bestfit.chmut);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, memory.c, 267, bestfit.chmut\n");
#endif
   	free(bestfit.pusListaPiezas);
#ifdef _DEBUG_MALLOC_		
   printf("Free, memory.c, 271, bestfit.pusListaPiezas\n");
#endif
   	
	//************** LIBERA indcross **************
   	free(indcross.chrom);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, memory.c, 277, indcross.chrom\n");
#endif
   	free(indcross.chmut);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, memory.c, 281, indcross.chmut\n");
#endif
   	free(indcross.pusListaPiezas);
#ifdef _DEBUG_MALLOC_		
   printf("Free, memory.c, 285, indcross.pusListaPiezas\n");
#endif
   
	//************** LIBERA envpop **************
   	for(i = 0; i < n_ind_a_enviar; i++) {
      	free(envpop[i].chrom);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, memory.c, 292, envpop[i].chrom\n");
#endif
   		free(envpop[i].chmut);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, memory.c, 296, envpop[i].chmut\n");
#endif
		free(envpop[i].pusListaPiezas);
#ifdef _DEBUG_MALLOC_		
   printf("Free, memory.c, 300, envpop[i].pusListaPiezas\n");
#endif
   	}//End for
   	free(envpop);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, memory.c, 305, envpop\n");
#endif

	//************** LIBERA recpop **************
   	for(i = 0; i < n_ind_a_recibir; i++) {
      	free(recpop[i].chrom);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, memory.c, 312, recpop[i].chrom\n");
#endif
   		free(recpop[i].chmut);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, memory.c, 316, recpop[i].chmut\n");
#endif
		free(recpop[i].pusListaPiezas);
#ifdef _DEBUG_MALLOC_		
   printf("Free, memory.c, 320, recpop[i].pusListaPiezas\n");
#endif
   	}//End for
   	free(recpop);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, memory.c, 325, recpop\n");
#endif
}//End freeall
