/*----------------------------------------------------------------------------------*/
/* app_ir.c - rutinas dependientes de la aplicación                           		*/
/*----------------------------------------------------------------------------------*/
/*       PROBLEMA CORTE DE PIEZAS IRREGULARES MANEJADO CON CROMOSOMA CON ENTEROS	*/
/*----------------------------------------------------------------------------------*/

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "var_pcpir.h"
#include "sga.h"
#include "memory.h"

//*****************************************************************
unsigned short 	usPiezaParaEncajar;
PIEZA	 		*ppzaPiezaParaEncajar;
PUNTO	 		*pptoPuntosPiezaParaEncajar;
unsigned short 	usPuntoPiezaParaEncajar;
double	 		dYMayorPerfil;
double   		dAreaPiezasEncajadas;
unsigned short 	usPuntoUltimo;
double   		dDeltaY;
double 			dDistanciaDesplazamiento;
double 			dDistanciaAuxiliar;
int				iPuntoPerfil;
//*****************************************************************

int app_leearchivo_ir(char *nombrearchivo, int rank_actual)
//Función principal de lectura del archivo
{
	FILE 		*fp; //puntero que identifca el archivo de instancia
	char 		nombre_archivo[100];
	unsigned	uPiezaActual, uVezActual, usPuntoActual, usPuntoUltimo;
	unsigned	uCantidadVecesPiezaActual, uCantidadPuntosPiezaActual;
	double		dYMenorII, dXMenorIII, dYMenorIII, dXMenorIV, dAreaPieza, dX, dY;
	int			iMaximoPuntosPerfil = 0, Orientacion, cont_orientacion;

	uMaximoRotaciones=3; //0=Sin Rotación, 1=Una Rotación, 2=Dos Rotaciones, 3=Tres Rotaciones


   	if(rank_actual == 0) //Lee archivo intancia de directorio intancias
      	sprintf(nombre_archivo, "%s%s", ruta_instancias, nombrearchivo);
   	else //Lee archivo desde directorio resultados, el archivo ha sido creado antes temporalmente
      	sprintf(nombre_archivo, "%s", nombrearchivo);

  	if((fp = fopen(nombre_archivo,"r"))== NULL){
		fprintf(outfp,"error al leer archivo %s\n",nombre_archivo);
		return 0;
   	}//End if
	
	//Formato Archivo
	//LINEA1		 : 	ANCHO_LAMINA 	CANT_TOTAL_PIEZAS
	//LINEAY_PIEZA_Z :	CANT_VECES_PIEZA_Z	AREA_PIEZA_Z CANT_PUNTOS_PIEZA_Z PUNTO1_X PUNTO1_Y PUNTO2_X PUNTO2_Y...PUNTON_X PUNTON_Y
	
	//Lee primera línea del archivo
	fscanf(fp, "%lf%d", &dAltoLamina, &uCantidadPiezas);

	//Reserva memoria para la estructura que almacenará las piezas del problema 
	if ( (ppzaPiezas = (PIEZA*)malloc(uCantidadPiezas*sizeof(PIEZA))) == NULL ){
     	nomemory("ppzaPiezas en app_lecturaarchivo_ir");
		#ifdef _DEBUG_MALLOC_		
		   printf("Malloc, App_ir.c, 43, ppzaPiezas, %d\n", uCantidadPiezas*sizeof(PIEZA));
		#endif
		return 0;
	}//End if

	uPiezaActual = 0;
	dAreaTotalPiezas = 0.0; //almacena el área total que ocupan las piezas
	cantidadtipospiezas = 0; //Variable que almacena la cantidad de tipos distintos de piezas.
	do{
		//Lee primeros datos de cada línea de piezas
		if (fscanf(fp, "%d%lf%d", &uCantidadVecesPiezaActual, &dAreaPieza, &uCantidadPuntosPiezaActual) < 1){
			fprintf(outfp,"Error al leer datos de piezas del archivo : %s!!\n",nombre_archivo);
            return 0;
		}//End if
		dAreaTotalPiezas += ((double)uCantidadVecesPiezaActual * dAreaPieza);
		cantidadtipospiezas ++;
		//Llena los datos de cada pieza y reserva memoria
		for ( uVezActual = 0; uVezActual < uCantidadVecesPiezaActual; uVezActual++ ){
			ppzaPiezas[ uPiezaActual + uVezActual ].usCantidadPuntos = uCantidadPuntosPiezaActual;
			ppzaPiezas[ uPiezaActual + uVezActual ].dArea = dAreaPieza;
			for ( cont_orientacion = 0; cont_orientacion < 4; cont_orientacion++ ){
				//Solicita memoria para almacenar todos los puntos de cada pieza considerando 4 orientaciones de cada pieza (una en cada cuadrante)
				ppzaPiezas[ uPiezaActual + uVezActual ].pptoPuntos[cont_orientacion] = (PUNTO*)malloc(uCantidadPuntosPiezaActual*sizeof(PUNTO));
				if ( ppzaPiezas[ uPiezaActual + uVezActual ].pptoPuntos[cont_orientacion] == NULL ){
			     	nomemory("ppzaPiezas[ uPiezaActual + uVezActual ].pptoPuntos[X] en app_lecturaarchivo_ir");
					#ifdef _DEBUG_MALLOC_		
					   printf("Malloc, App_ir.c, 65, ppzaPiezas, %d\n", uCantidadPuntosPiezaActual*sizeof(PUNTO));
					#endif
					return 0;
				}//End if
			}//End for
		}//End for
		//Lee cada punto y lo almacena en la estructura de piezas, consideran a cada punto en cada uno de los 4 cuadrantes
		for ( usPuntoActual = 0; usPuntoActual < uCantidadPuntosPiezaActual; usPuntoActual++ ){
			//Lee punto
			if (fscanf(fp, "%lf%lf", &dX, &dY) < 1){
				fprintf(outfp,"Error al leer datos (puntos) de piezas del archivo : %s!!\n",nombre_archivo);
	            return 0;
        	}//End if
        	//Obtiene la forma de cada pieza esté rotada en 90°.
			for ( uVezActual = 0; uVezActual < uCantidadVecesPiezaActual; uVezActual++ ){               // .pptoPuntos[3]  	.pptoPuntos[0]      
					//Punto en Cuadrante I																//			  	  |
					ppzaPiezas[ uPiezaActual + uVezActual ].pptoPuntos[0][ usPuntoActual ].dXAbs = dX;	//			  IV  |  I
        			ppzaPiezas[ uPiezaActual + uVezActual ].pptoPuntos[0][ usPuntoActual ].dYAbs = dY;  // 			 -----------
        			ppzaPiezas[ uPiezaActual + uVezActual ].pptoPuntos[0][ usPuntoActual ].dXRel = dX;	//			  III |  II
        			ppzaPiezas[ uPiezaActual + uVezActual ].pptoPuntos[0][ usPuntoActual ].dYRel = dY;	//				  |
					//Punto en Cuadrante II																   .pptoPuntos[2]	.pptoPuntos[1]
					ppzaPiezas[ uPiezaActual + uVezActual ].pptoPuntos[1][ usPuntoActual ].dXAbs = dY;
        			ppzaPiezas[ uPiezaActual + uVezActual ].pptoPuntos[1][ usPuntoActual ].dYAbs = -dX;
        			ppzaPiezas[ uPiezaActual + uVezActual ].pptoPuntos[1][ usPuntoActual ].dXRel = dY;
        			ppzaPiezas[ uPiezaActual + uVezActual ].pptoPuntos[1][ usPuntoActual ].dYRel = -dX;
        			//Punto en Cuadrante III
					ppzaPiezas[ uPiezaActual + uVezActual ].pptoPuntos[2][ usPuntoActual ].dXAbs = -dX;
        			ppzaPiezas[ uPiezaActual + uVezActual ].pptoPuntos[2][ usPuntoActual ].dYAbs = -dY;
        			ppzaPiezas[ uPiezaActual + uVezActual ].pptoPuntos[2][ usPuntoActual ].dXRel = -dX;
        			ppzaPiezas[ uPiezaActual + uVezActual ].pptoPuntos[2][ usPuntoActual ].dYRel = -dY;
        			//Punto en Cuadrante IV
					ppzaPiezas[ uPiezaActual + uVezActual ].pptoPuntos[3][ usPuntoActual ].dXAbs = -dY;
        			ppzaPiezas[ uPiezaActual + uVezActual ].pptoPuntos[3][ usPuntoActual ].dYAbs = dX;
        			ppzaPiezas[ uPiezaActual + uVezActual ].pptoPuntos[3][ usPuntoActual ].dXRel = -dY;
        			ppzaPiezas[ uPiezaActual + uVezActual ].pptoPuntos[3][ usPuntoActual ].dYRel = dX;
			}//End for
		}//End for
		uPiezaActual += uCantidadVecesPiezaActual; //avanza en la cantidad de piezas
	}while ( uPiezaActual < uCantidadPiezas );
	
	fclose(fp);
	//Con este for lo que se hace es desplazar las piezas Relativas (dXRel, dYRel) desde el cuadrante II, III, IV al cuadrante I.
	//Entonces, cada pieza se rota en 90° (4 posiciones => 4 formas distintas) y todas (4) las formas de cada pieza, quedan en el Cuadrante I
	for ( uPiezaActual = 0; uPiezaActual < uCantidadPiezas; uPiezaActual++ ){
		dYMenorII =	dXMenorIII = dYMenorIII = dXMenorIV = 0; 
		for ( usPuntoActual = 0; usPuntoActual < ppzaPiezas[ uPiezaActual ].usCantidadPuntos; usPuntoActual++ ){
			if ( ppzaPiezas[ uPiezaActual ].pptoPuntos[1][ usPuntoActual ].dYRel < dYMenorII )  // Y más negativo o más abajo en Cuadrante II
				dYMenorII = ppzaPiezas[ uPiezaActual ].pptoPuntos[1][ usPuntoActual ].dYRel;
			if ( ppzaPiezas[ uPiezaActual ].pptoPuntos[2][ usPuntoActual ].dXRel < dXMenorIII ) // X más negativo o más a la izquierda en Cuadrante III
				dXMenorIII = ppzaPiezas[ uPiezaActual ].pptoPuntos[2][ usPuntoActual ].dXRel;
			if ( ppzaPiezas[ uPiezaActual ].pptoPuntos[2][ usPuntoActual ].dYRel < dYMenorIII ) // Y más negativo o más abajo en Cuadrante III
				dYMenorIII = ppzaPiezas[ uPiezaActual ].pptoPuntos[2][ usPuntoActual ].dYRel;
			if ( ppzaPiezas[ uPiezaActual ].pptoPuntos[3][ usPuntoActual ].dXRel < dXMenorIV )  // X más negativo o más a la izquierda en Cuadrante IV
				dXMenorIV = ppzaPiezas[ uPiezaActual ].pptoPuntos[3][ usPuntoActual ].dXRel;
		}//End for
		//Cada punto relativo (Rel) de cada pieza se traslada al Cuadrante I.
		for ( usPuntoActual = 0; usPuntoActual < ppzaPiezas[ uPiezaActual ].usCantidadPuntos; usPuntoActual++ ){
			//Sube la componente dYRel de cada punto en dYMenorII para la Pieza del Cuadrante II hasta el Cuadrante I
			ppzaPiezas[ uPiezaActual ].pptoPuntos[1][ usPuntoActual ].dYRel = ppzaPiezas[ uPiezaActual ].pptoPuntos[1][ usPuntoActual ].dYRel - dYMenorII;
			//Desplaza hacia la derecha la componente dXRel de cada punto en dXMenorIII para la Pieza del Cuadrante III hasta el Cuadrante I
			ppzaPiezas[ uPiezaActual ].pptoPuntos[2][ usPuntoActual ].dXRel = ppzaPiezas[ uPiezaActual ].pptoPuntos[2][ usPuntoActual ].dXRel - dXMenorIII;
			//Sube la componente dYRel de cada punto en dYMenorIII para la Pieza del Cuadrante III hasta el Cuadrante I
			ppzaPiezas[ uPiezaActual ].pptoPuntos[2][ usPuntoActual ].dYRel = ppzaPiezas[ uPiezaActual ].pptoPuntos[2][ usPuntoActual ].dYRel - dYMenorIII;
			//Desplaza hacia la derecha la componente dXRel de cada punto en dXMenorIV para la Pieza del Cuadrante IV hasta el Cuadrante I
			ppzaPiezas[ uPiezaActual ].pptoPuntos[3][ usPuntoActual ].dXRel = ppzaPiezas[ uPiezaActual ].pptoPuntos[3][ usPuntoActual ].dXRel - dXMenorIV;
		}//End for
	}//End for
	//Llena la Estructura DATOSPIEZA de cada Pieza
	//Por cada orientación 0:0°, 1:90°, 2:180°, 3:270°
	for ( Orientacion = 0; Orientacion <= uMaximoRotaciones; Orientacion++ ){
		//Para cada pieza obtiene uPuntoXMenor,uPuntoYMenorXMenor,uPuntoYMenorXMayor,uPuntoYMayorXMenor,uPuntoYMayorXMayor,dAltura(alto de la pieza)
		for ( uPiezaActual = 0; uPiezaActual < uCantidadPiezas; uPiezaActual++ ){
			ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoXMenor =
			ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoYMenorXMenor =
			ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoYMayorXMenor =
			ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoYMenorXMayor =
			ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoYMayorXMayor = 0;
			//Para cada punto de cada pieza
			for ( usPuntoActual = 0; usPuntoActual < ppzaPiezas[uPiezaActual].usCantidadPuntos; usPuntoActual++ ){
				//Obtiene uPuntoXMenor
				if ( ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][usPuntoActual].dXRel < ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][ ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoXMenor ].dXRel )
					ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoXMenor = usPuntoActual;
				//Obtiene uPuntoYMenorXMenor
				if ( ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][usPuntoActual].dYRel < ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][ ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoYMenorXMenor ].dYRel )
					ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoYMenorXMenor = usPuntoActual;
				else if ( ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][usPuntoActual].dYRel == ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][ ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoYMenorXMenor ].dYRel &&
						  ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][usPuntoActual].dXRel < ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][ ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoYMenorXMenor ].dXRel )
					ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoYMenorXMenor = usPuntoActual;
				//Obtiene uPuntoYMayorXMenor
				if ( ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][usPuntoActual].dYRel > ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][ ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoYMayorXMenor ].dYRel )
					ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoYMayorXMenor = usPuntoActual;
				else if ( ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][usPuntoActual].dYRel == ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][ ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoYMayorXMenor ].dYRel &&
						  ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][usPuntoActual].dXRel < ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][ ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoYMayorXMenor ].dXRel )
					ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoYMayorXMenor = usPuntoActual;
				//Obtiene uPuntoYMenorXMayor
				if ( ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][usPuntoActual].dYRel < ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][ ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoYMenorXMayor ].dYRel )
					ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoYMenorXMayor = usPuntoActual;
				else if ( ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][usPuntoActual].dYRel == ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][ ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoYMenorXMayor ].dYRel &&
						  ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][usPuntoActual].dXRel > ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][ ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoYMenorXMayor ].dXRel )
					ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoYMenorXMayor = usPuntoActual;
				//Obtiene uPuntoYMayorXMayor
				if ( ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][usPuntoActual].dYRel > ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][ ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoYMayorXMayor ].dYRel )
					ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoYMayorXMayor = usPuntoActual;
				else if ( ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][usPuntoActual].dYRel == ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][ ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoYMayorXMayor ].dYRel &&
						  ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][usPuntoActual].dXRel > ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][ ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoYMayorXMayor ].dXRel )
					ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoYMayorXMayor = usPuntoActual;
			}//End for
			//Obtiene ancho relativo (en altura) que ocupa la pieza
			ppzaPiezas[uPiezaActual].Datos[Orientacion].dAltura = ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][ ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoYMayorXMayor ].dYRel -
					ppzaPiezas[uPiezaActual].pptoPuntos[Orientacion][ ppzaPiezas[uPiezaActual].Datos[Orientacion].uPuntoYMenorXMayor ].dYRel;
		}//End for
	}//End for
	//Obtiene cantidad máxima de puntos del perfil a lo alto de cada pieza
	for ( uPiezaActual = 0; uPiezaActual < uCantidadPiezas; uPiezaActual++ ){
		usPuntoUltimo = ppzaPiezas[ uPiezaActual ].Datos[0].uPuntoYMayorXMenor;
		for ( usPuntoActual = ppzaPiezas[ uPiezaActual ].Datos[0].uPuntoYMenorXMenor;
			  usPuntoActual != usPuntoUltimo;
			  usPuntoActual = ( usPuntoActual + 1 ) % ppzaPiezas[ uPiezaActual ].usCantidadPuntos ){
			iMaximoPuntosPerfil++;
			if ( ( usPuntoActual + 1 ) % ppzaPiezas[ uPiezaActual ].usCantidadPuntos == ppzaPiezas[ uPiezaActual ].Datos[0].uPuntoYMayorXMenor )
				usPuntoUltimo = ( usPuntoUltimo + 1 ) % ppzaPiezas[ uPiezaActual ].usCantidadPuntos;
		}//End for
	}//End for
	//Reserva memoria para cada perfil (4 perfiles)
	for ( cont_orientacion = 0; cont_orientacion < 4; cont_orientacion++ ){
		if ( (pPerfil[cont_orientacion] = (PUNTO**)malloc(2*iMaximoPuntosPerfil*sizeof(PUNTO*))) == NULL ){
	     	nomemory("pPerfil[X] en app_lecturaarchivo_ir");
			#ifdef _DEBUG_MALLOC_		
			   printf("Malloc, App_ir.c, 188, pPerfil[X], %d\n", 2*iMaximoPuntosPerfil*sizeof(PUNTO*));
			#endif
			return 0;
		}//End for
	}//End for
	//Reserva memoria para Perfil Auxiliar
	if ( (pPerfilAux1 = (PUNTO**)malloc(2*iMaximoPuntosPerfil*sizeof(PUNTO*))) == NULL ){
     	nomemory("pPerfilAux1 en app_lecturaarchivo_ir");
		#ifdef _DEBUG_MALLOC_		
		   printf("Malloc, App_ir.c, 198, pPerfilAux1, %d\n", 2*iMaximoPuntosPerfil*sizeof(PUNTO*));
		#endif
		return 0;
	}//End if
	
	//Setea variables globales
	//*** NEW
	CantidadBitsxPieza = ceil(log10(uCantidadPiezas)/log10(2));
	largo_cromosoma = CantidadBitsxPieza * uCantidadPiezas; //Define el largo del cromosoma
	//***
	fitness_inicial = 99999999.0; //Obtiene el fitness_inicial
	
	return -1;
}//End app_leearchivo_ir

void app_free_ir(void)
//Función que libera variables del problema
{
	unsigned	uPiezaActual;
	int			cont_orientacion;

	for ( uPiezaActual = 0; uPiezaActual < uCantidadPiezas; uPiezaActual++ ){
		for ( cont_orientacion = 0; cont_orientacion < 4; cont_orientacion++ )
			free(ppzaPiezas[ uPiezaActual ].pptoPuntos[cont_orientacion]);
	}//End for
	free(ppzaPiezas);
	for ( cont_orientacion = 0; cont_orientacion < 4; cont_orientacion++ )
		free(pPerfil[cont_orientacion]);
	free(pPerfilAux1);
}//End app_free_ir

double app_EncajePrimeraPieza_ir(unsigned Orientacion)
//Encaja la primera pieza considerando la Orientacion dada
{
	int i;
	double dXMax = 0.0;
	//Los puntos Relativos (dXRel, dYRel) se asignan a los puntos Absolutos (dXAbs, dYAbs)
	for ( usPuntoPiezaParaEncajar = 0; usPuntoPiezaParaEncajar < ppzaPiezaParaEncajar->usCantidadPuntos;
		usPuntoPiezaParaEncajar++ ){
		pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dXAbs = pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dXRel;
		pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dYAbs = pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dYRel;
		//Intenta obtener el valor de X mayor para la pieza ppzaPiezaParaEncajar
		if ( pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dXAbs > dXMax )
			dXMax = pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dXAbs;
	}//End for
	dYMayorPerfil = pptoPuntosPiezaParaEncajar[ ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMayorXMenor ].dYRel;

	// Meter al perfil[Orientacion] 
	iInicioPerfil[Orientacion] = 0;
	i = 0;
	usPuntoUltimo = ppzaPiezas[ usPiezaParaEncajar ].Datos[Orientacion].uPuntoYMayorXMayor;
	iPuntoMasIzquierdaAbajo[Orientacion] = 0;
	//For que introduce en el perfil(orientacion) los puntos desde el más abajo a la derecha hacia arriba 
	for ( usPuntoPiezaParaEncajar = ppzaPiezas[ usPiezaParaEncajar ].Datos[Orientacion].uPuntoYMenorXMayor;
		  usPuntoPiezaParaEncajar != usPuntoUltimo;
		  usPuntoPiezaParaEncajar = ( usPuntoPiezaParaEncajar + 1 ) % ppzaPiezaParaEncajar->usCantidadPuntos ){
		//Introduce el punto en el Perfil
		pPerfil[Orientacion][ i ] = pptoPuntosPiezaParaEncajar + usPuntoPiezaParaEncajar;
		//Intenta obtener el punto más a la izquierda y más abajo
		if ( pPerfil[Orientacion][ i ]->dXAbs < pPerfil[Orientacion][ iPuntoMasIzquierdaAbajo[Orientacion] ]->dXAbs )
			iPuntoMasIzquierdaAbajo[Orientacion] = i;
		else if ( pPerfil[Orientacion][ i ]->dXAbs == pPerfil[Orientacion][ iPuntoMasIzquierdaAbajo[Orientacion] ]->dXAbs &&
				  pPerfil[Orientacion][ i ]->dYAbs < pPerfil[Orientacion][ iPuntoMasIzquierdaAbajo[Orientacion] ]->dYAbs )
			iPuntoMasIzquierdaAbajo[Orientacion] = i;
		//Mecanismo de salida del for: Esto para que pueda tomar el último punto : usPuntoUltimo
		if ( ( usPuntoPiezaParaEncajar + 1 ) % ppzaPiezaParaEncajar->usCantidadPuntos == ppzaPiezas[ usPiezaParaEncajar ].Datos[Orientacion].uPuntoYMayorXMayor )
			usPuntoUltimo = ( usPuntoUltimo + 1 ) % ppzaPiezaParaEncajar->usCantidadPuntos;
//if ( gen==maxgen )
//printf("[%d](%f,%f);",i,pPerfil[Orientacion][ i ]->dXAbs,pPerfil[Orientacion][ i ]->dYAbs);
		i++;
	}//End for
	iFinPerfil[Orientacion] = i - 1;
	return( dXMax ); //Retorna el punto más a la derecha.
}//End app_EncajePrimeraPieza_ir

void app_EncajeEnBanda_ir(unsigned Orientacion)
//Función que Encaja en banda: La pieza será colocada arriba del Perfil
{
	//Cada punto de pptoPuntosPiezaParaEncajar (la pieza a encajar) se sube hasta el último punto (el más arriba) del Perfil
	for ( usPuntoPiezaParaEncajar = 0; usPuntoPiezaParaEncajar < ppzaPiezaParaEncajar->usCantidadPuntos;
		usPuntoPiezaParaEncajar++ ){
		pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dXAbs = pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dXRel;
		pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dYAbs = pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dYRel
			+ pPerfil[Orientacion][ iFinPerfil[Orientacion] ]->dYAbs;
	}//End for
}//End app_EncajeEnBanda_ir

void app_DeterminaPosicionInicial_ir(unsigned Orientacion, double dMax)
//Función que Ubica la pieza en el punto de partida desde la derecha del perfil
{
	double dCorrimiento;
	double r, s;

	if ( iPuntoMasIzquierdaAbajo[Orientacion] == iInicioPerfil[Orientacion] ||
		 pPerfil[Orientacion][ iPuntoMasIzquierdaAbajo[Orientacion] ]->dYAbs == pPerfil[Orientacion][ iInicioPerfil[Orientacion] ]->dYAbs )
		//El punto más a la izquierda abajo es el punto Inicio_Perfil
		dCorrimiento = 0;
	else if ( iPuntoMasIzquierdaAbajo[Orientacion] == iFinPerfil[Orientacion] ||
		 pPerfil[Orientacion][ iPuntoMasIzquierdaAbajo[Orientacion] ]->dYAbs == pPerfil[Orientacion][ iFinPerfil[Orientacion] ]->dYAbs )
		//El punto más a la izquierda abajo es el punto Fin_Perfil
		dCorrimiento = -1.0 * ppzaPiezaParaEncajar->Datos[Orientacion].dAltura;
	else {
		//El punto más a la izquierda abajo está entre el Inicio_Perfil y el Fin_Perfil
		s = pPerfil[Orientacion][ iPuntoMasIzquierdaAbajo[Orientacion] ]->dYAbs - pPerfil[Orientacion][ iPuntoMasIzquierdaAbajo[Orientacion] - 1 ]->dYAbs;
		r = pPerfil[Orientacion][ iPuntoMasIzquierdaAbajo[Orientacion] + 1 ]->dYAbs - pPerfil[Orientacion][ iPuntoMasIzquierdaAbajo[Orientacion] ]->dYAbs;
		if ( (r + s == 0.0) || s == 0.0 )
			dCorrimiento = 0;
		else
			dCorrimiento = -1.0 * (double)( ( s / ( r + s ) ) * ppzaPiezaParaEncajar->Datos[Orientacion].dAltura);
//		dCorrimiento = -(double)(ppzaPiezaParaEncajar->dAltura / 2.0);
	}//End else
	//*****************************************************
	//dCorrimiento = 0;
	//*****************************************************
	for ( usPuntoPiezaParaEncajar = 0;
		  usPuntoPiezaParaEncajar < ppzaPiezaParaEncajar->usCantidadPuntos;
		  usPuntoPiezaParaEncajar++ ){

		pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dXAbs = 
//***************** LINEA MODIFICADA **************************
			dMax + 99999999.0 +	pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dXRel;
			//dMax + pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dXRel;
//***************** LINEA MODIFICADA **************************

		pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dYAbs =
			pPerfil[Orientacion][ iPuntoMasIzquierdaAbajo[Orientacion] ]->dYAbs	+
			pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dYRel	+ dCorrimiento
//			-
//			(double)(ppzaPiezaParaEncajar->dAltura / 2.0)
			;
	}//End for
	//Ver cuando se usa el dDeltaY
	dDeltaY = 0.0;
	if ( pptoPuntosPiezaParaEncajar[ ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMenorXMenor ].dYAbs < 0.0 )
		dDeltaY = -1.0 * pptoPuntosPiezaParaEncajar[ ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMenorXMenor ].dYAbs;
	if ( pptoPuntosPiezaParaEncajar[ ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMayorXMenor ].dYAbs > dAltoLamina )
		dDeltaY = -1.0 * ( pptoPuntosPiezaParaEncajar[ ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMayorXMenor ].dYAbs - dAltoLamina );
	if ( dDeltaY != 0.0 )
		for ( usPuntoPiezaParaEncajar = 0;
			usPuntoPiezaParaEncajar < ppzaPiezaParaEncajar->usCantidadPuntos;
			usPuntoPiezaParaEncajar++ ){
			pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dYAbs = 
				pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dYAbs + dDeltaY;
		}//End for
}//End app_DeterminaPosicionInicial_ir

double app_dObtieneDistanciaPuntoLado_ir( double xP, double yP,	double xA, double yA, double xB, double yB )
//Función que obtiene distancia punto lado
{
	double dDistanciaPA;
	double dDistanciaPB;
	double fabs();
	double xQ;

	if ( yA == yB ){
		dDistanciaPA = fabs( xA - xP );
		dDistanciaPB = fabs( xB - xP );
		if ( dDistanciaPA < dDistanciaPB )
			return ( dDistanciaPA );
		return ( dDistanciaPB );		
	}else {
		xQ = ( yP - yA ) * ( ( xB - xA ) / ( yB - yA ) ) + xA;
		return ( fabs( xP - xQ ) );
	}//End if
}//End app_dObtieneDistanciaPuntoLado_ir

void app_BuscaDistanciaDesplazamiento_ir(unsigned Orientacion)
//Función que Mide y compara distancia al eje y
{
	BOOLEAN	fPrimero = TRUE;

	if ( 0 ){
		usPuntoUltimo = ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMenorXMenor;
		for ( usPuntoPiezaParaEncajar = ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMayorXMenor;
			  usPuntoPiezaParaEncajar != usPuntoUltimo;
			  usPuntoPiezaParaEncajar = ( usPuntoPiezaParaEncajar + 1 ) % ppzaPiezaParaEncajar->usCantidadPuntos ){
			printf( "(%f,%f)\n",
								pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dXAbs,
								pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dYAbs);
			if ( ( usPuntoPiezaParaEncajar + 1 ) % ppzaPiezaParaEncajar->usCantidadPuntos == ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMenorXMenor )
				usPuntoUltimo = ( usPuntoUltimo + 1 ) % ppzaPiezaParaEncajar->usCantidadPuntos;
		}//End for
		for ( iPuntoPerfil = iInicioPerfil[Orientacion]; iPuntoPerfil <= iFinPerfil[Orientacion]; iPuntoPerfil++ )
			printf( "(%f,%f)\n",
								pPerfil[Orientacion][ iPuntoPerfil ]->dXAbs,
								pPerfil[Orientacion][ iPuntoPerfil ]->dYAbs);
	}//End if
	/* Punto del perfil y lado de la pieza */
	usPuntoUltimo = ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMenorXMenor;
	for ( usPuntoPiezaParaEncajar = ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMayorXMenor;
		  usPuntoPiezaParaEncajar != usPuntoUltimo;
		  usPuntoPiezaParaEncajar = ( usPuntoPiezaParaEncajar + 1 ) % ppzaPiezaParaEncajar->usCantidadPuntos ){
		for ( iPuntoPerfil = iInicioPerfil[Orientacion]; iPuntoPerfil <= iFinPerfil[Orientacion]; iPuntoPerfil++ ){
			if ( pPerfil[Orientacion][ iPuntoPerfil ]->dYAbs < pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dYAbs &&
				pPerfil[Orientacion][ iPuntoPerfil ]->dYAbs > pptoPuntosPiezaParaEncajar[ ( usPuntoPiezaParaEncajar + 1 ) % ppzaPiezaParaEncajar->usCantidadPuntos ].dYAbs ){
				dDistanciaAuxiliar = app_dObtieneDistanciaPuntoLado_ir(
								pPerfil[Orientacion][ iPuntoPerfil ]->dXAbs,
								pPerfil[Orientacion][ iPuntoPerfil ]->dYAbs,
								pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dXAbs,
								pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dYAbs,
								pptoPuntosPiezaParaEncajar[ ( usPuntoPiezaParaEncajar + 1 ) % ppzaPiezaParaEncajar->usCantidadPuntos ].dXAbs,
								pptoPuntosPiezaParaEncajar[ ( usPuntoPiezaParaEncajar + 1 ) % ppzaPiezaParaEncajar->usCantidadPuntos ].dYAbs );
				if ( fPrimero || dDistanciaAuxiliar < dDistanciaDesplazamiento ){
					dDistanciaDesplazamiento = dDistanciaAuxiliar;
					fPrimero = FALSE;
				}//End if
			}//End if
		}//End for
		if ( ( usPuntoPiezaParaEncajar + 1 ) % ppzaPiezaParaEncajar->usCantidadPuntos == ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMenorXMenor )
			usPuntoUltimo = ( usPuntoUltimo + 1 ) % ppzaPiezaParaEncajar->usCantidadPuntos;
	}//End for
	/* Punto de la pieza y lado del perfil */
	for ( iPuntoPerfil = iInicioPerfil[Orientacion]; iPuntoPerfil < iFinPerfil[Orientacion]; iPuntoPerfil++ ){
		usPuntoUltimo = ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMenorXMenor;
		for ( usPuntoPiezaParaEncajar = ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMayorXMenor;
			  usPuntoPiezaParaEncajar != usPuntoUltimo;
			  usPuntoPiezaParaEncajar = ( usPuntoPiezaParaEncajar + 1 ) % ppzaPiezaParaEncajar->usCantidadPuntos ){
			if ( pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dYAbs > pPerfil[Orientacion][ iPuntoPerfil ]->dYAbs &&
				 pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dYAbs < pPerfil[Orientacion][ iPuntoPerfil + 1 ]->dYAbs ){
				dDistanciaAuxiliar = app_dObtieneDistanciaPuntoLado_ir(
								pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dXAbs,
								pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dYAbs,
								pPerfil[Orientacion][ iPuntoPerfil ]->dXAbs,
								pPerfil[Orientacion][ iPuntoPerfil ]->dYAbs,
								pPerfil[Orientacion][ iPuntoPerfil + 1 ]->dXAbs,
								pPerfil[Orientacion][ iPuntoPerfil + 1 ]->dYAbs );
				if ( fPrimero || dDistanciaAuxiliar < dDistanciaDesplazamiento ){
					dDistanciaDesplazamiento = dDistanciaAuxiliar;
					fPrimero = FALSE;
				}//End if
			}//End if
			if ( ( usPuntoPiezaParaEncajar + 1 ) % ppzaPiezaParaEncajar->usCantidadPuntos == ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMenorXMenor )
				usPuntoUltimo = ( usPuntoUltimo + 1 ) % ppzaPiezaParaEncajar->usCantidadPuntos;
		}//End for
	}//End for
	for ( iPuntoPerfil = iInicioPerfil[Orientacion]; iPuntoPerfil <= iFinPerfil[Orientacion]; iPuntoPerfil++ ){
		usPuntoUltimo = ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMenorXMenor;
		for ( usPuntoPiezaParaEncajar = ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMayorXMenor;
			  usPuntoPiezaParaEncajar != usPuntoUltimo;
			  usPuntoPiezaParaEncajar = ( usPuntoPiezaParaEncajar + 1 ) % ppzaPiezaParaEncajar->usCantidadPuntos ){
			if ( pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dYAbs == pPerfil[Orientacion][ iPuntoPerfil ]->dYAbs ){
				
				if ( (usPuntoPiezaParaEncajar != ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMenorXMenor && usPuntoPiezaParaEncajar != ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMayorXMenor) ||
					(usPuntoPiezaParaEncajar == ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMenorXMenor && ( iPuntoPerfil == iFinPerfil[Orientacion] || pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dYAbs < pPerfil[Orientacion][ iPuntoPerfil + 1 ]->dYAbs )) ||
					(usPuntoPiezaParaEncajar == ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMayorXMenor && pPerfil[Orientacion][ iPuntoPerfil ]->dYAbs > pPerfil[Orientacion][ iPuntoPerfil - 1 ]->dYAbs) ){

				//if ( usPuntoPiezaParaEncajar != ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMenorXMenor && usPuntoPiezaParaEncajar != ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMayorXMenor ||
				//	usPuntoPiezaParaEncajar == ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMenorXMenor && ( iPuntoPerfil == iFinPerfil[Orientacion] || pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dYAbs < pPerfil[Orientacion][ iPuntoPerfil + 1 ]->dYAbs ) ||
				//	usPuntoPiezaParaEncajar == ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMayorXMenor && pPerfil[Orientacion][ iPuntoPerfil ]->dYAbs > pPerfil[Orientacion][ iPuntoPerfil - 1 ]->dYAbs ) {

					dDistanciaAuxiliar = pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dXAbs -
										 pPerfil[Orientacion][ iPuntoPerfil ]->dXAbs;
					if ( fPrimero || dDistanciaAuxiliar < dDistanciaDesplazamiento ){
						dDistanciaDesplazamiento = dDistanciaAuxiliar;
						fPrimero = FALSE;
					}//End if
				}//End if
			}//End if
			if ( ( usPuntoPiezaParaEncajar + 1 ) % ppzaPiezaParaEncajar->usCantidadPuntos == ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMenorXMenor )
				usPuntoUltimo = ( usPuntoUltimo + 1 ) % ppzaPiezaParaEncajar->usCantidadPuntos;
		}//End for
	}//End for
	usPuntoUltimo = ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMenorXMenor;
	for ( usPuntoPiezaParaEncajar = ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMayorXMenor;
		  usPuntoPiezaParaEncajar != usPuntoUltimo;
		  usPuntoPiezaParaEncajar = ( usPuntoPiezaParaEncajar + 1 ) % ppzaPiezaParaEncajar->usCantidadPuntos ){
		if ( pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dXAbs < dDistanciaDesplazamiento )
			dDistanciaDesplazamiento = pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dXAbs;
		if ( ( usPuntoPiezaParaEncajar + 1 ) % ppzaPiezaParaEncajar->usCantidadPuntos == ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMenorXMenor )
			usPuntoUltimo = ( usPuntoUltimo + 1 ) % ppzaPiezaParaEncajar->usCantidadPuntos;
	}//End for
}//End app_BuscaDistanciaDesplazamiento_ir

void app_DesplazaPieza_ir(void)
//Función desplaza pieza
{
	for ( usPuntoPiezaParaEncajar = 0;
		usPuntoPiezaParaEncajar < ppzaPiezaParaEncajar->usCantidadPuntos;
		usPuntoPiezaParaEncajar++ ){
		if ( ( pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dXAbs = 
			pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dXAbs
			-
			dDistanciaDesplazamiento ) < 0.0 ){
			printf("Falla\n");
			printf("x:%f,desp:%f,xfinal:%f\n",
				pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dXAbs + dDistanciaDesplazamiento,
				dDistanciaDesplazamiento,
				pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dXAbs );
			//exit(1);
		}//End if
	}//End for
}//End app_DesplazaPieza_ir

double app_ActualizaPerfil_ir(unsigned Orientacion)
//Función que actualiza el perfil
{
	int iPuntoPerfilAntiguo;
	int iPuntoPerfilNuevo;

	BOOLEAN fPiezaIntegrada;
	BOOLEAN fComienzaPieza;

	int	i;
	double dMaximoX;

	iPuntoPerfilAntiguo = 0;
	iPuntoPerfilNuevo = 0;
	fPiezaIntegrada = 0;
	fComienzaPieza = 0;
	while ( iPuntoPerfilAntiguo <= iFinPerfil[Orientacion] || !fPiezaIntegrada ){
		if ( iPuntoPerfilAntiguo <= iFinPerfil[Orientacion] &&
			pPerfil[Orientacion][ iPuntoPerfilAntiguo ]->dYAbs < pptoPuntosPiezaParaEncajar[ ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMenorXMenor ].dYAbs ){

			//Todos los puntos del perfil actual los deja en pPerfilAux1 excepto el último punto del perfil
			pPerfilAux1[ iPuntoPerfilNuevo ] = pPerfil[Orientacion][ iPuntoPerfilAntiguo ];
			iPuntoPerfilNuevo++;
			iPuntoPerfilAntiguo++;

		} else if ( !fComienzaPieza &&
			iPuntoPerfilAntiguo <= iFinPerfil[Orientacion] &&
			pPerfil[Orientacion][ iPuntoPerfilAntiguo ]->dYAbs == pptoPuntosPiezaParaEncajar[ ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMenorXMenor ].dYAbs &&
			pPerfil[Orientacion][ iPuntoPerfilAntiguo ]->dYAbs > 0 ){

			//Está en el último punto del pefil (el de más arriba)
			pPerfilAux1[ iPuntoPerfilNuevo ] = pPerfil[Orientacion][ iPuntoPerfilAntiguo ];
			iPuntoPerfilNuevo++;
			iPuntoPerfilAntiguo++;
			fComienzaPieza = 1;

		} else{

			/*Incorporación del perfil de la pieza al perfil nuevo*/
			usPuntoUltimo = ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMayorXMenor;
			for ( usPuntoPiezaParaEncajar = ( pptoPuntosPiezaParaEncajar[ ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMenorXMenor ].dYAbs == 0 )?ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMenorXMayor:ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMenorXMenor;
				  usPuntoPiezaParaEncajar != usPuntoUltimo;
				  usPuntoPiezaParaEncajar = ( usPuntoPiezaParaEncajar + 1 ) % ppzaPiezaParaEncajar->usCantidadPuntos ){
				
				//Actualiza el nuevo perfil pPerfilAux1 con los puntos de la nueva pieza
				pPerfilAux1[ iPuntoPerfilNuevo ] = pptoPuntosPiezaParaEncajar + usPuntoPiezaParaEncajar;
				iPuntoPerfilNuevo++;

				if ( ( usPuntoPiezaParaEncajar + 1 ) % ppzaPiezaParaEncajar->usCantidadPuntos == ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMayorXMenor )
					usPuntoUltimo = ( usPuntoUltimo + 1 ) % ppzaPiezaParaEncajar->usCantidadPuntos;
			}//End for
			fPiezaIntegrada = 1;

			while ( iPuntoPerfilAntiguo <= iFinPerfil[Orientacion]
					&&
					pPerfil[Orientacion][ iPuntoPerfilAntiguo ]->dYAbs < pptoPuntosPiezaParaEncajar[ ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMayorXMenor ].dYAbs ){
				iPuntoPerfilAntiguo++;
			}//End while
			if ( iPuntoPerfilAntiguo <= iFinPerfil[Orientacion] && 
				 pPerfil[Orientacion][ iPuntoPerfilAntiguo ]->dYAbs >= pptoPuntosPiezaParaEncajar[ ppzaPiezaParaEncajar->Datos[Orientacion].uPuntoYMayorXMenor ].dYAbs )
				do {
					//Se supone que la pieza la colocó abajo y a la derecha del perfil
					//A esta altura el perfil pPerfilAux contiene sólo los puntos de la pieza a la derecha del perfil
					//Y ahora se completa el perfil pPerfilAux1 con el resto de los puntos del perfil pPerfil que están sobre los puntos de la pieza derecha
					pPerfilAux1[ iPuntoPerfilNuevo ] = pPerfil[Orientacion][ iPuntoPerfilAntiguo ];
					iPuntoPerfilNuevo++;
					iPuntoPerfilAntiguo++;

				} while ( iPuntoPerfilAntiguo <= iFinPerfil[Orientacion] );
		}//End else
	}//End while
	iFinPerfil[Orientacion] = iPuntoPerfilNuevo - 1;

	pPerfilAux2 = pPerfil[Orientacion];
	pPerfil[Orientacion] = pPerfilAux1; //Actualiza la variable pPerfil con el nuevo perfil almacenado en pPerfilAux1
	pPerfilAux1 = pPerfilAux2;

	//Trata de sacar del perfil los puntos que no aporten área (puntos sobrepuestos)
	//Si el último punto (el más arriba) tiene misma altura que el penúltimo, elimina del perfil el último punto.
	if ( pPerfil[Orientacion][ iFinPerfil[Orientacion] ]->dYAbs == pPerfil[Orientacion][ iFinPerfil[Orientacion] - 1 ]->dYAbs )
		iFinPerfil[Orientacion]--;

	//Si puntos de perfil iguales (porque se sobreponen al unir la pieza con el perfil) => se saca uno de los dos puntos del perfil (se saca el primero de abajo hacia arriba)
	if ( iFinPerfil[Orientacion] >= 3 ){
		for ( iPuntoPerfil = iInicioPerfil[Orientacion]; iPuntoPerfil + 2 <= iFinPerfil[Orientacion]; iPuntoPerfil++ ){
			if ( pPerfil[Orientacion][ iPuntoPerfil ]->dYAbs == pPerfil[Orientacion][ iPuntoPerfil + 1 ]->dYAbs &&
				 ( pPerfil[Orientacion][ iPuntoPerfil ]->dXAbs == pPerfil[Orientacion][ iPuntoPerfil + 1 ]->dXAbs ||
				   pPerfil[Orientacion][ iPuntoPerfil ]->dYAbs == pPerfil[Orientacion][ iPuntoPerfil + 2 ]->dYAbs ) ){
				for ( i = iPuntoPerfil + 1; i < iFinPerfil[Orientacion]; i++ )
					pPerfil[Orientacion][ i ] = pPerfil[Orientacion][ i + 1 ];
				iFinPerfil[Orientacion]--;
				iPuntoPerfil--;
			}//End if
		}//End for
	}//End if

	//Obtiene iPuntoMasIzquierdaAbajo : Es el punto más a la izquierda (con menor X de todos los puntos). 
	//Si hay un punto con igual X que el más a la izquierda, se toma el con menor Y. 
	iPuntoMasIzquierdaAbajo[Orientacion] = iInicioPerfil[Orientacion];
	dMaximoX = 0.0;
	for ( iPuntoPerfil = iInicioPerfil[Orientacion]; iPuntoPerfil <= iFinPerfil[Orientacion]; iPuntoPerfil++ ){
		if ( pPerfil[Orientacion][ iPuntoPerfil ]->dXAbs < pPerfil[Orientacion][ iPuntoMasIzquierdaAbajo[Orientacion] ]->dXAbs )
			iPuntoMasIzquierdaAbajo[Orientacion] = iPuntoPerfil;
		else if ( pPerfil[Orientacion][ iPuntoPerfil ]->dXAbs == pPerfil[Orientacion][ iPuntoMasIzquierdaAbajo[Orientacion]  ]->dXAbs &&
				  pPerfil[Orientacion][ iPuntoPerfil ]->dYAbs < pPerfil[Orientacion][ iPuntoMasIzquierdaAbajo[Orientacion]  ]->dYAbs )
			iPuntoMasIzquierdaAbajo[Orientacion] = iPuntoPerfil;
		if ( pPerfil[Orientacion][ iPuntoPerfil ]->dXAbs > dMaximoX )
			dMaximoX = pPerfil[Orientacion][ iPuntoPerfil ]->dXAbs;
//		if ( gen==maxgen )
//			printf("[%d](%f,%f);",iPuntoPerfil,pPerfil[Orientacion][ iPuntoPerfil ]->dXAbs,pPerfil[Orientacion][ iPuntoPerfil ]->dYAbs);
	}//End for
//	if ( gen==maxgen ){
//		printf("\n[%d]\n",iPuntoMasIzquierdaAbajo[Orientacion]);
//		getchar();
//	}
	return( dMaximoX );
}//End app_ActualizaPerfil_ir

TEval app_funceval_ir(unsigned Orientacion, double dMaximoX, double dAreaPiezasEncajadas)
// Función de evaluación: Determina el área que ocupa el perfil...
{
	double dAreaPerfil;
	int	iPuntoPerfil;

	TEval Eval;
	
	dAreaPerfil = 0.0;
	for ( iPuntoPerfil = iInicioPerfil[Orientacion]; iPuntoPerfil < iFinPerfil[Orientacion]; iPuntoPerfil++ ){
		dAreaPerfil = dAreaPerfil +
					  ( pPerfil[Orientacion][ iPuntoPerfil ]->dXAbs + pPerfil[Orientacion][ iPuntoPerfil + 1 ]->dXAbs ) *
					  ( pPerfil[Orientacion][ iPuntoPerfil + 1 ]->dYAbs - pPerfil[Orientacion][ iPuntoPerfil ]->dYAbs );
		// Escribe en archivo de Estaditicas para problema particular
		//fprintf(rprofp, "pPerfil[x]->dXAbs(%f); pPerfil[x]->dYAbs(%f)   pPerfil[x+1]->dXAbs(%f); pPerfil[x+1]->dYAbs(%f)\n", pPerfil[Orientacion][ iPuntoPerfil ]->dXAbs, pPerfil[Orientacion][ iPuntoPerfil ]->dYAbs, pPerfil[Orientacion][ iPuntoPerfil + 1 ]->dXAbs, pPerfil[Orientacion][ iPuntoPerfil + 1 ]->dYAbs);
	}
	dAreaPerfil = dAreaPerfil / 2.0;
	
	//Se trata de minimizar la diferencia entre el área del perfil y el área que ocupan las piezas, así como que las 
	//piezas se ubique lo más a la izquierda posible.
	//Eval.perdida = ( dAreaPerfil - dAreaPiezasEncajadas ) ; //+ (dMaximoX * dAltoLamina) ;
	//Eval.perdida = dMaximoX ;
	Eval.perdida = (dMaximoX * dAltoLamina) - dAreaPiezasEncajadas;
	Eval.var1    = dMaximoX; //Variable donde se almacena largo máximo de la lámina...	
	Eval.piezas  = uCantidadPiezas;
	
	Eval.n_perdidas = 0; //Variable no usada
	//Eval.areaocupada = dAreaPerfil;
    //Eval.calidad = (dAreaPiezasEncajadas / dAreaPerfil) ;
	Eval.areaocupada = dAreaPiezasEncajadas;
    Eval.calidad = (dAreaPiezasEncajadas / (dMaximoX * dAltoLamina)) ;
	return Eval; 
}//End app_funceval_ir

void app_objfunc_ir(struct individual *critter)
// Función Objetivo, transforma el string cromosoma en un arreglo de piezas
{
	double dValorObjetivo[ 4 ];
	double dXMax[4];
    unsigned i, Orientacion;
    TEval Eval_tmp;
 
 	//*** NEW
   	unsigned 	mask=1;
   	unsigned 	tp;
   	int 		j, k, z, y, stop, CantidadBitsLeido, NumeroDeLaPieza, primera_pieza_que_no_existe;
	unsigned 	bitpos, ok_pieza_encontrada;
 	//***

	dAreaPiezasEncajadas = 0.0;

	//*** NEW
	CantidadBitsLeido = 0;
	NumeroDeLaPieza = 0;
	i = 0;
   	
   	//PASO 1 : Transforma el valor del grupo de bits en un número de pieza...
   	for(k = 0; k < chromsize; k++) {
    	if(k == (chromsize-1))
         	stop = lchrom-(k*UINTSIZE);
      	else
         	stop = UINTSIZE;
	   	tp  = critter->chrom[k];
      	for(j = 0; j < stop; j++) {
         	bitpos = j + UINTSIZE*k;
         	// Entra al if si el bit actual es 1
         	if(tp&mask) {
         		NumeroDeLaPieza = NumeroDeLaPieza + pow(2, CantidadBitsLeido);	
			}//End if
         	tp = tp>>1;
         	CantidadBitsLeido++;
         	if (CantidadBitsLeido == CantidadBitsxPieza) {
         		critter->pusListaPiezas[i] = NumeroDeLaPieza%uCantidadPiezas; //graba el módulo
         		i++;
         		CantidadBitsLeido = 0;
         		NumeroDeLaPieza = 0;
         	}//End if
      	}//End for
   	}//End for 
	
	//PASO 2: Determina las piezas repetidas y asigna la primera disponible sobre el valor que se encuentra disponible.
	primera_pieza_que_no_existe = 0;
	for ( i = 1; i < uCantidadPiezas; i++ ){
		for ( k = 0; k < i; k++ ){
			//Busco desde la izquierda a la derecha si la pieza i ya existe en el lado izquierdo 
			if(critter->pusListaPiezas[k] == critter->pusListaPiezas[i]){
				//NumeroDeLaPieza ya existe => debo cambiarla por la primera que no exista en pusListaPiezas(desde 0 a uCantidadPiezas)
				for ( y = primera_pieza_que_no_existe; y < uCantidadPiezas; y++ ){
					ok_pieza_encontrada = 0;
					for ( z = 0; z < uCantidadPiezas; z++ ){
						if(critter->pusListaPiezas[z] == y){
							ok_pieza_encontrada = 1;
							z = uCantidadPiezas;
						}//End if
					}//End for
					if(ok_pieza_encontrada==0){
						//Pieza no encontrada => la asigna
						critter->pusListaPiezas[i] = y;
						primera_pieza_que_no_existe = y + 1; // asigna la siguiente ya que <y> la asignó ahora => está asignada
						y = uCantidadPiezas;
						k = i;
					}//End if
				}//End for
			}//End if
		}//End for
	}//End for
	//***

	for ( i = 0; i < uCantidadPiezas; i++ ){
		//Obtiene número entero que identifica a la pieza seleccionada
		usPiezaParaEncajar = critter->pusListaPiezas[i];
		//En ppzaPiezaParaEncajar asigna la pieza que resulta de desplazarse en usPiezaParaEncajar desde ppzaPiezas
		ppzaPiezaParaEncajar = ppzaPiezas + usPiezaParaEncajar;
		//Suma el Area de la pieza seleccionada
		dAreaPiezasEncajadas = dAreaPiezasEncajadas + ppzaPiezaParaEncajar->dArea;
		if ( i == 0 ){ //Encaje de la primera pieza
			//Para la primera pieza y por cada Orientacion obtiene el perfil y calcula el área del perfil y el ancho máx que ocupan las piezas
			for ( Orientacion = 0; Orientacion <= uMaximoRotaciones; Orientacion++ ){
				pptoPuntosPiezaParaEncajar = ppzaPiezaParaEncajar->pptoPuntos[Orientacion];
				dXMax[Orientacion] = app_EncajePrimeraPieza_ir(Orientacion);
				Eval_tmp = app_funceval_ir( Orientacion, dXMax[ Orientacion ], dAreaPiezasEncajadas );
				dValorObjetivo[Orientacion] = Eval_tmp.perdida;
			}//End for
			//Determina qué Orientacion de pieza tiene menor dValorObjetivo
			for ( Orientacion = 0; Orientacion <= uMaximoRotaciones; Orientacion++ ){
				if ( dValorObjetivo[Orientacion] <= dValorObjetivo[0] &&
					 dValorObjetivo[Orientacion] <= dValorObjetivo[1] &&
					 ( uMaximoRotaciones == 1 || ( dValorObjetivo[Orientacion] <= dValorObjetivo[2] && 
					 ( uMaximoRotaciones == 2 || dValorObjetivo[Orientacion] <= dValorObjetivo[3] ) ) ) ){				
					ppzaPiezaParaEncajar->iOrientacion = Orientacion;
					break;
				}//End if 
			}//End for
			//Para la Orientacion con función objetivo mínima se actualizan todos los perfiles con los mismos datos del
			//Perfil donde la Orientacion tuvo función objetivo mínima
			for ( Orientacion = 0; Orientacion <= uMaximoRotaciones; Orientacion++ ){
				dXMax[ Orientacion ] = dXMax[ ppzaPiezaParaEncajar->iOrientacion ];
				dMaximoX = dXMax[ ppzaPiezaParaEncajar->iOrientacion ];
				memcpy( pPerfil[ Orientacion ], pPerfil[ ppzaPiezaParaEncajar->iOrientacion], ( iFinPerfil[ ppzaPiezaParaEncajar->iOrientacion] + 1 ) * sizeof( pPerfil[ ppzaPiezaParaEncajar->iOrientacion ] ) );
				iFinPerfil[ Orientacion ] = iFinPerfil[ ppzaPiezaParaEncajar->iOrientacion ];
				iPuntoMasIzquierdaAbajo[Orientacion] = iPuntoMasIzquierdaAbajo[ppzaPiezaParaEncajar->iOrientacion];
			}//End for
		} else{ //Resto de las piezas
			//Va a probar de poner la pieza en todas las Orientaciones 
			for ( Orientacion = 0; Orientacion <= uMaximoRotaciones; Orientacion++ ){
				//Obtiene los puntos dado una Orientacion
				pptoPuntosPiezaParaEncajar = ppzaPiezaParaEncajar->pptoPuntos[Orientacion];
				//Determina si es posible encajar la pieza de acuerdo a su altura en el último punto (el más arriba) del perfil
				if ( dAltoLamina - pPerfil[Orientacion][ iFinPerfil[Orientacion] ]->dYAbs >= ppzaPiezaParaEncajar->Datos[Orientacion].dAltura ){
					//Es posible encajar la pieza sobre el perfil
					// Encaje en banda : la pieza será colocada arriba del perfil : Se mueve la pieza hasta el punto más arriba del Perfil
					app_EncajeEnBanda_ir(Orientacion);
				}else {
					//No es posible encajar la pieza encima del perfil la trata de colocar a la derecha del perfil
					//Determinar posicion inicial a la derecha del perfil
					app_DeterminaPosicionInicial_ir(Orientacion,dXMax[Orientacion]);
					//Busqueda de la distancia a desplazar la pieza hacia el encaje
					app_BuscaDistanciaDesplazamiento_ir(Orientacion);
					//Desplazar la pieza nueva en la distancia hallada
					app_DesplazaPieza_ir();
				}//End else
				/*Actualiza perfil*/
				dXMax[Orientacion] = app_ActualizaPerfil_ir(Orientacion);
				Eval_tmp = app_funceval_ir(Orientacion,dXMax[Orientacion], dAreaPiezasEncajadas);
				dValorObjetivo[Orientacion] = Eval_tmp.perdida;
			}//End for
			//Determina qué Orientacion de pieza tiene menor dValorObjetivo
			for ( Orientacion = 0; Orientacion <= uMaximoRotaciones; Orientacion++ ){
				if ( dValorObjetivo[Orientacion] <= dValorObjetivo[0] &&
					 dValorObjetivo[Orientacion] <= dValorObjetivo[1] &&
					 ( uMaximoRotaciones == 1 || ( dValorObjetivo[Orientacion] <= dValorObjetivo[2] &&
					 ( uMaximoRotaciones == 2 || dValorObjetivo[Orientacion] <= dValorObjetivo[3] ) ) ) ){
					ppzaPiezaParaEncajar->iOrientacion = Orientacion;
					break;
				}//End if
			}//End for
			//Para la Orientacion con función objetivo mínima se actualizan todos los perfiles con los mismos datos del
			//Perfil donde la Orientacion tuvo función objetivo mínima
			for ( Orientacion = 0; Orientacion <= uMaximoRotaciones; Orientacion++ ){
				dXMax[ Orientacion ] = dXMax[ ppzaPiezaParaEncajar->iOrientacion ];
				dMaximoX = dXMax[ ppzaPiezaParaEncajar->iOrientacion ];
				memcpy( pPerfil[ Orientacion ], pPerfil[ ppzaPiezaParaEncajar->iOrientacion], ( iFinPerfil[ ppzaPiezaParaEncajar->iOrientacion] + 1 ) * sizeof( pPerfil[ ppzaPiezaParaEncajar->iOrientacion ] ) );
				iFinPerfil[ Orientacion ] = iFinPerfil[ ppzaPiezaParaEncajar->iOrientacion ];
				iPuntoMasIzquierdaAbajo[Orientacion] = iPuntoMasIzquierdaAbajo[ppzaPiezaParaEncajar->iOrientacion];
			}//End for
		}//End else
		
		//Imprime puntos en archivo texto
		//pptoPuntosPiezaParaEncajar = ppzaPiezaParaEncajar->pptoPuntos[ppzaPiezaParaEncajar->iOrientacion];
		//for ( usPuntoPiezaParaEncajar = 0; usPuntoPiezaParaEncajar < ppzaPiezaParaEncajar->usCantidadPuntos;
		//	usPuntoPiezaParaEncajar++ ){
		//   	// Escribe en archivo de Estaditicas para problema particular
		//	fprintf(rprofp, "Pieza %d, Punto %d, X = %f, Y = %f\n", critter->pusListaPiezas[i], i, 
		//			pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dXAbs,
		//			pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dYAbs);
		//}//End for
		
	}//End for
    critter->PEval = app_funceval_ir(0,dMaximoX, dAreaPiezasEncajadas);
    critter->fitness = (float) critter->PEval.perdida;
    
    //fprintf(rprofp, "Fitness = %f\n",critter->fitness);
}//End app_objfunc_ir

void app_genera_resultados_problema_ir(int corrida, int tipo_problema, char *nombrearchivo)
//Función que imprime los resultados del problema corte de pieza irregular
{
	if(encabezado_resultado_problema == 0) {
      	//IMPRIME ENCABEZADO DE LOS RESULTADOS DEL PROBLEMA
      	fprintf(rprofp,"CORRIDA;TIPO_PROBLEMA;ARCHIVO_PROBLEMA;CANTIDAD_PIEZAS;CANTIDAD_TIPOS_PIEZAS;");
      	fprintf(rprofp,"ANCHO_LAMINA;LARGO_LAMINA;");
      	fprintf(rprofp,"MS_CALIDAD;MS_FITNESS;");
      	fprintf(rprofp,"MS_AREA_OCUPADA;MS_NUMERO_PIEZAS_COLOCADAS;");
      	fprintf(rprofp,"GENERACION_OCURRENCIA\n");
      	encabezado_resultado_problema = 1;//Con ésto no imprime más el encabezado
    }//End if 
	//IMPRIME RESULTADOS DEL PROBLEMA
  	fprintf(rprofp,"%d;%d;%s;%d;%d;%lf;%lf;%f;%f;%f;%d;%d\n",  
  	//CORRIDA;TIPO_PROBLEMA;ARCHIVO_PROBLEMA;CANTIDAD_PIEZAS;CANTIDAD_TIPOS_PIEZAS;ANCHO_LAMINA;LARGO_LAMINA;
  			corrida, tipo_problema, nombrearchivo, uCantidadPiezas, cantidadtipospiezas, bestfit.PEval.var1, dAltoLamina, 
  	//MS_CALIDAD;MS_FITNESS;		
  			(100 * bestfit.PEval.calidad), bestfit.fitness,
	//MS_AREA_OCUPADA;MS_NUMERO_PIEZAS_COLOCADAS;GENERACION_OCURRENCIA;
            bestfit.PEval.areaocupada, bestfit.PEval.piezas, bestfit.generation);
}//End app_genera_resultados_problema_ir

