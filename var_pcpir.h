/*---------------------------------------------------------------------------------------*/
/* var_pcpir.h - variables globales del Problema de Corte de Piezas Irrestricto          */
/*---------------------------------------------------------------------------------------*/

#ifndef VAR_PCPIR_H_
#define VAR_PCPIR_H_

double 		dAltoLamina; 		//Altura fija de la lámina
double 		dAnchoLamina; 		//Ancho fijo de la lámina (caso considerando cromosoma binario)
double 		dAreaTotalPiezas;	//Área total que ocupan las piezas
unsigned	uCantidadPiezas;	//Cantidad total de piezas de la instancia
unsigned	uMaximoRotaciones;	//Cantidad de Rotaciones a realizar
unsigned 	cantidadtipospiezas;// Variable que almacena la cantidad de 
								// de tipos de piezas del problemas		

//*** NEW
double 		CantidadBitsxPieza; //Variable que almacena la cantidad de bits a usar para poder decodificar la cantidad de piezas del problema
//***

typedef struct {
	double dXAbs;
	double dYAbs; 
	double dXRel;
	double dYRel;
} PUNTO;

typedef struct {
	unsigned		uPuntoXMenor;
	unsigned		uPuntoYMenorXMenor;
	unsigned		uPuntoYMayorXMenor;
	unsigned		uPuntoYMenorXMayor;
	unsigned		uPuntoYMayorXMayor;
	double			dAltura;
} DATOSPIEZA;

typedef struct {
	unsigned short	usCantidadPuntos;
	double			dArea;
	int				iOrientacion;
	PUNTO			*pptoPuntos[ 4 ];
	DATOSPIEZA		Datos[ 4 ];
} PIEZA;

PIEZA	*ppzaPiezas;			//Puntero donde se almacena el arreglo de piezas
PUNTO	**pPerfil[ 4 ];			//Almacena Perfil
PUNTO	**pPerfilAux1;
PUNTO	**pPerfilAux2;

int	iInicioPerfil[4];
int	iFinPerfil[4];
int iPuntoMasIzquierdaAbajo[4];
double dMaximoX;
double dgXMayor;
double dgAreaPerfil;

#define TRUE 1
#define FALSE 0

typedef unsigned short BOOLEAN;

#endif /*VAR_PCPIR_H_*/
