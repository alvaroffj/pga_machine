/*---------------------------------------------------------------------------------------*/
/* var_pcp.h - variables globales del Problema de Corte de Pieza Bidimensional Restricto */
/*---------------------------------------------------------------------------------------*/

#ifndef VAR_PCP_H_
#define VAR_PCP_H_

int 			NumPie; 			/* Número total de piezas */
int 			cantidadtipospiezas;/* Variable que almacena la cantidad de */
									/* de tipos de piezas del problemas		*/
int 			areaallpiezas;		/* Es la suma de las áreas de todas las piezas */
int         	AltoPl,AnchoPl; 	/* Dimensiones de la placa */
int				TotalPie; 			/* Número total de piezas => Piezas Ganancias + Piezas Pérdidas */
float 			peso_func_obj;  	/* Uso en funcion evaluación - Factor de la pérdida */
float 			peso_uni;			/* Uso en funcin evaluación - Factor unificación de pérdidas */
float 			peso_perdida;		/* Factor de la componente pérdida */
float 			peso_distancia;		/* Factor de la componente distancia */
float 			peso_digregacion;	/* Factor de la componente digregación */

/* Estructuras de la aplicación */

/*Estructura para almacenar una pieza, en un arreglo*/
typedef struct nodoap
{
   	int ancho;
   	int alto;
   	int numero;
	int cantidadpiezas;
}TNodoAP;

/*Estructura para retornar una pieza de la lista de prdidas*/
typedef struct nodore
{
   int ancho;
   int alto;
}TNodoRE;

/*Estructura para la lista donde se almacenan las prdidas*/
typedef struct nodope
{
   int ancho;
   int alto;
   struct nodope *prox;
}TListaPE;

/*Estructura para retornar una pieza de la lista de piezas (Prdida o Ganancia)*/
typedef struct nodore2
{
   	int  ancho;
   	int  alto;
   	int  xini;
   	int  yini;
   	char tipo;  //P=Prdida o G=Ganacia
   	int  pieza; //Nmero de pieza utilizada
}TNodoRE2;

/*Estructura para la lista donde se almacenan las piezas (Prdida o Ganancia)*/
typedef struct nodope2
{
   	int  ancho;
  	int  alto;
  	int  xini;
   	int  yini;
   	char tipo;  //P=Pérdida o G=Ganacia
   	int  pieza; //Nmero de pieza utilizada
   	struct nodope2 *prox;
}TListaPE2;

/*Variables globales*/
TListaPE2  		*LPer2; /*Lista de Pérdidas o Ganancias*/
TListaPE    	*LPer; /*Lista de prdidas*/
TNodoAP  		*piezasdistintas;  /* variable que apunta a las piezas de la lámina    */
TNodoAP     	*piezasproblema;   /* variable que apunta a todas piezas del problema */

#endif /*VAR_PCP_H_*/
