/*----------------------------------------------------------------------------*/
/* master.h - rutinas relacionadas con el coordinador		                  */
/*----------------------------------------------------------------------------*/

#ifndef _MASTER_H_
#define _MASTER_H_

void   almacena_ind_recibidos_en_pop_global(struct individual *recpop, int coord_ind_a_recibir, int nodo);
struct individual *selecciona_individuos_a_enviar(struct individual *glopop, 
                   int coord_ind_globales, int coord_ind_a_enviar);
int    FitnessComparaMaster(struct individual *Indi, struct individual *Indj);
void   nomemoryMaster(char *string);
void   initmallocMaster(int coord_ind_a_recibir, int coord_ind_a_enviar, int coord_ind_globales);
void   freeallMaster(int coord_ind_a_recibir, int coord_ind_a_enviar, int coord_ind_globales);
void   statisticsMaster(struct individual *glopop, int coord_ind_globales, int workers, int coord_ind_a_recibir);
int    almacena_archivo_instancia(char *nombrearchivo);
int    genera_archivo_temporal(char *nombre_archivo, char *chars, int cant_char_a_rec);

#endif

