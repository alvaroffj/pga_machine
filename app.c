/*----------------------------------------------------------------------------*/
/* app.c - rutinas dependientes de la aplicación                              */
/*----------------------------------------------------------------------------*/
/*         PROBLEMA CORTE DE PIEZA GUILLOTINA BIDIMENSIONAL RESTRICTO         */
/*----------------------------------------------------------------------------*/

#include <stdlib.h>

#include "memory.h"
#include "sga.h"
#include "app_g.h"
#include "app_ng.h"
#include "app_ir.h"

int app_leearchivo(int tipo_problema, char *nombrearchivo, int rank_actual)
//Función principal de lectura del archivo
{
	if(tipo_problema == 0){ 
		//Problema de Corte de Pieza Guillotina
		if(app_leearchivo_g(nombrearchivo, rank_actual))
			return -1;
		else
			return 0;
	}//End if	
	else if(tipo_problema == 1){
		//Problema de Corte de Pieza No Guillotina
		if(app_leearchivo_ng(nombrearchivo, rank_actual))
			return -1;
		else
			return 0;
	}//End else if	
	else if(tipo_problema == 2){
		//Problema de Corte de Pieza Irregulares
		if(app_leearchivo_ir(nombrearchivo, rank_actual))
			return -1;
		else
			return 0;
	}//End else if	
	else
		return 0;
}//End app_leearchivo

void app_objfunc(int tipo_problema, struct individual *critter)
// Función Objetivo general, apunta a la función objetivo específica de acuerdo a la variable tipo_problema
{
	if(tipo_problema == 0) 
		//Problema de Corte de Pieza Guillotina
		app_objfunc_g(critter);
	else if(tipo_problema == 1)
		//Problema de Corte de Pieza No Guillotina
		app_objfunc_ng(critter);
	else if(tipo_problema == 2)
		//Problema de Corte de Pieza Irregulares
		app_objfunc_ir(critter);
}//End objfunc

void app_genera_resultados_problema(int corrida, int tipo_problema, char *nombrearchivo)
//Función que imprime los resultados del problema
{
	if(tipo_problema == 0) 
		//Problema de Corte de Pieza Guillotina
		app_genera_resultados_problema_g(corrida, tipo_problema, nombrearchivo);
	else if(tipo_problema == 1)
		//Problema de Corte de Pieza No Guillotina
		app_genera_resultados_problema_ng(corrida, tipo_problema, nombrearchivo);
	else if(tipo_problema == 2)
		//Problema de Corte de Pieza Irregulares
		app_genera_resultados_problema_ir(corrida, tipo_problema, nombrearchivo);
}//End app_genera_resultados_problema

void app_free(int tipo_problema) 
//Función que libera variables del problema
{
	if(tipo_problema == 0)
		//Problema de Corte de Pieza Guillotina
		app_free_g();
	else if(tipo_problema == 1)
		//Problema de Corte de Pieza No Guillotina
		app_free_ng();		
	else if(tipo_problema == 2)
		//Problema de Corte de Pieza Irregulares
		app_free_ir();		
}//End app_free	
