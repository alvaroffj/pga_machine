/*----------------------------------------------------------------------------*/
/* sga.h - variables globales relacionadas con el algoritmo	                  */
/*----------------------------------------------------------------------------*/

#define LINELENGTH 80                                    /* width of printout */
#define BITS_PER_BYTE 8            /* number of bits per byte on this machine */
#define UINTSIZE (BITS_PER_BYTE*sizeof(unsigned))    /* # of bits in unsigned */
//#define _DEBUG_MALLOC_ /* SI DEF => MUESTRA MENSAJES MALLOC Y FREE          */
//#define _PRINT_MIGRACION_/* SI DEF => MUESTRA MENSAJES DE MIGRACION EN RANK 0 */

#define MSJ_DATOS                      10
#define MSJ_TERMINO                    11
#define MSJ_SIGA                       12
#define MSJ_LISTO_PARA_COMENZAR        13
#define MSJ_COMIENCE                   14
#define MSJ_TERMINE_EVOLUCION          15
#define MSJ_DATOS_COORDINADOR          16
#define MSJ_TERMINO_CONFIRMADO         17
#define MSJ_DATOS_ULTIMA_VEZ           18
#define MSJ_ENVIE_INDIVIDUOS           19
#define MSJ_LISTO_PARA_MIGRAR          20
#define MSJ_RUNMAX                     21
#define MSJ_LINEA_IN                   22
#define MSJ_CANT_CHAR_A_REC            23
#define MSJ_ERROR_ARCHIVO_INSTANCIA    24
#define MSJ_CHAR_ARCHIVO_INSTANCIA     25

#define true   1
#define false  0

#include <stdio.h>
#include <sys/times.h>
#include <mpi.h>

/* file pointers */
FILE *outfp, *infp, *evofp, *rprofp, *ralgfp;

/*Estructura que almacena las pérdidas del individuo */
typedef struct eval
{
   float 	perdida;
   float 	areaocupada;
   float 	calidad;
   int   	piezas;
   float 	c_distancia;   //Componente Distancia
   float 	c_perdidareal; //Componente Perdida
   float 	c_digregacion; //Componente Digregacion
   int   	n_perdidas;    //Numero de Perdidas generadas
   double 	var1;		   //Variable Nro.1
}TEval;

/* Global structures and variables */
struct individual 
{
   unsigned *chrom;     /* chromosome string for the individual */
   float    fitness;    /* fitness of the individual */
   TEval    PEval;      /* Estructura con montos de pérdida total, calidad, etc..*/
   int      xsite;      /* crossover site at mating chromosome string */
   int      msite;      /* crossover site at mating chromosome  mutation*/
   int      parent[2];  /* who the parents of offspring were */
   unsigned *chmut;     /* chromosoma que representa la mutation en las piezas */
   unsigned short *pusListaPiezas; /* cromosoma donde cada gen puede tener un valor mayor o igual a cero */
};

struct bestever
{
   unsigned *chrom;     /* chromosome string for the best-ever individual */
   float    fitness;    /* fitness of the best-ever individual */
   TEval    PEval;      /* Estructura con montos de p�rdida total, calidad, etc..*/
   int      generation; /* generation which produced it */
   unsigned *chmut;     /* chromosoma mutation que representa las piezas */
   unsigned short *pusListaPiezas; /* cromosoma donde cada gen puede tener un valor mayor o igual a cero */
};

struct indcruz
{
   unsigned *chrom;     /* cromosoma string=>individuo usado para el cruzamiento */
   unsigned *chmut;     /* cromosoma mutac.=>individuo usado para el cruzamiento */
   unsigned short *pusListaPiezas; /* cromosoma donde cada gen puede tener un valor mayor o igual a cero */
};

struct individual *oldpop;    /* last generation of individuals */
struct individual *temppop;   /* temp generation of individuales */
struct individual *newpop;    /* next generation of individuals */
struct individual *envpop;    /* individuos a enviar */
struct individual *recpop;    /* individuos a recibir */
struct individual *glopop;    /* individuos en población global del Coordinador */
struct bestever bestfit;      /* fittest individual so far */
struct indcruz indcross;      /* Individuo usado para el operador de cruza */
float  sumfitness;            /* summed fitness for entire population */
float  max;                   /* maximum fitness of population */
float  min;                   /* minumum fitness of population */
float  med;                   /* medium  fitness of population */
float  varianza;              /* varianza de la poblacion */
float  desvestandar;          /* Desviación Estándar de la poblacion */
int    numfiles;              /* number of open files */
int    popsize;               /* population size */
int    chlistasize;			  /* number of bytes needed to store lista */
int    lchrom;                /* length of the chromosome per individual */
int    lchmut;                /* length of the mutatin chromosome per individual */
int    chromsize;             /* number of bytes needed to store lchrom string */
int    chmutsize;             /* number of bytes needed to store NumPie string */
int    gen;                   /* current generation number */
int    maxgen;                /* maximum generation number */
int    printstrings;      	  /* flag to print chromosome strings (default on) */
int    nmutation;             /* number of mutations to chromosome string */
int    ncross;                /* number of crossovers to chromosome string */
int    mmutation;             /* number of mutations to chromosome mutation */
int    mcross;                /* number of crossovers to chromosome mutation */
float  pcross;                /* probability of crossover */
float  pmutation;             /* probability of mutation */
float  randomseed;            /* seed probability */
int    run;                   /* cuenta cantidad de archivos a usar */
int    runmax;                /* cuenta cantidad de archivos maximos a usar */
int    n_ind_a_enviar;        /* nro. individuos a enviar a nodo maestro*/
int    n_ind_a_recibir;       /* nro. individuos a recibir desde nodo maestro*/
int    cantidad_char_a_recibir; /*Cantidad de char a recibir en AG's desde Coordinador */ 
int    coord_ind_a_env;       /* nro. individuos a enviar desde coord. a nodo AG*/
int    coord_ind_a_rec;       /* nro. individuos a recibir desde nodo AG a coord*/
int    coord_ind_global;      /* nro. individuos total recibidos en Coordinador*/
//float  pind_env_rec;          /* porcentaje de ind.de la Pob a enviar y recibir */
float  pind_env;		      /* porcentaje de ind. de la pob a enviar*/
float  pind_rec;		      /* porcentaje de ind. de la pob a recibir*/
int    tasa_migracion;        /* número de generaciones a esperar para migrar*/ 
int    n_migracion;           /* nro. de migración en que va el Coordinador */
char   answer[2];             /* variable que indica si imprime report() o no */
char   answer_mod_mig[2];     /* variable que indica model migracion A o S */
int    modelo_migracion;      /* indica mododel migracion 1=Sincrono o 0=Asincrono */
int    cuenta_gen_para_migracion;   /* Cuenta nro. de gen para migracion */
int    espera_enviar_individuos;    /* variable => espera para enviar ind.*/
int    tipo_problema;		  /* variable que indica qué tipo de problema se está considerando */
							  /* TIPO_PROBLEMA = 0 => PCPG  : Problema de Corte de Pieza Guillotina */
							  /* TIPO_PROBLEMA = 1 => PCPNG : Problema de Corte de Pieza No Guillotina */      

/*Variables globales*/
char           linea[255];           /* variable utilizada para almacenar texto para el Log */
char           nomarch[100];         /* variable donde se almacena nombre de archivo a usar */
char           ruta_instancias[100]; /* Ruta completa del directorio donde est�n los archivos instancias */
char           ruta_resultados[100]; /* Ruta completa del directorio donde quedar�n los archivos resultados */
char 		   arch_salida[100]; 	 /* Archivo resultante de salida */
char	 	   arch_reporte_pro[100];/* Archivo de reporte de variables importantes por corrida, relacionada por cada problema a resolver */
char		   arch_reporte_alg[100];/* Archivo de reporte de variables importantes por corrida, relacionada con el algoritmo genético a resolver */
char 		   arch_evolucion[100];  /* Archivo con evolución del Algoritmo Genético por cada corrida */
unsigned long  r250_buffer[250];     /*Variable usada en la generacion de la semilla aleatoria */
unsigned long  r250_index;           /*Variable usada en la generacion de la semilla aleatoria */
unsigned long  seed_val;             /*Variable usada en la generacion de la semilla aleatoria */
int      	   rank_seed;        	 /*rank_seed: variable que hace correr función nueva_semilla */
                          			 /*            dependiendo que rank es.						*/
int            bitsxcodigobinario;   /* variable donde se almacena cantidad de bits  */
                                     /* del Codigo Binario necesarios para almacenar */
                                     /* los tipos de piezas del problema             */
int 		   largo_cromosoma;	 	 /* Define el largo del cromosoma */
float		   fitness_inicial;		 /* Define fitness_inicial con el que parte el bestfit */
int 		   cantidad_char_que_determinan_instancia_del_problema; /* Determina cuantos char representan la instancia a resolver */
char 		   *char_instancia_problema; /* Variable que apunta a los char que contienen la información de una instancia del problema */ 
int		       encabezado_resultado_algoritmo;/* Variable que determina si encabezado del archivo de resultado del algortmo está impreso o no */
int 		   encabezado_resultado_problema; /* Variable que determina si encabezado del archivo de resultado del problema está impreso o no */
char     	   linea_in[100];    	 /* Variable usada para pasar desde nodo0 a cada rank, cada linea de archivo in.txt */
int 		   tipo_elemento_cromosoma;	/* Define el Tipo de elemento de cada cromosoma */

enum { BINARIO = 0, ENTEROS = 1};

// Variables Utilizadas para funci�n Elapsed_time
typedef struct {
   double elapsed_time;             /*Tiempo total transcurrido*/	
   double cpu_time;                 /*Tiempo de CPU*/
   double comm_time;                /*Tiempo de Comunicaci�n*/
} cpu_consummed_time_t;

//Variables para determinar el tiempo transcurrido
double time_start, time_middle, time_end, time_comm, time_comm_rank, time_send, time_mejor;
cpu_consummed_time_t time_consumation;

//Variables MPI
MPI_Datatype   		message_type_send, message_type_receive;
MPI_Request    		request;
MPI_Status     		status;
int            		numtasks, rank, workers;
int            		flag1, flag2;     // Variables para chequear Estado de env�o y recepci�n de msj.
int            		nodo0, nodox;     // nodo0 : Nodo Coordinador, nodox : Nodo AG x-�simo.
struct individual 	*pop_a_enviar; //Puntero a poblaci�n a enviar por cada Rank's.

/**********************************************/
/********* VARIABLES DEL COORDINADOR **********/
int LISTO_PARA_COMENZAR; //Variable que indica que ha recibido msj de listo para comenzar de todos los rank
int LISTO_PARA_TERMINAR; //Variable que indica que ha recibido msj de t�rmino de todos los rank
int LISTO_PARA_SEGUIR;   //Variable que indica que ha recibido los datos de todos los rank => SIGA
int LISTO_PARA_SEGUIR_ULTIMA_VEZ;   //Variable que indica que ha recibido los últimos datos de todos 
                                    //los rank => SIGA por �ltima vez
int LISTO_PARA_TERMINAR_EVOLUCION;  //Indica que rank0 da orden de terminar evolucion a los dem�s rank
int LISTO_PARA_MIGRAR_TODOS_LOS_RANK; //Variable que indica si todos los Rank deben Migrar sus individuos
/**********************************************/
