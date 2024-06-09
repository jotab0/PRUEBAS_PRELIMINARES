#ifndef M_GESTOR_H_
#define M_GESTOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>


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
	int size; 
	char* pathInstrucciones;
	t_list* lista_de_instrucciones;
	t_list* tabla_paginas;
	pthread_mutex_t mutex_tabla_paginas;
}t_proceso;


/// Marcos y Pagina ///  

typedef struct {
    int nro_marco;
	int num_pagina;
    int base;
    bool disponible;
	t_proceso* proceso;
	int cantidad_usado;
	bool queda_lugar_disponible;

} t_marco;

typedef struct {
	int nro_pagina; 
	int nro_marco;
	int tam_usado;
	t_proceso* proceso;
	
} t_pagina;

typedef struct{
	int num_pagina;
	int num_marco;

}t_tabla_de_pagina;
//-------------------------------------------------------------
// SEMAFOROS 




#endif