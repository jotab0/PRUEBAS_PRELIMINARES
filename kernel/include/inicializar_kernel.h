#ifndef INICIALIZAR_KERNEL_H_
#define INICIALIZAR_KERNEL_H_

#include "k_gestor.h"

void inicializar_kernel();
void inicializar_logs();
void inicializar_configs();
void crear_listas();
void inicializar_semaforos();
void inicializar_mutexes();

pthread_mutex_t mutex_lista_ready;
pthread_mutex_t mutex_lista_exec;
pthread_mutex_t mutex_ticket;
pthread_mutex_t mutex_flag_exit;
pthread_mutex_t mutex_pid;

t_list* ready;
t_list* execute;
t_list* new;
t_list* blocked;
t_list* lista_exit;

sem_t sem_enviar_interrupcion;

#endif