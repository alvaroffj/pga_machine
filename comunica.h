#ifndef _COMUNICA_H_
#define _COMUNICA_H_

void envia_mensaje(int tipo_mensaje, char *texto_mensaje);
void comunicacion_asincrona_con_coordinador(void);
void comunicacion_asincrona_con_cada_rank(void);
void comunicacion_sincrona_con_coordinador(void);
void comunicacion_sincrona_con_cada_rank(void);
void Build_type_pop(struct individual *pop, MPI_Datatype *message_type_pop, int size_pop);
int  lee_char_y_genera_achivo_instancia_tmp(int tipo_problema, int cant_enteros_a_rec);
void inicializa_mpi(int argc,char *argv[]);
void envia_a_cada_rank(int mensaje); 

#endif
