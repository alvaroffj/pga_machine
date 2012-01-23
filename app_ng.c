/*----------------------------------------------------------------------------*/
/* app_ng.c - rutinas dependientes de la aplicación                           */
/*----------------------------------------------------------------------------*/
/*       PROBLEMA CORTE DE PIEZA NO-GUILLOTINA BIDIMENSIONAL RESTRICTO        */
/*----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <math.h>

#include "memory.h"
#include "sga.h"
#include "var_pcp.h"
 
int AreaNodoAPCompara_ng(TNodoAP *Nodoi, TNodoAP *Nodoj)
//Función que compara Nodos AP utilizando como comparación el área de cada pieza (ancho x alto)
{
   	if((Nodoi->ancho * Nodoi->alto) > (Nodoj->ancho * Nodoj->alto))
      	return(1);
   	if((Nodoi->ancho * Nodoi->alto) < (Nodoj->ancho * Nodoj->alto))
      	return(-1);
   	return(0);
}//End AreaNodoAPCompara

int HorizontalNodoAPCompara_ng(TNodoAP *Nodoi, TNodoAP *Nodoj)
//Función que compara Nodos AP utilizando como comparación el ancho de cada pieza
{
   	if(Nodoi->ancho > Nodoj->ancho)
      	return(1);
   	if(Nodoi->ancho < Nodoj->ancho)
      	return(-1);
   	return(0);
}//End HorizontalNodoAPCompara

int VerticalNodoAPCompara_ng(TNodoAP *Nodoi, TNodoAP *Nodoj)
//Función que compara Nodos AP utilizando como comparación el alto de cada pieza
{
   	if(Nodoi->alto > Nodoj->alto)
      	return(1);
   	if(Nodoi->alto < Nodoj->alto)
      	return(-1);
   	return(0);
}//End VerticalNodoAPCompara

void ordena_piezas_problema_ng(void)
//Obtiene Arreglo con todas las Piezas del Problema
{
   	int i, j, k=0;

#define ORDEN_x_NINGUNO          0
#define ORDEN_x_AREA             1
#define ORDEN_x_LADO_HORIZONTAL  2
#define ORDEN_x_LADO_VERTICAL    3
#define TIPO_ORDEN               ORDEN_x_AREA

   	//Solicita memoria para piezasdistintas
   	if((piezasproblema = (TNodoAP*) malloc(NumPie*sizeof(TNodoAP))) == NULL)
      	nomemory("piezasproblema en ordena_piezas_problema_ng");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, App_ng.c, 237, piezasproblema, %d\n", NumPie*sizeof(TNodoAP));
#endif

#if (TIPO_ORDEN == ORDEN_x_NINGUNO)
   	//No hay ordenamiento
#elif(TIPO_ORDEN == ORDEN_x_AREA)
   	//Ordenamiento Qsort por Area de cada pieza
   	//Deja primero las piezas con area menor en forma ascendente con respecto al area de c/pieza
   	qsort(piezasdistintas, cantidadtipospiezas + 1, sizeof(TNodoAP), (void *) AreaNodoAPCompara_ng);
#elif(TIPO_ORDEN == ORDEN_x_LADO_HORIZONTAL)
   	//Ordenamiento Qsort horizontal (considera el ancho de cada pieza)
   	//Deja primero las piezas con ancho menor en forma ascendente con respecto al ancho de c/pieza
   	qsort(piezasdistintas, cantidadtipospiezas + 1, sizeof(TNodoAP), (void *) HorizontalNodoAPCompara_ng);
#elif(TIPO_ORDEN == ORDEN_x_LADO_VERTICAL)
   	//Ordenamiento Qsort Vertical (considera el alto de cada pieza)
   	//Deja primero las piezas con alto menor en forma ascendente con respecto al alto de c/pieza
   	qsort(piezasdistintas, cantidadtipospiezas + 1, sizeof(TNodoAP), (void *) VerticalNodoAPCompara_ng);
#endif

   	// Una vez ordenadas las piezas, se leen de mayor a menor.
   	// Como el ordenamiento es de menor a mayor hay que leer las piezas de atras para adelante
   	for(i=cantidadtipospiezas;i>0;i--) {
   		for(j=1;j<=piezasdistintas[i].cantidadpiezas;j++) {
      		piezasproblema[k].ancho          = piezasdistintas[i].ancho;
      		piezasproblema[k].alto           = piezasdistintas[i].alto;
      		piezasproblema[k].numero         = piezasdistintas[i].numero;
      		piezasproblema[k].cantidadpiezas = 1;
     		k++;
   		}//End for
   	}//End for
}//End ordena_piezas_problema

int app_leearchivo_ng(char *nombrearchivo, int rank_actual)
//Función principal de lectura del archivo
{
   	FILE *fp;
   	int i, num, alt, anc, lim, id=1, estado_arch;
   	char nombre_archivo[100];

   	if(rank_actual == 0) //Lee archivo intancia de directorio intancias
      	sprintf(nombre_archivo, "%s%s", ruta_instancias, nombrearchivo);
   	else //Lee archivo desde directorio resultados
      	sprintf(nombre_archivo, "%s", nombrearchivo);
   	if((fp = fopen(nombre_archivo,"r"))== NULL){
      	fprintf(outfp,"error al leer archivo %s\n",nombrearchivo);
      	return 0;
   	}//End if

   	// Inicialización de variables globales
   	NumPie = 0;
   	cantidadtipospiezas = 0;
   	areaallpiezas = 0;

   	// Lee Ancho y Largo de la Lámina
   	fscanf(fp,"%d %d",&AnchoPl,&AltoPl);
   	// Lee cantidad de tipos de piezas del problema
   	fscanf(fp,"%d",&num);

   	//Solicita memoria para piezasdistintas
   	if((piezasdistintas = (TNodoAP*) malloc((num+1)*sizeof(TNodoAP))) == NULL)
      	nomemory("piezasdistintas en app_lecturaarchivo_ng");
#ifdef _DEBUG_MALLOC_
   	printf("Malloc, App_ng.c, 238, piezasdistintas, %d\n", (num+1)*sizeof(TNodoAP));
#endif
   
   	estado_arch = true;
   	//Inicializa primer elemento
   	piezasdistintas[0].ancho          = 0;
   	piezasdistintas[0].alto           = 0;
   	piezasdistintas[0].numero         = 0;
   	piezasdistintas[0].cantidadpiezas = 0;
   	for(i=1;i<=num;i++) {
      	// Lee ancho, alto y restricciones para cada tipo de pieza
      	if (fscanf(fp,"%d %d %d",&anc,&alt,&lim) != EOF) {
         	if ((anc > 0) && (alt > 0) && (lim > 0)) {
            	piezasdistintas[i].ancho          = anc;
            	piezasdistintas[i].alto           = alt;
            	piezasdistintas[i].numero         = id;
            	piezasdistintas[i].cantidadpiezas = lim;
            	//Incremento id si quiero que sólo cada tipo pieza tenga id distinto
            	id++;
            	NumPie=NumPie+lim;
           		areaallpiezas = areaallpiezas + (anc * alt * lim);
         	}//End if
         	else {
            	estado_arch = false; 
            	break;
         	}//End else
      	}//End if
      	else if ((i - 1) != num) {
         	estado_arch = false; //Hay menos piezas en archivo que la especificada.
         	break;
      	}//End else if
   	}//End for
   
   	//Cierra archivo de piezas
   	fclose(fp);

   	//Si archivo tiene problemas => retorna con error
   	if (estado_arch == false) return 0;
   
   	//Establece la cantidad máxima de tipos de piezas distintos del problema
   	id--;
   	cantidadtipospiezas=id;
   
   	if(NumPie == 0) return 0;
	largo_cromosoma = NumPie; //Define el largo del cromosoma
	fitness_inicial = (float) (AltoPl * AnchoPl); //Obtiene el fitness_inicial

   	// Establece valor en variables utilizadas en función evaluación
	peso_func_obj    = 0.85;// Uso en función evaluación - Factor de la pérdida
	peso_uni      	 = 0.15;// Uso en función evaluación - Factor unificación de pérdidas
	peso_perdida     = 0.5;	/*Factor de la componente perdida*/
	peso_distancia   = 0.5;	/*Factor de la componente distancia*/
	peso_digregacion = 0.0;	/*Factor de la componente digregacion*/

	//Ordena las piezas y determina arreglo piezasproblema[]
   	ordena_piezas_problema_ng();
   	return -1;
}//End app_leearchivo

TEval app_funceval_ng(TNodoAP *piezas)
// Función de evaluación
{
   	int      i,acum,cont;
   	int      Acan = 0,Acal = 0,Nuan,Nual;
   	int      PieInc = NumPie;
   	int      AreaPlaca = AnchoPl * AltoPl;
   	float    AreaOcup,UnifPerd, TAU, CONSTANTE;
   	int      siguiente=1;
   	int 		Arreglo[AnchoPl], Arr1, Arr2, Arreglo_donde_empieza, Arreglo_valor;
   	int 		Patron_sirve, Arreglo_min_altura;
   	TEval    Eval;

   	// Inicializa la Lista de Pérdidas
   	LPer = NULL;

   	// Inicializa Arreglo que contiene tantos registros como ancho tenga la placa
   	for(i=0;i<AnchoPl;i++){
      	Arreglo[i]=0;
   	}//End for

   	// Se saltan las piezas de ancho y largo = 0
   	for(i=0;i<NumPie;i++){
      	Acan = piezas[i].ancho;
      	Acal = piezas[i].alto;
      	if ((Acan != 0) && (Acal != 0) && (Acan <= AnchoPl) && (Acal <= AltoPl)) {
        	// Entra aqui si la pieza tiene dimensiones > 0 y cabe en la lámina
			//Coloca la pieza abajo a la izquierda
			for(Arr1=0;Arr1<Acan;Arr1++){
	    		Arreglo[Arr1]=Acal;
	   		}//End for
            siguiente = i+1;
        	i = NumPie;
      	}//End if 
      	else
         	PieInc--;
   	}//End for
   	if(siguiente<NumPie) {
      	for(i=siguiente;i<NumPie;i++) {
         	Nuan = piezas[i].ancho;
         	Nual = piezas[i].alto;
         	if((Nuan != 0) && (Nual != 0) && (Nuan <= AnchoPl) && (Nual <= AltoPl)) {
            	//¿ Es posible colocar la pieza en el layout ?
            	//Si es <SI> entonces se actualiza valores en vector
            	//Si es <NO> entonces se PieInc--
				Arreglo_donde_empieza=-1; 
		    	Arreglo_min_altura=AltoPl;
		    	Arreglo_valor=0;
		    	/*Con este for recorre odo el vector probando como queda la pieza en cada posición del vector*/
		    	for(Arr1=0;Arr1<AnchoPl;Arr1++){  //Por cada posiciòn del vector 
		    		if ((Arr1+Nuan) <= AnchoPl) { //Analiza si la pieza a colocar no toca con el lado derecho de la lámina		    		
						Arreglo_valor=Arreglo[Arr1]; //Asigna el valor del arreglo en esta posición  
						if (Arreglo_valor+Nual <= AltoPl) { //Analiza si la pieza a colocar no toca con el lado superior de la lámina
							Patron_sirve=-1; //-1 => Sì sirve por default
				    		for(Arr2=0;Arr2<Nuan;Arr2++){//Este for sirve para ver que todas las posiciones de la pieza da la misma altura => la pieza cabe
				    			if (Arreglo[Arr1+Arr2] != Arreglo_valor) {
				    				Patron_sirve=0;
				    				Arr2= Nuan;
				    			}//End if
				    		}//End for
				    		if (Patron_sirve == -1) {
								if (Arreglo_valor+Nual < Arreglo_min_altura) { //Va a colocar la pieza lo más abajo posible
									Arreglo_donde_empieza = Arr1; //Asigna donde va a colocar la pieza
									Arreglo_min_altura = Arreglo_valor+Nual; //actualiza la mínima altura
								}//End if
								else if ((Arreglo_valor+Nual == Arreglo_min_altura) && (Arreglo_donde_empieza == -1)) {
									Arreglo_donde_empieza = Arr1;
								}//End else if
				    		}//End if  
						}//End if
		    		}//End if
		   		}//End for
		    	if (Arreglo_donde_empieza == -1) {
		    		//Pieza no sirve
			    	PieInc--; // Pieza no sirve.
		    	}//End if
		    	else {
		    		//Pieza sirve
					for(Arr1=Arreglo_donde_empieza;Arr1<Arreglo_donde_empieza+Nuan;Arr1++){
			    		Arreglo[Arr1] = Arreglo[Arr1] + Nual;
			   		}//End for			    
		    	}//End else            
         	}//End if
         	else PieInc--; // Pieza excede el area disponible en la lámina.
      	}//End for
   	}//End if
   	//Obtiene la pérdida que es la suma de todas las diferencias entre el alto de la placa y cada altura del vector arreglo 
   	acum=0;
   	cont = 0;
   	for(Arr1=0;Arr1<AnchoPl;Arr1++){
		if (AltoPl - Arreglo[Arr1] > 0) {
			acum = acum + (AltoPl - Arreglo[Arr1]);
			cont++;
		}//End if
   	}//End for
   	CONSTANTE = 1.0;
   	TAU = CONSTANTE * (float) cantidadtipospiezas;
  
   	Eval.c_perdidareal = (float) ((float) acum/(float) AreaPlaca); //Componente Perdida
   	Eval.c_distancia   = (float) ((float) (NumPie - PieInc) /(float) NumPie); //Componente Distancia
   	//Eval.c_digregacion = (peso_digregacion * (1.0 - expf((float) (((float) -cont)/TAU)))); //Componente Digregacion
   	
   
   	/******************** MOMENTANIO *******************/
   	//Eval.c_perdidareal = (float) acum;
   	//Eval.c_perdidareal = (float) (acum + acum * (NumPie - PieInc)) ;
   	//Eval.c_perdidareal = (float) (acum) * (1.0 - expf((float) (((float) (PieInc - NumPie))/TAU)));
   	//Eval.c_distancia   = 0.0;
   	Eval.c_digregacion = 0.0;
   	/******************** MOMENTANIO *******************/

   	Eval.perdida       = acum * (peso_perdida * Eval.c_perdidareal + peso_distancia * Eval.c_distancia + peso_digregacion * Eval.c_digregacion);
   	Eval.piezas        = PieInc;
   	AreaOcup           = (float)(AreaPlaca - acum)/ (float) AreaPlaca;
   	if(cont == 0)
      	UnifPerd = 1.0;  //OJO -> Ver esto si está bien.
   	else
      	UnifPerd = (float) 1.0/(float) cont;
   	Eval.n_perdidas = cont; //Numero de Perdidas generadas
   	Eval.areaocupada = AreaOcup;
   	Eval.calidad = AreaOcup*peso_func_obj + UnifPerd*peso_uni;
   	return Eval;
}//End app_funceval

void app_objfunc_ng(struct individual *critter)
// Función Objetivo, transforma el string cromosoma en un arreglo de piezas
{
   	unsigned mask=1, tp, tp1, bitpos;
   	int j, k, stop;
   	TNodoAP *piezaschromo;// variable que apunta a todas las piezas del cromosoma generado

   	if((piezaschromo = (TNodoAP*) malloc(NumPie*sizeof(TNodoAP))) == NULL)
      	nomemory("piezaschromo en objfunc_ng");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, App_ng.c, 440, piezaschromo, %d\n", NumPie*sizeof(TNodoAP));
#endif

   	//Asigna cada pieza del arreglo piezasproblema en arreglo piezaschromo dependiendo del string chrom
   	// OJO ===> chromsize = chmutsize = NumPie
   	for(k = 0; k < chromsize; k++) {
   		if(k == (chromsize-1))
      		stop = lchrom-(k*UINTSIZE);
   		else
      		stop = UINTSIZE;
   		tp  = critter->chrom[k];
   		tp1 = critter->chmut[k];
   		for(j = 0; j < stop; j++) {
      		bitpos = j + UINTSIZE*k;
         	// Entra al if si el bit actual es 1
      		if(tp&mask) {
            	//Asigna pieza desde arreglo piezasproblema
         		if(tp1&mask) {
	               	// Efectúa rotación de piezas
	            	piezaschromo[bitpos].ancho       = piezasproblema[bitpos].alto;
	            	piezaschromo[bitpos].alto        = piezasproblema[bitpos].ancho;
	         	}//End if
	         	else {
	            	// No efectua rotacion de piezas
	            	piezaschromo[bitpos].ancho       = piezasproblema[bitpos].ancho;
	            	piezaschromo[bitpos].alto        = piezasproblema[bitpos].alto;
	         	}//End else
	         	piezaschromo[bitpos].numero          = piezasproblema[bitpos].numero;
	         	piezaschromo[bitpos].cantidadpiezas  = 1;
	      	}//End if
      		else {
            	//Asigna solo ceros => pieza no se considera
         		piezaschromo[bitpos].ancho           = 0;
         		piezaschromo[bitpos].alto            = 0;
		        piezaschromo[bitpos].numero          = 0;
        		piezaschromo[bitpos].cantidadpiezas  = 1;
      		}//End else
      		tp = tp>>1;
      		tp1 = tp1>>1;
   		}//End for
   	}//End for
   	critter->PEval = app_funceval_ng(piezaschromo);
   	critter->fitness =  (float) critter->PEval.perdida;
   	free(piezaschromo);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, app_ng.c, 512, piezaschromo\n");
#endif
}//End objfunc_ng

void app_genera_resultados_problema_ng(int corrida, int tipo_problema, char *nombrearchivo)
//Función que imprime los resultados del problema corte de pieza no guillotina
{
	if(encabezado_resultado_problema == 0) {
      	//IMPRIME ENCABEZADO DE LOS RESULTADOS DEL PROBLEMA
      	fprintf(rprofp,"CORRIDA;TIPO_PROBLEMA;ARCHIVO_PROBLEMA;CANTIDAD_PIEZAS;CANTIDAD_TIPOS_PIEZAS;");
      	fprintf(rprofp,"ANCHO_LAMINA;LARGO_LAMINA;AREA_LAMINA;AREA_PIEZAS;PESO_PERDIDA;PESO_DISTANCIA;PESO_DIGREGACION;");
      	fprintf(rprofp,"MS_CALIDAD;MS_FITNESS;MS_PERDIDA_REAL;MS_DISTANCIA;MS_DIGREGACION;");
      	fprintf(rprofp,"MS_AREA_OCUPADA;MS_NUMERO_PIEZAS_COLOCADAS;MS_NUMERO_PERDIDAS_GENERADAS;");
      	fprintf(rprofp,"GENERACION_OCURRENCIA\n");
      	encabezado_resultado_problema = 1;//Con ésto no imprime más el encabezado
    }//End if 
	//IMPRIME RESULTADOS DEL PROBLEMA
  	fprintf(rprofp,"%d;%d;%s;%d;%d;%d;%d;%d;%d;%f;%f;%f;%f;%f;%f;%f;%f;%f;%d;%d;%d\n",  
  	//CORRIDA;TIPO_PROBLEMA;ARCHIVO_PROBLEMA;CANTIDAD_PIEZAS;CANTIDAD_TIPOS_PIEZAS;ANCHO_LAMINA;LARGO_LAMINA;AREA_LAMINA;AREA_PIEZAS;
  			corrida, tipo_problema, nombrearchivo, NumPie, cantidadtipospiezas, AnchoPl, AltoPl, AnchoPl * AltoPl, areaallpiezas,
  	//PESO_PERDIDA;PESO_DISTANCIA;PESO_DIGREGACION;MS_CALIDAD;MS_FITNESS;		
  			(100.0 * peso_perdida), (100.0 * peso_distancia), (100.0 * peso_digregacion), (100.0 * bestfit.PEval.calidad), bestfit.fitness,
  	//MS_PERDIDA_REAL;MS_DISTANCIA;MS_DIGREGACION;
            (100.0 * bestfit.PEval.c_perdidareal), (100.0 * bestfit.PEval.c_distancia), (100.0 * bestfit.PEval.c_digregacion), 
	//MS_AREA_OCUPADA;MS_NUMERO_PIEZAS_COLOCADAS;MS_NUMERO_PERDIDAS_GENERADAS;GENERACION_OCURRENCIA;
            (100.0 * bestfit.PEval.areaocupada), bestfit.PEval.piezas, bestfit.PEval.n_perdidas, bestfit.generation);
}//End app_genera_resultados_problema_ng

void app_free_ng(void)
//Función que libera variables del problema
{
	free(piezasdistintas);
#ifdef _DEBUG_MALLOC_		
	printf("Free, app_ng.c, 287, piezasdistintas\n");
#endif
    free(piezasproblema);
#ifdef _DEBUG_MALLOC_		
	printf("Free, app_ng.c, 290, piezasproblema\n");
#endif
}//End app_free_ng

