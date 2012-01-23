/*----------------------------------------------------------------------------*/
/* rselect.c:  roulette wheel selection.                                      */
/*----------------------------------------------------------------------------*/

#include "sga.h"
#include "random.h"

void preselect(void)
// Obtiene suma total del fitness para oldpop
{
   	int j;

   	sumfitness = 0.0;
   	for(j = 0; j < popsize; j++) {
      	if(oldpop[j].fitness != 0.0)
         	sumfitness += 1.0 / oldpop[j].fitness;
      	else //Esto para indicar un valor alto, sino se indeterminara
         	sumfitness += 10000.0;
   	}//End for
}//End preselect

int xselect(void)
// roulette-wheel selection que considera variables del tipo double
{
   	double sum, pick;
	int i;

   	pick = (double) randomperc();
   	sum = 0.0;

   	if(sumfitness != 0.0) {
      	for(i = 0; (sum < pick) && (i < popsize); i++)
         	if(oldpop[i].fitness != 0.0)         
	            sum += (double) (((double) 1.0) /(double) oldpop[i].fitness)/((double) sumfitness);
         	else //Esto para indicar un valor alto, sino se indeterminara
	            sum += (double) ((double) 10000.0)/((double) sumfitness);
   	}//End if
   	else
      	i = rnd(1, popsize);
   	return(i-1);
}//End xselect

