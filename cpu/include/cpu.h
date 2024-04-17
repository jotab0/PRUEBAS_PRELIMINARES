#ifndef CPU_H_
#define CPU_H_

#include "c_gestor.h"
#include "inicializar_cpu.h"
#include "comunicaciones.h"
#include <utils/include/shared.h>

// VARIABLES GLOBALES
t_log* cpu_logger;
t_log* cpu_logger_extra;

t_config* cpu_config;

char* path_config_CPU = "/home/utnso/Documents/tp-2024-1c-ubuntunel/cpu/CPU.config";

// Variables de config
char* IP_MEMORIA;
char* PUERTO_MEMORIA;
char* PUERTO_ESCUCHA_DISPATCH;
char* PUERTO_ESCUCHA_INTERRUPT;

int CANTIDAD_ENTRADAS_TLB;
char* ALGORITMO_TLB;

int fd_cpu_dispatch;
int fd_cpu_interrupt;
int fd_kernel_dispatch;
int fd_kernel_interrupt;
int fd_memoria;



#endif