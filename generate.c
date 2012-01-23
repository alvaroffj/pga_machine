/*----------------------------------------------------------------------------*/
/* generate.c - crea una nueva generación de individuos                       */
/*----------------------------------------------------------------------------*/

#include "sga.h"
#include "operators.h"
#include "app.h"
#include "rselect.h"

void generation(int tipo_problema)
// Rutina que establece la generación de nuevos individuos
{
   	int mate1, mate2, jcross=0, jmcross=0, j = 0;
  
   	// Obtiene suma total del fitness para oldpop
   	preselect();

   	// Efectúa selección, cruzamiento y mutación
   	do {
      	// obtiene índices de individuos en oldpop a efectuar cruzamiento
      	mate1 = xselect(); 
      	mate2 = xselect();
		//Cruzamiento en un solo punto para cromosoma String, cromosoma Mutación, cromosoma Lista
      	jcross = crossover(oldpop[mate1].chrom, oldpop[mate2].chrom, newpop[j].chrom, newpop[j+1].chrom,
       					   oldpop[mate1].chmut, oldpop[mate2].chmut, newpop[j].chmut, newpop[j+1].chmut,
       					   oldpop[mate1].pusListaPiezas, oldpop[mate2].pusListaPiezas, newpop[j].pusListaPiezas, newpop[j+1].pusListaPiezas);
/*
	    // Cruzamiento Uniforme para cromosoma string y cromosoma mutation
      	xcrossover(oldpop[mate1].chrom, oldpop[mate2].chrom, newpop[j].chrom, newpop[j+1].chrom,
                   oldpop[mate1].chmut, oldpop[mate2].chmut, newpop[j].chmut, newpop[j+1].chmut);
*/

      	mutation(&(newpop[j]));
      	mutation(&(newpop[j+1]));

      	// Decodifica string, evalúa fitness y guarda parentesco de ambos hijos
      	app_objfunc(tipo_problema, &(newpop[j]));
      	newpop[j].parent[0]           = mate1+1;
      	newpop[j].xsite               = jcross;
      	jmcross                       = jcross/bitsxcodigobinario;
      	if(jcross%bitsxcodigobinario) jmcross++;
      	newpop[j].msite               = jmcross;
      	newpop[j].parent[1]           = mate2+1;

      	app_objfunc(tipo_problema, &(newpop[j+1]));
      	newpop[j+1].parent[0]         = newpop[j].parent[0];
      	newpop[j+1].xsite             = newpop[j].xsite;
      	newpop[j+1].msite             = newpop[j].msite;
      	newpop[j+1].parent[1]         = newpop[j].parent[1];

      	// Incrementa índice de la población
      	j = j + 2;
   	}while(j < (popsize-1));
}//End generation
