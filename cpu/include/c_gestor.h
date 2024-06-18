#ifndef CPU_GESTOR_H
#define CPU_GESTOR_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include <utils/include/shared.h>

#include <commons/log.h>
#include <commons/config.h>
#include <commons/temporal.h>

#define LIBRE 0
#define OCUPADA 1

// VARIABLES GLOBALES
extern t_log* cpu_logger;
extern t_log* cpu_logger_extra;
extern t_log* cpu_log_obligatorio;

extern t_config* cpu_config;

extern char* path_config_CPU;

typedef struct{
    int proceso_pid;
    int proceso_pc;
    int proceso_tiempo_ejecutado;
    int proceso_ticket;
    uint32_t AX;
    uint32_t BX; 
    uint32_t CX;
    uint32_t DX;
    uint32_t SI;
    uint32_t DI;
}t_contexto;

extern t_contexto* contexto;
//extern t_list* lista;
extern pthread_mutex_t mutex_manejo_contexto;

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

// CICLO INSTRUCCION

extern char** instruccion_dividida;

extern int motivo_interrupcion;

//extern char* motivo_bloqueo;

extern bool hay_interrupcion;

// TLB

typedef struct tlbEntrada{
    uint32_t pid;
    uint32_t pagina;
    int32_t marco;
    int estado;

    int orden_carga;
    t_temporal* ultimo_uso;
}t_tlbEntrada;

typedef struct tlb {
    t_tlbEntrada* entradas;
    int tamanio;
} t_tlb;

extern t_tlb* tlb;
extern int algoritmo_tlb;



extern int ordenCargaGlobal;

/*
typedef struct {
    int pid;
    int nro_pag;
    int direc_fisica;
    bool ult_vez_usado;
} t_entradaTabla;

extern t_list* tlb;
*/

extern int tamanio_pagina;

extern char* valor_marco;

extern int marco;

extern int resultado;

extern char* respuesta_marco_lectura;
extern char* respuesta_marco_escritura;

#endif


