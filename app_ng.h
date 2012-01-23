/*----------------------------------------------------------------------------*/
/* app2_ng.c - rutinas para Problema Corte de Piezas No Guillotina = PCPNG    */
/*----------------------------------------------------------------------------*/

#ifndef _APP_NG_H_
#define _APP_NG_H_

void     ordena_piezas_problema_ng(void);
int      app_leearchivo_ng(char *nombrearchivo, int rank_actual);
TEval    app_funceval_ng(TNodoAP *piezas);
void     app_objfunc_ng(struct individual *critter);
int      AreaNodoAPCompara_ng(TNodoAP *Nodoi, TNodoAP *Nodoj);
int      HorizontalNodoAPCompara_ng(TNodoAP *Nodoi, TNodoAP *Nodoj);
int      VerticalNodoAPCompara_ng(TNodoAP *Nodoi, TNodoAP *Nodoj);
void 	 app_genera_resultados_problema_ng(int corrida, int tipo_problema, char *nombrearchivo);
void 	 app_free_ng(void);

#endif
