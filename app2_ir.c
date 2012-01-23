/*--------------------------------------------------------------------*/
/* app2_ir.c - rutinas para Problema de Corte de Piezas Irregulares   */ 
/* 			   manejado con cromosoma con ENTEROS         			  */
/*--------------------------------------------------------------------*/

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

double app2_EncajePrimeraPieza_ir(unsigned Orientacion)
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
}//End app2_EncajePrimeraPieza_ir

void app2_EncajeEnBanda_ir(unsigned Orientacion)
//Función que Encaja en banda: La pieza será colocada arriba del Perfil
{
	//Cada punto de pptoPuntosPiezaParaEncajar (la pieza a encajar) se sube hasta el último punto (el más arriba) del Perfil
	for ( usPuntoPiezaParaEncajar = 0; usPuntoPiezaParaEncajar < ppzaPiezaParaEncajar->usCantidadPuntos;
		usPuntoPiezaParaEncajar++ ){
		pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dXAbs = pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dXRel;
		pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dYAbs = pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dYRel
			+ pPerfil[Orientacion][ iFinPerfil[Orientacion] ]->dYAbs;
	}//End for
}//End app2_EncajeEnBanda_ir

void app2_DeterminaPosicionInicial_ir(unsigned Orientacion, double dMax)
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
}//End app2_DeterminaPosicionInicial_ir

double app2_dObtieneDistanciaPuntoLado_ir( double xP, double yP,	double xA, double yA, double xB, double yB )
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
}//End app2_dObtieneDistanciaPuntoLado_ir

void app2_BuscaDistanciaDesplazamiento_ir(unsigned Orientacion)
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
				dDistanciaAuxiliar = app2_dObtieneDistanciaPuntoLado_ir(
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
				dDistanciaAuxiliar = app2_dObtieneDistanciaPuntoLado_ir(
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
}//End app2_BuscaDistanciaDesplazamiento_ir

void app2_DesplazaPieza_ir(void)
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
}//End app2_DesplazaPieza_ir

double app2_ActualizaPerfil_ir(unsigned Orientacion)
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
}//End app2_ActualizaPerfil_ir

void app2_genera_layout_ir(int tipo_problema, char *nombrearchivo, cpu_consummed_time_t *t, int run_actual)
//Función que genera el layout de salida
{
	char strtot[32768], straux[100];
	
	//Primero coloca parámetros importantes 
	fprintf(outfp,"25 %d %lf %lf\n", uCantidadPiezas, bestfit.PEval.var1, dAltoLamina); //25 líneas => 24 parámetros + 1 (fin de parámetros) 
  	strtot[0] = 0;
    if(tipo_problema == 2) 
    	sprintf(straux,"TIPO_PROBLEMA                  : %s\n", "P.C.P Irregulares");
    else 
    	sprintf(straux,"TIPO_PROBLEMA                  : %s\n", "NO DEFINIDO...");
    strcat(strtot, straux);
	sprintf(straux,"ARCHIVO_PROBLEMA               : %s\n", nombrearchivo);
	strcat(strtot, straux);
  	sprintf(straux,"CORRIDA EN ARCHIVO REPORTE     : %d\n", run_actual);
  	strcat(strtot, straux);
	sprintf(straux,"CANTIDAD_PIEZAS                : %d\n", uCantidadPiezas);
	strcat(strtot, straux);
	sprintf(straux,"CANTIDAD_TIPOS_PIEZAS          : %d\n", cantidadtipospiezas);
	strcat(strtot, straux);
	sprintf(straux,"ANCHO_LAMINA (unidades)        : %lf\n", bestfit.PEval.var1);
	strcat(strtot, straux);
	sprintf(straux,"LARGO_LAMINA (unidades)        : %lf\n", dAltoLamina);
	strcat(strtot, straux);
	sprintf(straux,"MAXIMO_GENERACIONES            : %d\n", maxgen);
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
	sprintf(straux,"MS_AREA_OCUPADA (unidades2)    : %f\n", bestfit.PEval.areaocupada);
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
	fprintf(outfp,"%s@\n", strtot);

	//Finalmente detalla pieza a pieza
	//fprintf(outfp,"  TIPO_PIEZA\t   POS_X_INI\t   POS_Y_INI\t   POS_X_FIN\t   POS_Y_FIN\tESTADO_PIEZA\tAREA_OCUPADA\t\n");
	//while(Bus2 != NULL) {
   	//	fprintf(outfp,"%12d\t%12d\t%12d\t%12d\t%12d\t%12c\t%12d\t\n",
	//   	Bus2->pieza, Bus2->xini, Bus2->yini, Bus2->xini+Bus2->ancho, Bus2->yini+Bus2->alto,
	//	Bus2->tipo, Bus2->ancho * Bus2->alto);
  	//	Bus2 = Bus2->prox;
 	//}//End while	
  	//fprintf(outfp,"\n");
}//End app2_genera_layout_ir

TEval app2_funceval_ir(unsigned Orientacion, double dMaximoX, double dAreaPiezasEncajadas)
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
	//Eval.perdida = ( dAreaPerfil - dAreaPiezasEncajadas ); //+ (dMaximoX * dAltoLamina) ;
	//Eval.perdida = dMaximoX ;
	Eval.perdida = (dMaximoX * dAltoLamina) - dAreaPiezasEncajadas;
	Eval.var1    = dMaximoX; //Variable donde se almacena largo máximo de la lámina...	
	Eval.piezas  = uCantidadPiezas;
	
	Eval.n_perdidas = 0; //Variable no usada
	//Eval.areaocupada = dAreaPerfil;
    //Eval.calidad = 100 * (dAreaPiezasEncajadas / dAreaPerfil) ;
    Eval.areaocupada = dAreaPiezasEncajadas;
    Eval.calidad = (dAreaPiezasEncajadas / (dMaximoX * dAltoLamina)) ;
    
	return Eval; 
}//End app2_funceval_ir

void app2_objfunc_ir(int tipo_problema, char *nomarch, struct bestever *bestcritter, int run_actual)
// Función Objetivo, transforma el string cromosoma en un arreglo de piezas
{
	double dValorObjetivo[ 4 ];
	double dXMax[4];
    unsigned i, Orientacion;
    TEval Eval_tmp;
 
    // LLENA archivo de layout para el problema particular
	app2_genera_layout_ir(tipo_problema, nomarch, &(time_consumation), run_actual);

	dAreaPiezasEncajadas = 0.0;
	for ( i = 0; i < uCantidadPiezas; i++ ){
		//Obtiene número entero que identifica a la pieza seleccionada
		usPiezaParaEncajar = bestcritter->pusListaPiezas[i];
		//En ppzaPiezaParaEncajar asigna la pieza que resulta de desplazarse en usPiezaParaEncajar desde ppzaPiezas
		ppzaPiezaParaEncajar = ppzaPiezas + usPiezaParaEncajar;
		//Suma el Area de la pieza seleccionada
		dAreaPiezasEncajadas = dAreaPiezasEncajadas + ppzaPiezaParaEncajar->dArea;
		if ( i == 0 ){ //Encaje de la primera pieza
			//Para la primera pieza y por cada Orientacion obtiene el perfil y calcula el área del perfil y el ancho máx que ocupan las piezas
			for ( Orientacion = 0; Orientacion <= uMaximoRotaciones; Orientacion++ ){
				pptoPuntosPiezaParaEncajar = ppzaPiezaParaEncajar->pptoPuntos[Orientacion];
				dXMax[Orientacion] = app2_EncajePrimeraPieza_ir(Orientacion);
				Eval_tmp = app2_funceval_ir( Orientacion, dXMax[ Orientacion ], dAreaPiezasEncajadas );
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
					app2_EncajeEnBanda_ir(Orientacion);
				}else {
					//No es posible encajar la pieza encima del perfil la trata de colocar a la derecha del perfil
					//Determinar posicion inicial a la derecha del perfil
					app2_DeterminaPosicionInicial_ir(Orientacion,dXMax[Orientacion]);
					//Busqueda de la distancia a desplazar la pieza hacia el encaje
					app2_BuscaDistanciaDesplazamiento_ir(Orientacion);
					//Desplazar la pieza nueva en la distancia hallada
					app2_DesplazaPieza_ir();
				}//End else
				/*Actualiza perfil*/
				dXMax[Orientacion] = app2_ActualizaPerfil_ir(Orientacion);
				Eval_tmp = app2_funceval_ir(Orientacion,dXMax[Orientacion], dAreaPiezasEncajadas);
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
		pptoPuntosPiezaParaEncajar = ppzaPiezaParaEncajar->pptoPuntos[ppzaPiezaParaEncajar->iOrientacion];
		for ( usPuntoPiezaParaEncajar = 0; usPuntoPiezaParaEncajar < ppzaPiezaParaEncajar->usCantidadPuntos;
			usPuntoPiezaParaEncajar++ ){
		   	// Escribe en archivo de Estaditicas para problema particular
			fprintf(outfp, "Pieza %d, Punto %d, X = %f, Y = %f\n", bestcritter->pusListaPiezas[i], i, 
					pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dXAbs,
					pptoPuntosPiezaParaEncajar[ usPuntoPiezaParaEncajar ].dYAbs);
		}//End for
	}//End for
    bestcritter->PEval = app2_funceval_ir(0,dMaximoX, dAreaPiezasEncajadas);
    bestcritter->fitness = (float) bestcritter->PEval.perdida;
    
    //fprintf(outfp, "Fitness = %f, Tiempo Total = %f\n",bestcritter->fitness, time_consumation.elapsed_time); 
    
    fprintf(outfp,"\n");
    
}//End app2_objfunc_ir

