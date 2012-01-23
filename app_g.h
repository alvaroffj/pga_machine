/*----------------------------------------------------------------------------*/
/* app_g.h - rutinas para Problema de Corte de Piezas Guillotina              */
/*----------------------------------------------------------------------------*/

#ifndef APP_G_H_
#define APP_G_H_

#include "var_pcp.h"

void     app_liberamemlistaperdidas_g(int perdidaexterna);
void     app_agregalistap_g(int anc,int alt);
void     ordena_piezas_problema_g(void);
int      app_leearchivo_g(char *nombrearchivo, int rank_actual);
TEval    app_funceval_g(TNodoAP *piezas);
void     app_objfunc_g(struct individual *critter);
TNodoRE  app_pieza2perdida_g(int anc,int alt);
int      AreaNodoAPCompara_g(TNodoAP *Nodoi, TNodoAP *Nodoj);
int      HorizontalNodoAPCompara_g(TNodoAP *Nodoi, TNodoAP *Nodoj);
int      VerticalNodoAPCompara_g(TNodoAP *Nodoi, TNodoAP *Nodoj);
void     app_genera_resultados_problema_g(int corrida, int tipo_problema, char *nombrearchivo);
void 	 app_free_g(void);

#endif /*APP_G_H_*/
