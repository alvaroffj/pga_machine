#ifndef _RANDOM_H_
#define _RANDOM_H_

void     advance_random(void);
int      flip(float prob);
void     randomize(void);
double   randomperc(void);
int      rnd(int low,int high);
void     warmup_random(float random_seed);
void     inicializa_semilla(void);
unsigned long randlcg(void);
float    nueva_semilla(void);

#endif

