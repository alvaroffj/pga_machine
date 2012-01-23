/*---------------------------------------------------------------------------------------------------*/
/* app2_ng.h - rutinas que generan archivo con layout para Problema de Corte de Piezas NO Guillotina */
/*---------------------------------------------------------------------------------------------------*/

#ifndef _APP2_NG_H_
#define _APP2_NG_H_

void     app2_agregalistap_ng(int x, int y, int anc, int alt, char tipo, int t_pieza);
void 	 app2_genera_layout_ng(int tipo_problema, char *nombrearchivo, cpu_consummed_time_t *t, TListaPE2 *Bus2, int run_actual);
void     app2_funceval_ng(int tipo_problema, char *nomarch, TNodoAP *piezas, int run_actual);
void     app2_objfunc_ng(int tipo_problema, char *nomarch, struct bestever *bestcritter, int run_actual);
TNodoRE2 app2_pieza2perdida_ng(int anc,int alt);

#endif
