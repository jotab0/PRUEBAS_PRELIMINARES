#ifndef KERNEL_H_
#define KERNEL_H_

#include "k_gestor.h"

#include "kernel_cpu_dispatch.h"
#include "kernel_cpu_interrupt.h"
#include "kernel_entradasalida.h"
#include "kernel_memoria.h"
#include "inicializar_kernel.h"
#include <utils/include/shared.h>
#include <pthread.h>
#include "consola.h"

// VARIABLES GLOBALES
t_log* kernel_logger;
t_log* kernel_log_debug;
t_log* kernel_logger_extra;
t_config* kernel_config;

int fd_memoria;
int fd_entradasalida;
int fd_cpu_dispatch;
int fd_cpu_interrupt;
int fd_kernel;
// Variables de config
char* PUERTO_ESCUCHA;

char* IP_MEMORIA;
char* PUERTO_MEMORIA;

char* IP_ENTRADASALIDA;
char* PUERTO_ENTRADASALIDA;

char* IP_CPU;
char* PUERTO_CPU_DISPATCH;
char* PUERTO_CPU_INTERRUPT;

char* ALGORITMO_PLANIFICACION;
int QUANTUM;
char** RECURSOS;
char** INSTANCIAS_RECURSOS;
int GRADO_MULTIPROGRAMACION;

pthread_mutex_t mutex_lista_ready;
pthread_mutex_t mutex_lista_ready_plus;
pthread_mutex_t mutex_lista_exec;
pthread_mutex_t mutex_lista_new;
pthread_mutex_t mutex_lista_blocked;
pthread_mutex_t mutex_lista_exit;
pthread_mutex_t mutex_procesos_en_core;

pthread_mutex_t mutex_ticket;
pthread_mutex_t mutex_pid;

t_list* ready;
t_list* ready_plus;
t_list* execute;
t_list* new;
t_list* blocked;
t_list* lista_exit;

sem_t sem_enviar_interrupcion;
sem_t sem_interrupt_pcp;
sem_t sem_interrupt_plp;
sem_t sem_estructura_iniciada_en_memoria;

int ALGORITMO_PCP_SELECCIONADO;

int ticket_actual = 0;    
int identificador_PID = 0;
int contador_pcbs = 0;
bool flag_exit = false;
int procesos_en_core = 0;;

#endif