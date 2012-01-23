/*--------------------------------------------------------------------------------------------------*/
/* app2_ir.h - rutinas que generan archivo con layout para Problema de Corte de Piezas Irregulares  */ 
/*             manejado con cromosoma con ENTEROS           										*/
/*--------------------------------------------------------------------------------------------------*/

#ifndef APP2_IR_H_
#define APP2_IR_H_

double 	 app2_EncajePrimeraPieza_ir(unsigned Orientacion);
void 	 app2_EncajeEnBanda_ir(unsigned Orientacion);
void 	 app2_DeterminaPosicionInicial_ir(unsigned Orientacion, double dMax);
double   app2_dObtieneDistanciaPuntoLado_ir( double xP, double yP,	double xA, double yA, double xB, double yB );
void 	 app2_BuscaDistanciaDesplazamiento_ir(unsigned Orientacion);
void 	 app2_DesplazaPieza_ir(void);
double 	 app2_ActualizaPerfil_ir(unsigned Orientacion);
TEval    app2_funceval_ir(unsigned Orientacion, double dMaximoX, double dAreaPiezasEncajadas);
void     app2_objfunc_ir(int tipo_problema, char *nomarch, struct bestever *bestcritter, int run_actual);
void 	 app2_genera_layout_ir(int tipo_problema, char *nombrearchivo, cpu_consummed_time_t *t, int run_actual);
 
#endif /*APP2_IR_H_*/
