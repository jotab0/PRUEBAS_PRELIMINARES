#ifndef CPU_H_
#define CPU_H_

#include "c_gestor.h"
#include "inicializar_cpu.h"
#include "comunicaciones_cpu.h"
#include <utils/include/shared.h>
#include <pthread.h>

// VARIABLES GLOBALES
t_log* cpu_logger;
t_log* cpu_logger_extra;
t_contexto* contexto;
t_config* cpu_config;
t_log* cpu_log_obligatorio;

pthread_mutex_t mutex_manejo_contexto;

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

char* respuesta_marco_escritura;
char* respuesta_marco_lectura;

int marco;
int tamanio_pagina;
int motivo_interrupcion;
int resultado;
char** instruccion_dividida;
int algoritmo_tlb;
t_tlb* tlb;
int ordenCargaGlobal;

#endif