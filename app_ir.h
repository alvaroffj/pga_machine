/*------------------------------------------------------------------*/
/* app_ir.h - rutinas para Problema de Corte de Piezas Irregulares  */
/*   		  manejado con cromosoma con ENTEROS           			*/
/*------------------------------------------------------------------*/

#ifndef APP_IR_H_
#define APP_IR_H_

int      app_leearchivo_ir(char *nombrearchivo, int rank_actual);
void 	 app_free_ir(void);
double   app_EncajePrimeraPieza_ir(unsigned Orientacion);
void 	 app_EncajeEnBanda_ir(unsigned Orientacion);
void 	 app_DeterminaPosicionInicial_ir(unsigned Orientacion, double dMax);
double 	 app_dObtieneDistanciaPuntoLado_ir( double xP, double yP,	double xA, double yA, double xB, double yB );
void 	 app_BuscaDistanciaDesplazamiento_ir(unsigned Orientacion);
void 	 app_DesplazaPieza_ir(void);
TEval 	 app_funceval_ir(unsigned Orientacion, double dMaximoX, double dAreaPiezasEncajadas);
void     app_objfunc_ir(struct individual *critter);
void 	 app_genera_resultados_problema_ir(int corrida, int tipo_problema, char *nombrearchivo);

#endif /*APP_IR_H_*/
