#ifndef M_GESTOR_H_
#define M_GESTOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <utils/include/shared.h> 

#include <commons/log.h>
#include <commons/config.h>

//-------------------------------------------------------------
// VARIABLES GLOBALES 

extern t_log* memoria_logger;
extern t_log* memoria_logger_extra;
extern t_config* memoria_config;

extern char* PUERTO_ESCUCHA;
extern int TAM_MEMORIA;
extern int TAM_PAGINA;
extern char* PATH_INSTRUCCIONES;
extern int RETARDO_RESPUESTA;


extern int fd_memoria;
extern int fd_cpu;
extern int fd_es;
extern int fd_kernel;


extern t_list* lista_procesos;
extern t_list* list_pagina;
extern t_list* lista_marcos;

//-------------------------------------------------------------
// STRUCTS 

/// Proceso ///

typedef struct {
    int pid_proceso;
	int size; // preguntar el size de donde lo saco 
	char* pathInstrucciones;
	t_list* lista_de_instrucciones;
	t_list* tabla_paginas;
	pthread_mutex_t mutex_tabla_paginas;
}t_proceso;


/// Marcos y Pagina ///  

typedef struct {
    int nro_marco;
    int base;
    bool disponible;
	t_proceso* proceso;
	int num_pagina;
} t_marco;

typedef struct {
	int nro_pagina; 
	int nro_marco;
	
} t_pagina;

typedef struct{
	int num_pagina;
	int num_frame;

}t_tabla_de_pagina;
//-------------------------------------------------------------
// SEMAFOROS 

extern pthread_mutex_t mutex_lista_marcos;
extern pthread_mutex_t mutex_espacio_usuario;
extern pthread_mutex_t mutex_carga_global;
extern sem_t sem_swap;
extern sem_t sem_pagefault;

extern int ordenCargaGlobal;

#endif