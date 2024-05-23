#ifndef CPU_GESTOR_H
#define CPU_GESTOR_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include <utils/include/shared.h>

#include <commons/log.h>
#include <commons/config.h>

// VARIABLES GLOBALES
extern t_log* cpu_logger;
extern t_log* cpu_logger_extra;

extern t_config* cpu_config;

extern char* path_config_CPU;

extern t_log* cpu_logger;
extern t_log* cpu_log_obligatorio;

typedef struct{
    int proceso_pid;
    int proceso_pc;
    int proceso_tiempo_ejecutado;
    int proceso_ticket;
    uint32_t AX;
    uint32_t BX; 
    uint32_t CX;
    uint32_t DX;
}t_contexto;

extern t_contexto* contexto;

extern pthread_mutex_t mutex_manejo_contexto;

extern t_log* cpu_logger;
extern t_log* cpu_log_obligatorio;

// Variables de config
extern char* IP_MEMORIA;
extern char* PUERTO_MEMORIA;
extern char* PUERTO_ESCUCHA_DISPATCH;
extern char* PUERTO_ESCUCHA_INTERRUPT;

extern int CANTIDAD_ENTRADAS_TLB;
extern char* ALGORITMO_TLB;

extern int fd_cpu_dispatch;
extern int fd_cpu_interrupt;
extern int fd_kernel_dispatch;
extern int fd_kernel_interrupt;
extern int fd_memoria;

#endif


