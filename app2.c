/*--------------------------------------------------------------------------------------*/
/* app2.c - rutina general que permite llamar a la función objetivo según tipo_problema */
/*--------------------------------------------------------------------------------------*/

#include "sga.h"
#include "app2_g.h"
#include "app2_ng.h"
#include "app2_ir.h"

void app2_objfunc(int tipo_problema, char *nomarch, struct bestever bestcritter, int run_actual)
// Función Objetivo General
{
	if(tipo_problema == 0) 
		//Problema de Corte de Pieza Guillotina
		app2_objfunc_g(tipo_problema, nomarch, &(bestcritter), run_actual);
	else if(tipo_problema == 1)
		//Problema de Corte de Pieza No Guillotina
		app2_objfunc_ng(tipo_problema, nomarch, &(bestcritter), run_actual);
	else if(tipo_problema == 2)
		//Problema de Corte de Pieza Irregulares
		app2_objfunc_ir(tipo_problema, nomarch, &(bestcritter), run_actual); 
		
}//End app2_objfunc


