/*----------------------------------------------------------------------------*/
/* app_g.c - rutinas dependientes de la aplicación                              */
/*----------------------------------------------------------------------------*/
/*         PROBLEMA CORTE DE PIEZA GUILLOTINA BIDIMENSIONAL RESTRICTO         */
/*----------------------------------------------------------------------------*/

#include <stdlib.h>
#include <math.h>

#include "memory.h"
#include "sga.h"
#include "var_pcp.h"


void app_liberamemlistaperdidas_g(int perdidaexterna)
//Libera memoria de la lista de pérdidas
{
   	TListaPE *LAux;
   	LAux = LPer;

   	if(LAux == NULL)  return;
   	while(LAux != NULL) {
      	LPer = LAux->prox;
      	free(LAux);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, app_g.c, 24, LAux\n");    // Libera LBusca
#endif
      	LAux = LPer;
   	}//End while
}//End app_liberamemlistaperdidas_g

void app_agregalistap_g(int anc,int alt)
// Agrega un elemento al principio de la lista de pérdidas, su parámetro
// inicial es un puntero apuntando al nodo final de la lista
{
   	TListaPE *LAux;
   
   	if((LAux = (TListaPE*) malloc(sizeof(TListaPE))) == NULL)
      	nomemory("Laux en app_agregalistap_g");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, App_g.c, 115, LAux, %d\n", sizeof(TListaPE));
#endif
   	LAux->ancho = anc;
   	LAux->alto = alt;
   	LAux->prox = LPer;
   	LPer = LAux;
} //End app_agregalistap_g

TNodoRE app_pieza2perdida_g(int anc,int alt)
// Politica de Encaje : Se escoje la menor pérdida de entre las menores...
{
   	TListaPE *LAux, *LBusca;
   	TNodoRE Ret;
   	int d_ancho_min = AnchoPl, d_alto_min = AltoPl, d_ancho_actual, d_alto_actual;
   	int  pos_nodo_best_fit = -1, j = 0;
   
   	LAux = LPer;
   	LBusca = LPer;
   	if(LBusca == NULL) {
      	Ret.ancho = -1;
      	return Ret;
   	}//End if
   	while(LAux != NULL) {  // Mientras LAux.prox no apunte a NULL
      	if((anc <= LAux->ancho) && (alt <= LAux->alto)) { //Pieza cabe en la pérdida actual ??
         	//SI, la pieza cabe en la pérdida actual
         	d_ancho_actual = LAux->ancho - anc; //Diferencia entre ancho de la pérdida actual y ancho de la pieza
         	d_alto_actual = LAux->alto - alt;   //Diferencia entre alto de la pérdida actual y alto de la pieza
         	if((d_ancho_actual == 0) && (d_alto_actual == 0)){ //Ve si pieza cabe justo en la pérdida actual (no sobra nada)
            	pos_nodo_best_fit = j; //Selecciona la pérdida j, será sacada desde la lista de pérdidas
            	break;                 //ya que fue ocupada completamente por una pieza.
         	}//End if
         	else if(d_ancho_actual == 0){ //Quiere decir que la pieza ocupó el ancho completo de la pérdida actual
            	if(d_alto_actual <= d_alto_min){
               		d_ancho_min = d_ancho_actual;
               		d_alto_min = d_alto_actual;
               		pos_nodo_best_fit = j;
            	}//End if
            	else if((d_ancho_min != 0) && (d_alto_min != 0)){
               		//Me interesa no perder la pieza actual ya que por lo menos
               		//esta calza justo por un lado, pero la d_min NO...
               		d_ancho_min = d_ancho_actual;
               		d_alto_min = d_alto_actual;
               		pos_nodo_best_fit = j;
           		}//End else if
         	}//End else if
         	else if(d_alto_actual == 0){
            	if(d_ancho_actual <= d_ancho_min){
               		d_ancho_min = d_ancho_actual;
               		d_alto_min = d_alto_actual;
              		pos_nodo_best_fit = j;
            	}//End if
            	else if((d_ancho_min != 0) && (d_alto_min != 0)){
               		//Me interesa no perder la pieza actual ya que por lo menos
               		//esta calza justo por un lado, pero la d_min NO...
               		d_ancho_min = d_ancho_actual;
               		d_alto_min = d_alto_actual;
               		pos_nodo_best_fit = j;
            	}//End else if
         	}//End else if
         	else if((d_ancho_actual < d_ancho_min) || (d_alto_actual < d_alto_min)){
            	d_ancho_min = d_ancho_actual;
            	d_alto_min = d_alto_actual;
            	pos_nodo_best_fit = j;
         	}//End else if
      	}//End if
      	j++;
      	LAux = LAux->prox;
   	}//End while

   	if(pos_nodo_best_fit == 0){
      	//Trata de calzar nueva pieza en el nodo cabeza de la lista de Pérdidas
      	Ret.ancho = LBusca->ancho; // Nueva pieza calza en primer nodo
      	Ret.alto = LBusca->alto;   // Asigna nuevo nodo
      	LPer = LBusca->prox;       // LPer queda apuntando al próximo
      	free(LBusca);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, app_g.c, 187, LBusca\n"); // Libera LBusca
#endif
      	return Ret;
   	}//End if
   	else if(pos_nodo_best_fit > 0){
      	LAux = LPer;
      	LBusca = LPer;
      	j = 1;
      	while(LAux != NULL) {  // Mientras LAux.prox no apunte a NULL
         	LAux = LBusca->prox;
         	if(j == pos_nodo_best_fit) {
            	Ret.ancho = LAux->ancho;
            	Ret.alto = LAux->alto;
            	LBusca->prox = LAux->prox;
            	free(LAux);
#ifdef _DEBUG_MALLOC_		
   	printf("Free, app_g.c, 200, LAux\n");
#endif
            	return Ret;
         	}//End if
         	LBusca = LBusca->prox;
         	j++;
      	}//End while
   	}//End elseif
   	Ret.ancho = -1; // Sale por aqui cuando hay una lista de piezas
   	return Ret;     // y la nueva pieza no calza en ninguna de las pérdidas.
}//End app_pieza2perdida_g

int AreaNodoAPCompara_g(TNodoAP *Nodoi, TNodoAP *Nodoj)
//Función que compara Nodos AP utilizando como comparación el área de cada pieza (ancho x alto)
{
   	if((Nodoi->ancho * Nodoi->alto) > (Nodoj->ancho * Nodoj->alto))
      	return(1);
   	if((Nodoi->ancho * Nodoi->alto) < (Nodoj->ancho * Nodoj->alto))
      	return(-1);
   	return(0);
}//End AreaNodoAPCompara_g

int HorizontalNodoAPCompara_g(TNodoAP *Nodoi, TNodoAP *Nodoj)
//Función que compara Nodos AP utilizando como comparación el ancho de cada pieza
{
   	if(Nodoi->ancho > Nodoj->ancho)
      	return(1);
   	if(Nodoi->ancho < Nodoj->ancho)
      	return(-1);
   	return(0);
}//End HorizontalNodoAPCompara_g

int VerticalNodoAPCompara_g(TNodoAP *Nodoi, TNodoAP *Nodoj)
//Función que compara Nodos AP utilizando como comparación el alto de cada pieza
{
   	if(Nodoi->alto > Nodoj->alto)
      	return(1);
   	if(Nodoi->alto < Nodoj->alto)
      	return(-1);
   	return(0);
}//End VerticalNodoAPCompara_g

void ordena_piezas_problema_g(void)
//Obtiene Arreglo con todas las Piezas del Problema
{
   	int i, j, k=0;
	
#define ORDEN_x_NINGUNO          0
#define ORDEN_x_AREA             1
#define ORDEN_x_LADO_HORIZONTAL  2
#define ORDEN_x_LADO_VERTICAL    3
#define TIPO_ORDEN               ORDEN_x_AREA
//#define TIPO_ORDEN               ORDEN_x_NINGUNO

   	//Solicita memoria para piezasdistintas
   	if((piezasproblema = (TNodoAP*) malloc(NumPie*sizeof(TNodoAP))) == NULL)
      	nomemory("piezasproblema en ordena_piezas_problema_g");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, App_g.c, 237, piezasproblema, %d\n", NumPie*sizeof(TNodoAP));
#endif

#if (TIPO_ORDEN == ORDEN_x_NINGUNO)
   	//No hay ordenamiento
#elif(TIPO_ORDEN == ORDEN_x_AREA)
   	//Ordenamiento Qsort por Area de cada pieza
   	//Deja primero las piezas con area menor en forma ascendente con respecto al area de c/pieza
	qsort(piezasdistintas, cantidadtipospiezas + 1, sizeof(TNodoAP), (void *) AreaNodoAPCompara_g);
#elif(TIPO_ORDEN == ORDEN_x_LADO_HORIZONTAL)
   	//Ordenamiento Qsort horizontal (considera el ancho de cada pieza)
   	//Deja primero las piezas con ancho menor en forma ascendente con respecto al ancho de c/pieza
   	qsort(piezasdistintas, cantidadtipospiezas + 1, sizeof(TNodoAP), (void *) HorizontalNodoAPCompara_g);
#elif(TIPO_ORDEN == ORDEN_x_LADO_VERTICAL)
   	//Ordenamiento Qsort Vertical (considera el alto de cada pieza)
   	//Deja primero las piezas con alto menor en forma ascendente con respecto al alto de c/pieza
   	qsort(piezasdistintas, cantidadtipospiezas + 1, sizeof(TNodoAP), (void *) VerticalNodoAPCompara_g);
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
}//End ordena_piezas_problema_g

int app_leearchivo_g(char *nombrearchivo, int rank_actual)
//Función principal de lectura del archivo
{
   	FILE *fp;
   	int i, num, alt, anc, lim, id=1, estado_arch;
   	char nombre_archivo[100];

   	if(rank_actual == 0) //Lee archivo intancia de directorio intancias
      	sprintf(nombre_archivo, "%s%s", ruta_instancias, nombrearchivo);
      	
   	else //Lee archivo desde directorio resultados, el archivo ha sido creado antes temporalmente
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
      	nomemory("piezasdistintas en app_lecturaarchivo_g");
#ifdef _DEBUG_MALLOC_
	printf("Malloc, App_g.c, 238, piezasdistintas, %d\n", (num+1)*sizeof(TNodoAP));
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
   	ordena_piezas_problema_g();
   	return -1;
}//End app_leearchivo

TEval app_funceval_g(TNodoAP *piezas)
// Función de evaluación
{
   	int      i,acum,cont;
   	int      Acan = 0,Acal = 0,Nuan,Nual;
   	int      PerdExt;
   	int      PieInc = NumPie;
   	int      AreaPlaca = AnchoPl * AltoPl;
   	float    AreaOcup,UnifPerd, TAU, CONSTANTE;
   	int      siguiente=1;
   	TEval    Eval;
   	TNodoRE  Perd;
   	TListaPE *Bus;
   	// Inicializa la Lista de Pérdidas
   	LPer = NULL;

   	// Se saltan las piezas de ancho y largo = 0
   	for(i=0;i<NumPie;i++){
      	Acan = piezas[i].ancho;
      	Acal = piezas[i].alto;
      	if ((Acan != 0) && (Acal != 0) && (Acan <= AnchoPl) && (Acal <= AltoPl)) {
         	// Entra aqui si la pieza tiene dimensiones > 0 y cabe en la lámina
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
	            Perd = app_pieza2perdida_g(Nuan,Nual);
            	if(Perd.ancho != -1) { // La pieza nueva calza en alguna pérdida dentro de la lista de pérdidas
	               	if((Nuan < Perd.ancho) && (Nual < Perd.alto)) { 
	                  	// La nueva pieza está dentro de una pérdida
	                  	// => se generan 2 nuevas pérdidas
	                  	// Define fragmento de pérdida inferior (dentro del patrón y debajo de la nueva pieza)
	                  	app_agregalistap_g(Perd.ancho,Perd.alto - Nual);
	                  	// Define fragmento de pérdida derecho (dentro del patrón y al lado de la nueva pieza)
	                  	app_agregalistap_g(Perd.ancho - Nuan,Nual);
	               	}//End if
	               	else if(Nual < Perd.alto)  
	                  	// La nueva pieza calza exacto a lo ancho de la pérdida
	                  	// y queda una pérdida inferior (por debajo de la nueva pieza)
	                  	//Agrega nueva pérdida del mismo ancho de la pérdida anterior pero de menor alto.
	                  	app_agregalistap_g(Nuan,Perd.alto - Nual);
	               	else if(Nuan < Perd.ancho)  
	                  	// La nueva pieza calza exacto a lo largo de la pérdida
	                  	// y queda una pérdida derecha (por al lado de la nueva pieza)
	                  	//Agrega nueva pérdida del mismo alto de la perdida anterior pero de menor ancho.
	                  	app_agregalistap_g(Perd.ancho - Nuan,Nual);
	            }//End if
            	else {  // Nueva pieza no calza en las pérdidas o no hay pérdidas.
               		if((Acal + Nual) <= AltoPl) {  
	                	// Determina corte horizontal,
                  		// la pieza la pone por debajo del patrón de corte
                  		if(Acan > Nuan) {  
                     		// Genera Pérdida al lado derecho de la nueva pieza
                     		// y por debajo del patrón de corte actual
                     		// Ingresa nueva Pérdida de dimensiones (crecimiento a lo alto
                     		// (hacia abajo) del patrón) x (Alto de la nueva pieza)
                     		app_agregalistap_g(Acan - Nuan,Nual);
                     		// Dimensiona nuevo alto del patrón de corte actual
                     		Acal = Acal + Nual;
                  		}//End if
	                  	else if(Nuan > Acan) {      
	                     	// Genera Pérdida al lado derecho el patrón de corte
	                     	// y por encima de la nueva pieza.
	                     	// Ingresa nueva Pérdida de dimensiones (crecimiento a lo ancho
	                     	// (hacia lado derecho) del patrón) x (Alto del patrón de corte antiguo)
	                     	app_agregalistap_g(Nuan - Acan,Acal);
	                     	// Dimensiona nuevo ancho del patrón de corte actual
	                     	Acan = Nuan;
	                     	// Dimensiona nuevo alto del patrón de corte actual
	                     	Acal = Acal + Nual;
	                  	}//End elseif
	                  	else  
	                     	// No hay pérdida,el ancho de la nueva pieza coincide exacto con el del patrón
	                     	// de corte, pero hay que actualizar el alto del nuevo patrón de corte
	                     	Acal = Acal + Nual;
               		}//End if
               		else if((Acan + Nuan) <= AnchoPl) { 
                  		// Determina corte vertical, pieza la pone
                  		// al lado derecho del patrón de corte
                  		if(Acal > Nual) { 
                     		// Genera Pérdida bajo la nueva pieza
                     		// y a la derecha del patrón de corte actual
                     		// Ingresa nueva Pérdida de dimensiones
                     		// (Ancho nueva pieza) x (crecimiento a lo ancho (derecho) del patrón)
                     		app_agregalistap_g(Nuan,Acal - Nual);
                     		// Dimensiona nuevo ancho del patrón de corte actual
                     		Acan = Acan + Nuan;
                  		}//End if
                  		else if(Nual > Acal) {  
                     		// Genera Pérdida bajo el patrón de corte
                     		// y a la izquierda de la nueva pieza.
                     		// Ingresa nueva Pérdida de dimensiones (Ancho del patrón antes de insertar
                     		// nueva pieza) x (crecimiento a lo alto (hacia abajo) del patrón)
                     		app_agregalistap_g(Acan,Nual - Acal);
                     		// Dimensiona nuevo ancho del patrón de corte actual
                     		Acan = Acan + Nuan;
                     		// Dimensiona nuevo alto del patrón de corte actual
                     		Acal = Nual;
                  		}//End else if
                  		else 
	                     	// No hay pérdida,el alto de la nueva pieza coincide con el del patrón de corte
    	                 	// pero hay que actualizar el ancho del nuevo patrón de corte
        	             	Acan = Acan + Nuan;
             	  	}// End else if
               		else PieInc--; // Pieza excede el area disponible en la lámina.
            	}//End else
         	}//End if
      		else PieInc--; // Pieza excede el area disponible en la lámina.
      	}//End for
   	}//End if
   	PerdExt = (AnchoPl - Acan) * AltoPl + Acan * (AltoPl - Acal);
   	Bus = LPer;
   	acum = 0;
	cont = 0;
   	while(Bus != NULL) { //Calcula Pérdida Interna
      	acum = acum + (Bus->ancho * Bus->alto);
      	Bus = Bus->prox;
      	cont++;
   	}//End while
   	acum = acum + PerdExt;

   	//UnFitness : Componente_Perdida + Componente_Distancia + Componente_Digregacion
   	//Componente_Perdida     = Peso_Perdida * (Perdida_Total / Area_Placa)
   	//Componente_Distancia   = Peso_Distancia * (SUMA_en_i(Restriccion_pieza_i - Numero_Pieza_i_Presentes) / SUMA_en_i(Restriccion_pieza_i))
   	//Componente_Digregacion = Peso_Digregacion * (1 - EXP(- Numero_Perdidas / TAU)) 
   	//                         donde TAU = K * Cantidad_de_Piezas_Distintas 
   	//                         => La idea es tener las piezas de un mismo tipo, juntas
   	//El sistema podra discriminar la digregacion dentro de 2 TAU, K: Hay que determinarlo experimentalmente.

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
    app_liberamemlistaperdidas_g(PerdExt);
    return Eval;
}//End app_funceval_g

void app_objfunc_g(struct individual *critter)
// Función Objetivo, transforma el string cromosoma en un arreglo de piezas
{
   unsigned mask=1, tp, tp1, bitpos;
   int 		j, k, stop;
   TNodoAP 	*piezaschromo;// variable que apunta a todas las piezas del cromosoma generado

   	if((piezaschromo = (TNodoAP*) malloc(NumPie*sizeof(TNodoAP))) == NULL)
      	nomemory("piezaschromo en objfunc");
#ifdef _DEBUG_MALLOC_		
   	printf("Malloc, App_g.c, 440, piezaschromo, %d\n", NumPie*sizeof(TNodoAP));
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
                piezaschromo[bitpos].ancho = 0;
                piezaschromo[bitpos].alto = 0;
                piezaschromo[bitpos].numero = 0;
                piezaschromo[bitpos].cantidadpiezas = 1;
            }//End else
            tp = tp >> 1;
            tp1 = tp1 >> 1;
        }//End for
    }//End for
    critter->PEval = app_funceval_g(piezaschromo);
    critter->fitness = (float) critter->PEval.perdida;
    free(piezaschromo);
#ifdef _DEBUG_MALLOC_		
    printf("Free, app_g.c, 512, piezaschromo\n");
#endif
}//End app_objfunc_g

void app_genera_resultados_problema_g(int corrida, int tipo_problema, char *nombrearchivo)
//Función que imprime los resultados del problema corte de pieza guillotina
{
    if (encabezado_resultado_problema == 0) {
        //IMPRIME ENCABEZADO DE LOS RESULTADOS DEL PROBLEMA

        fprintf(rprofp, "CORRIDA;MS_FITNESS;WORKER_MEJOR_INDIVIDUO\n");

        encabezado_resultado_problema = 1; //Con ésto no imprime más el encabezado
    }//End if 
    //IMPRIME RESULTADOS DEL PROBLEMA

    fprintf(rprofp, "%d %f %d\n", corrida, bestfit.fitness, bestfit.generation);

}//End app_genera_resultados_problema_g

void app_free_g(void)
//Función que libera variables del problema
{
    free(piezasdistintas);
#ifdef _DEBUG_MALLOC_		
    printf("Free, app_g.c, 287, piezasdistintas\n");
#endif
    free(piezasproblema);
#ifdef _DEBUG_MALLOC_		
    printf("Free, app_g.c, 290, piezasproblema\n");
#endif
}//End app_free_g
