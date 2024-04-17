#ifndef K_GESTOR_H_
#define K_GESTOR_H_

/* TODAS LAS VARIABLES GLOBALES QUE NECESITE SE DECLARAN 
EN kernel.h Y SE HACE UNA COPIA EN k_gestor.h CON LA PALABRA 
RESERVADA extern. */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "../src/utils/include/shared.h"

#include <commons/log.h>
#include <commons/config.h>

//Variables globales

extern t_log* kernel_logger;
extern t_log* kernel_logger_extra;
extern t_config* kernel_config;

extern char* path_config_Kernel;

// Variables de config

extern char* PUERTO_ESCUCHA;

extern char* IP_MEMORIA;
extern char* PUERTO_MEMORIA;

extern char* IP_ENTRADASALIDA;
extern char* PUERTO_ENTRADASALIDA;

extern char* IP_CPU;
extern char* PUERTO_CPU_DISPATCH;
extern char* PUERTO_CPU_INTERRUPT;

extern char* ALGORITMO_PLANIFICACION;
extern int QUANTUM;
extern char** RECURSOS;
extern char** INSTANCIAS_RECURSOS;
extern int GRADO_MULTIPROGRAMACION;

extern int fd_memoria;
extern int fd_entradasalida;
extern int fd_cpu_dispatch;
extern int fd_cpu_interrupt;

#endif