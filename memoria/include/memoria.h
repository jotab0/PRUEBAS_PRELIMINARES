#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "m_gestor.h"
#include "../include/inicializar_memoria.h"
#include "../include/comunicaciones_memoria.h"
#include "../include/encargarse_cpu.h"
#include "../include/encargarse_kernel.h"
#include  "../include/encargarse_es.h"
#include "../include/operaciones_proceso.h"
#include "../include/espacio_usuario.h"
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


char* servidor_memoria_nombre;

// variables espacio usuario y paginacion 

void* espacio_usuario;
t_dictionary *  tablas; 
t_list *  lista_marcos;
int  cantidad_marcos;

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
