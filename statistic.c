/*----------------------------------------------------------------------------*/
/* statistic.c - obtiene estadìsticas                                         */
/*----------------------------------------------------------------------------*/

#include <stdlib.h>

#include "memory.h"
#include "sga.h"
#include "random.h"
#include "utility.h"

void statistics(struct individual *pop)
// Calcula Estadística de la población
{
   	int i, j, k=-1;
   	float sqrtf(float);
   
   	min = pop[0].fitness;
   	max = pop[0].fitness;
   	med = 0.0;
   	varianza = 0.0;

   	// Loop para max, min
   	for(j = 0; j < popsize; j++) {
      	if(pop[j].fitness > max) max = pop[j].fitness;         // Nuevo máximo
      	if(pop[j].fitness < min) min = pop[j].fitness;         // nuevo mánimo
      	med = med + pop[j].fitness; //Suma de fitness
      	varianza = varianza + (pop[j].fitness * pop[j].fitness); //calculo de la varianza
      	// Nuevo mejor individuo global
      	if(pop[j].fitness < bestfit.fitness) {
         	bestfit.fitness = pop[j].fitness;
         	k = j; //Quiere decir que se actualizó mejor individuo
      	}//End if
   	}//End for
   	med = (float) (med / (float) popsize);
   	varianza = ((float) (varianza / (float) popsize)) - (med * med); 
 
   	//Calcula la Desviación Estándar de la población
  	if(varianza < 0.0) varianza = (-1.0) * varianza; //Sólo se aceptan valores +
   	desvestandar = sqrtf(varianza);
   
   	// Si actualizó mejor individuo se pasan los datos restantes del mejor individuo.
   	if(k >= 0) {
     	for(i = 0; i < chromsize; i++) // Actualiza Cromosoma String
         	bestfit.chrom[i] = pop[k].chrom[i];
      	for(i = 0; i < chmutsize; i++)// Actualiza Cromosoma Mutación
         	bestfit.chmut[i] = pop[k].chmut[i];
		for(i = 0; i < chlistasize; i++) // Actualiza Cromosoma Lista
            bestfit.pusListaPiezas[i] = pop[k].pusListaPiezas[i];
       	bestfit.fitness     = pop[k].fitness;
      	bestfit.PEval       = pop[k].PEval;
      	bestfit.generation  = gen;
   	}//End if
	   
   	//Calcula el tiempo actual transcurrido
   	time_middle = MPI_Wtime() - time_start;
   
   	// Escribe en archivo de Estaditicas para problema particular
   	fprintf(evofp, "%d %d %.5f %.5f %.5f %.5f %.3f %d\n", n_migracion, gen, min, max, med, desvestandar, time_middle, bestfit.generation);
}//End statistics

struct individual *obtiene_individuos_a_enviar(struct individual *opop, struct individual *npop)
//Obtiene los Y individuos a enviar (selecciona los mejores o al azar)
{
   	int i, j;
	int FitnessCompara(struct individual *Indi, struct individual *Indj);

#define OBTIENE_MEJORES_INDIVIDUOS		0
#define OBTIENE_INDIVIDUOS_AL_AZAR  		1
#define TIPO_SELECCION 		         	OBTIENE_MEJORES_INDIVIDUOS   

#if (TIPO_SELECCION == OBTIENE_MEJORES_INDIVIDUOS)
   	//1ro. Toma la newpop(npop) y la copia en la oldpop(opop)
   	//2do. Ordena mediante un qsort la oldpop(opop)
   	//3ro. Obtiene de la oldpop(opop) los mejores Y individuos
   
   	//1ro. Toma la newpop(npop) y la copia en la oldpop(opop)
   	for(j = 0; j < popsize; j++) {
      	for(i = 0; i < chromsize; i++) // Actualiza Cromosoma String
         	opop[j].chrom[i] = npop[j].chrom[i];
      	for(i = 0; i < chmutsize; i++)// Actualiza Cromosoma Mutación
         	opop[j].chmut[i] = npop[j].chmut[i];
		for(i = 0; i < chlistasize; i++) // Actualiza Cromosoma Lista
            opop[j].pusListaPiezas[i] = npop[j].pusListaPiezas[i];
      	opop[j].fitness     = npop[j].fitness;
      	opop[j].PEval       = npop[j].PEval;
      	opop[j].xsite       = npop[j].xsite;
      	opop[j].msite       = npop[j].msite;
      	opop[j].parent[0]   = npop[j].parent[0];
      	opop[j].parent[1]   = npop[j].parent[1];
   	}//End for
   
	//2do. Ordena mediante un qsort la oldpop(opop)
   	//Deja primero los individuos con menor fitness en forma ascendente con respecto al fitness
   	qsort(opop, popsize, sizeof(struct individual), (void *) FitnessCompara);

   	//3ro. Obtiene de la oldpop(opop) los mejores Y individuos (n_ind_a_enviar)
   	for(j = 0; j < n_ind_a_enviar; j++) {
      	for(i = 0; i < chromsize; i++) // Actualiza Cromosoma String
         	envpop[j].chrom[i] = opop[j].chrom[i];
      	for(i = 0; i < chmutsize; i++)// Actualiza Cromosoma Mutación
         	envpop[j].chmut[i] = opop[j].chmut[i];
		for(i = 0; i < chlistasize; i++) // Actualiza Cromosoma Lista
            envpop[j].pusListaPiezas[i] = opop[j].pusListaPiezas[i];
      	envpop[j].fitness      = opop[j].fitness;
      	envpop[j].PEval        = opop[j].PEval;
      	envpop[j].xsite        = opop[j].xsite;
      	envpop[j].msite        = opop[j].msite;
      	envpop[j].parent[0]    = opop[j].parent[0];
      	envpop[j].parent[1]    = opop[j].parent[1];
   	}//End for
   
#elif (TIPO_SELECCION == OBTIENE_INDIVIDUOS_AL_AZAR)
   	int k;
   
   	//Obtiene individuos a enviar al azar
   	for(j = 0; j < n_ind_a_enviar; j++) {
      	k = rnd(0, popsize - 1);
      	for(i = 0; i < chromsize; i++) // Actualiza Cromosoma String
         	envpop[j].chrom[i] = opop[k].chrom[i];
      	for(i = 0; i < chmutsize; i++)// Actualiza Cromosoma Mutación
         	envpop[j].chmut[i] = opop[k].chmut[i];
		for(i = 0; i < chlistasize; i++) // Actualiza Cromosoma Lista
            envpop[j].pusListaPiezas[i] = opop[k].pusListaPiezas[i];
      	envpop[j].fitness      = opop[k].fitness;
      	envpop[j].PEval        = opop[k].PEval;
      	envpop[j].xsite        = opop[k].xsite;
      	envpop[j].msite        = opop[k].msite;
      	envpop[j].parent[0]    = opop[k].parent[0];
      	envpop[j].parent[1]    = opop[k].parent[1];
   	}//End for
#endif
   	return (envpop);
}//End obtiene_individuos_a_enviar

int FitnessCompara(struct individual *Indi, struct individual *Indj)
//Funcion que compara Individuos utilizando como comparacion el fitness de cada uno
{
   	if(Indi->fitness > Indj->fitness)
      	return(1);
   	if(Indi->fitness < Indj->fitness)
      	return(-1);
   	return(0);
}//End FitnessCompara

void inserta_individuos_en_poblacion(struct individual *rpop, struct individual *npop, struct individual *opop)
//Inserta Z individuos en población actual (newpop) sacando los peores o al azar
//rpop : Población de individuos recibida
//npop : Población de individuos nueva (newpop)
//opop : Población de individuos antigua (oldpop)
{
   	int i, j, k;

#define INSERTA_INDIVIDUOS_SACANDO_PEORES	   0
#define INSERTA_INDIVIDUOS_SACANDO_AL_AZAR	   1
#define TIPO_INSERCION 		                   INSERTA_INDIVIDUOS_SACANDO_PEORES

#if (TIPO_INSERCION == INSERTA_INDIVIDUOS_SACANDO_PEORES)
   	//1ro. Toma la newpop(npop) y la copia en opop(oldpop), 
   	//     pero en opop[j].msite se copia índice de posición del individuo en población.
   	//     Con este índice se identificará al individuo a reemplazar en la npop (newpop)
   	//2do. Ordena mediante un qsort la opop(oldpop), quedan al final los de mayor fitness,
   	//     los cuales serún los peores individuos a reemplazar
   	//3ro. Obtiene de la opop(oldpop) los peores n_ind_a_recibir individuos
   	//     y los reemplaza por los nuevos individuos de la poblacion rpop.
   	//     Finalmente, newpop tendrá en la posición de los peores individuos, los
   	//     provenientes de la población rpop.
   
   	//1ro. Toma la newpop(npop) y la copia en opop(oldpop) (sólo fitness e índice del fitness), 
   	//     pero en opop[j].msite se copia índice de posición del individuo en población.
   	//     Con este índice se identificará al individuo a reemplazar en la npop (newpop)
   	for(j = 0; j < popsize; j++) {
      	//for(i = 0; i < chromsize; i++) // Actualiza Cromosoma String
      	//   opop[j].chrom[i] = npop[j].chrom[i];
      	//for(i = 0; i < chmutsize; i++)// Actualiza Cromosoma Mutación
      	//   opop[j].chmut[i] = npop[j].chmut[i];
      	opop[j].fitness     = npop[j].fitness;
      	//opop[j].PEval       = npop[j].PEval;
      	//opop[j].xsite       = npop[j].xsite;
      	opop[j].msite       = j; //Se usa para saber a qué individuo corresponde este fitness
      	//opop[j].parent[0]   = npop[j].parent[0];
      	//opop[j].parent[1]   = npop[j].parent[1];
   	}//End for
   
   	//2do. Ordena mediante un qsort la opop(oldpop), quedan al final los de mayor fitness,
   	//     los cuales serán los peores individuos a reemplazar
  	qsort(opop, popsize, sizeof(struct individual), (void *) FitnessCompara);

   	//3ro. Obtiene de la opop(oldpop) los peores n_ind_a_recibir individuos
	//     y los reemplaza por los nuevos individuos de la poblacion rpop.
   	//     Finalmente, newpop tendrá en la posición de los peores individuos, los
   	//     provenientes de la población rpop.
   	for(j = 0; j < n_ind_a_recibir; j++) {
      	//k : es la posición de los j-ésimos peores individuos que se está buscando, 
      	//    ordenados en opop de menor a mayor fitness. Es decir, los peores están al final.
      	//(popsize - 1) - j : me dé la posición desde el final hacia atrás en la medida que avanza j
      	//opop[(popsize - 1) - j].msite : es la posición del individuo malo dentro 
      	//                                de npop(newpop) a ser reemplazado
      	k = opop[(popsize - 1) - j ].msite;
      	for(i = 0; i < chromsize; i++) // Actualiza Cromosoma String
         	npop[k].chrom[i] = rpop[j].chrom[i];
      	for(i = 0; i < chmutsize; i++)// Actualiza Cromosoma Mutación
         	npop[k].chmut[i] = rpop[j].chmut[i];
		for(i = 0; i < chlistasize; i++) // Actualiza Cromosoma Lista
            npop[k].pusListaPiezas[i] = rpop[j].pusListaPiezas[i];
      	npop[k].fitness     = rpop[j].fitness;
      	npop[k].PEval       = rpop[j].PEval;
      	npop[k].xsite       = rpop[j].xsite;
      	npop[k].msite       = rpop[j].msite;
      	npop[k].parent[0]   = rpop[j].parent[0];
      	npop[k].parent[1]   = rpop[j].parent[1];
   	}//End for

#elif (TIPO_INSERCION == INSERTA_INDIVIDUOS_SACANDO_AL_AZAR)
   	// recibe rpop individuos y los inserta al azar en npop (newpop)
   	for(j = 0; j < n_ind_a_recibir; j++) {
     	 k = rnd(0, popsize - 1);
      	for(i = 0; i < chromsize; i++) // Actualiza Cromosoma String
         	npop[k].chrom[i] = rpop[j].chrom[i];
      	for(i = 0; i < chmutsize; i++)// Actualiza Cromosoma Mutación
         	npop[k].chmut[i] = rpop[j].chmut[i];
		for(i = 0; i < chlistasize; i++) // Actualiza Cromosoma Lista
            npop[k].pusListaPiezas[i] = rpop[j].pusListaPiezas[i];
      	npop[k].fitness     = rpop[j].fitness;
      	npop[k].PEval       = rpop[j].PEval;
      	npop[k].xsite       = rpop[j].xsite;
      	npop[k].msite       = rpop[j].msite;
      	npop[k].parent[0]   = rpop[j].parent[0];
      	npop[k].parent[1]   = rpop[j].parent[1];
   	}//End for
#endif
}//End inserta_individuos_en_poblacion
