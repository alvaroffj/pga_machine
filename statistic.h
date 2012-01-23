#ifndef _STATISTIC_H_
#define _STATISTIC_H_

void   statistics(struct individual *pop);
struct individual *obtiene_individuos_a_enviar(struct individual *opop, struct individual *npop);
int    FitnessCompara(struct individual *Indi, struct individual *Indj);
void   inserta_individuos_en_poblacion(struct individual *rpop, struct individual *npop, struct individual *opop);

#endif

