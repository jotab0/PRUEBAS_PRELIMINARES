#ifndef K_GESTOR_H_
#define K_GESTOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <readline/readline.h>
#include <semaphore.h>

#include "../src/utils/include/shared.h"

#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>

//Variables globales

extern t_log* kernel_logger;
extern t_log* kernel_log_debug;
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
extern int fd_kernel;

typedef enum{
	NEW,
	READY,
	EXEC,
	BLOCKED,
	EXIT
}estado_pcb;

typedef struct{
	uint32_t AX;
	uint32_t BX;
	uint32_t CX;
	uint32_t DX;
}registrosCPU;

typedef struct{ //
	int pid;
	int program_counter;
	int quantum;
	int tiempo_ejecutado;
	int ticket;
	estado_pcb estado;
	registrosCPU* registros_CPU;
}pcb;

// LISTAS Y VARIABLES DE PLANIFICACIÓN



extern int identificador_PID;	// mutex: mutex_pid
extern int contador_pcbs;
extern int ticket_actual;		// mutex: mutex_ticket

extern pthread_mutex_t mutex_lista_ready;
extern pthread_mutex_t mutex_lista_ready_plus;
extern pthread_mutex_t mutex_lista_exec;
extern pthread_mutex_t mutex_ticket;
extern pthread_mutex_t mutex_pid;

extern t_list* ready;			// mutex: mutex_lista_ready
extern t_list* ready_plus;		// mutex: mutex_lista_ready_plus
extern t_list* execute;			// mutex: mutex_lista_exec
extern t_list* new;
extern t_list* blocked;
extern t_list* lista_exit;

extern sem_t sem_enviar_interrupcion;


#endif