#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "m_gestor.h"
#include "../include/inicializar_memoria.h"
#include "../include/comunicaciones_memoria.h"
#include "../include/encargarse_cpu.h"
#include "../include/encargarse_kernel.h"
#include "../include/operaciones_proceso.h"
#include <utils/include/shared.h>
#include <pthread.h>

// VARIABLES GLOBALES
t_log* memoria_logger;
t_log* memoria_logger_extra;

t_config* memoria_config;

int fd_memoria;
int fd_cpu;
int fd_es;
int fd_kernel;
int servidor_fd_memoria;
int servidor_escucha;

// Variable de procesos en memoria
t_list* lista_procesos;

// Variables de config
char* PUERTO_ESCUCHA;
int TAM_MEMORIA;
int TAM_PAGINA;
char* PATH_INSTRUCCIONES;
int RETARDO_RESPUESTA;
char* IP_MEMORIA;

#endif
