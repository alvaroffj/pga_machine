/*-----------------------------------------------------------------------------------------------*/
/* app2_g.h - rutinas que generan archivo con layout para Problema de Corte de Piezas Guillotina */
/*-----------------------------------------------------------------------------------------------*/

#ifndef APP2_G_H_
#define APP2_G_H_

#include "var_pcp.h"

void     app2_liberamemlistaperdidas_g(void);
void     app2_agregalistap_g(int x, int y, int anc, int alt, char tipo, int t_pieza);
void 	 app2_genera_layout_g(int tipo_problema, char *nombrearchivo, cpu_consummed_time_t *t, TListaPE2 *Bus2, int run_actual);
void     app2_funceval_g(int tipo_problema, char *nomarch, TNodoAP *piezas, int run_actual);
void     app2_objfunc_g(int tipo_problema, char *nomarch, struct bestever *bestcritter, int run_actual);
TNodoRE2 app2_pieza2perdida_g(int anc,int alt);

#endif /*APP2_G_H_*/
