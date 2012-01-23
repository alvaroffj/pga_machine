/*------------------------------------------------------------------------------*/
/* random.c - Contiene generador de números aleatorios y utilidades relacionadas*/
/* incluyendo advance_random, warmup_random, random, randomize, flip, y rnd     */
/*------------------------------------------------------------------------------*/

#include <limits.h>
#include <time.h>

#include "sga.h"

// variables son declaradas estáticas de modo que ellas no tengan conflictos
// con nombres de otras variables en otros archivos
static double  oldrand[55];   // Arreglo de 55 números aleatorios
static int     jrand;         // Actual número aleatorio

void advance_random(void)
// Crea batch de 55 números aleatorios
{
   	int j1;
   	double new_random;

   	for(j1 = 0; j1 < 24; j1++) {
      	new_random = oldrand[j1] - oldrand[j1+31];
      	if(new_random < 0.0) new_random = new_random + 1.0;
         	oldrand[j1] = new_random;
   	}//End for
   	for(j1 = 24; j1 < 55; j1++) {
      	new_random = oldrand [j1] - oldrand [j1-24];
      	if(new_random < 0.0) new_random = new_random + 1.0;
         	oldrand[j1] = new_random;
   	}//End for
}//End advance_random

double randomperc(void)
/* Fetch a single random number between 0.0 and 1.0 - Subtractive Method */
/* See Knuth, D. (1969), v. 2 for details */
/* name changed from random() to avoid library conflicts on some machines*/
{
   	jrand++;
	if(jrand >= 55) {
      	jrand = 1;
      	advance_random();
   	}//End if
   	return(oldrand[jrand]);
}//End randomperc

int flip(float prob)
// Tira una moneda, retorna True si es cara
{
   	if(randomperc() <= (double) prob)
    	return(1);
   	else
      	return(0);
}//End flip

int rnd(int low,int high)
// Entrega un entero aleatorio entre low y high
{
   	int i;

   	if(low >= high)
      	i = low;
   	else {
      	i = (int) ((randomperc() * (double) (high - low + 1)) + (double) low);
      	if(i > high) i = high;
   	}//End else
   	return(i);
}//End rnd

void warmup_random(float random_seed)
// Get random off and running */
{
   	int j1, ii;
   	double new_random, prev_random;

   	oldrand[54] = random_seed;
   	new_random = 0.000000001;
   	prev_random = random_seed;
   	for(j1 = 1 ; j1 <= 54; j1++) {
      	ii = (21*j1)%54;
      	oldrand[ii] = new_random;
      	new_random = prev_random-new_random;
      	if(new_random<0.0) new_random = new_random + 1.0;
      	prev_random = oldrand[ii];
   	}//End for
   	advance_random();
   	advance_random();
   	advance_random();
   	jrand = 0;
}//End warmup_random

void randomize(void)
/* Get seed number for random and start it up */
{
   	int j1;

   	for(j1=0; j1<=54; j1++)
      	oldrand[j1] = 0.0;
   	jrand=0;
   	warmup_random(randomseed);
}//End randomize

/**********************************************************************************/
/************************ GENERACION DE SEMILLA ALEATORIA *************************/
/**********************************************************************************/

/*********** PARA 32 BITS ****************/
const unsigned long MSB = 0x80000000L;
const unsigned long ALL_BITS = 0xffffffffL;
const unsigned long HALF_RANGE = 0x40000000L;
/*****************************************/

/*********** PARA 64 BITS ****************
const unsigned long MSB = 0x8000000000000000L;
const unsigned long ALL_BITS = 0xffffffffffffffffL;
const unsigned long HALF_RANGE = 0x4000000000000000L;
/*****************************************/


const unsigned long STEP = 7;
const unsigned long quotient  = LONG_MAX / 16807L;
const unsigned long remainder_x = LONG_MAX % 16807L;

unsigned long randlcg(void)
{
   	if ( seed_val <= quotient )
      	seed_val = (seed_val * 16807L) % LONG_MAX;
   	else	{
      	unsigned long high_part = seed_val / quotient;
      	unsigned long low_part  = seed_val % quotient;
      	unsigned long test = 16807L * low_part - remainder_x * high_part;
      	if ( test > 0 )
         	seed_val = test;
      	else
         	seed_val = test + LONG_MAX;
   	}//End else
   	return seed_val;
}//End randlcg 

void inicializa_semilla(void)
{
   	unsigned long j, k;
   	unsigned long mask, msb;
   	seed_val = time(NULL);
   	r250_index = 0;
   
   	for (j = 0; j < 250; j++)
      	r250_buffer[j] = randlcg();/* al parecer reseteo*/
   	for (j = 0; j < 250; j++)
      	if ( randlcg() > HALF_RANGE )
         	r250_buffer[j] |= MSB;
   	msb = MSB;
   	mask = ALL_BITS;
   	for (j = 0; j < 32; j++) {
      	k = STEP * j + 3;
      	r250_buffer[k] &= mask;
      	r250_buffer[k] |= msb;
      	mask >>= 1;
      	msb  >>= 1;
   	}//End for
}//End inicializa_semilla

float nueva_semilla(void)
{
   	unsigned long j;
   	unsigned long new_rand;
   	float get_seed;

   	if ( r250_index >= 147 )
      	j = r250_index - 147;
   	else
      	j = r250_index + 103;
   	new_rand = r250_buffer[ r250_index ] ^ r250_buffer[ j ];
   	r250_buffer[ r250_index ] = new_rand;
   	if ( r250_index >= 249 )
      	r250_index = 0;
   	else
      	r250_index++;
   	get_seed = (float)((float)new_rand / (float)ALL_BITS);
   	return get_seed;
}//End nueva_semilla
