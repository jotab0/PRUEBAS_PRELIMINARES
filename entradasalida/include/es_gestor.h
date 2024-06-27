#ifndef ES_H_
#define ES_H_

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include <utils/include/shared.h> 



struct fcb
{
    uint32_t BLOQUE_INICIAL;
    uint32_t TAMANIO_ARCHIVO;
};
typedef struct fcb t_fcb;

struct bitmap
{
    char *direccion;
    uint32_t tamanio;
    t_bitarray *bitarray;
};
typedef struct bitmap t_bitmap;

// VARIABLES GLOBALES

// LOGS Y CONFIGS
extern t_log* es_logger;
extern t_log* es_logger_extra;

extern t_config* es_config;

// Variables de config
extern char* TIPO_INTERFAZ;
extern int TIEMPO_UNIDAD_TRABAJO;

extern char* IP_KERNEL;
extern char* PUERTO_KERNEL;

extern char* IP_MEMORIA;
extern char* PUERTO_MEMORIA;

extern char* PATH_BASE_DIALFS;
extern int BLOCK_SIZE;
extern int BLOCK_COUNT;

// FILE DESCRIPTORS
extern int fd_kernel;
extern int fd_memoria;

typedef struct {
    char* nombre_interfaz;
    char* tipo_interfaz;
    int tiempo_unidad_trabajo;
    char* ip_kernel;
    int puerto_kernel;
} configuracion_interfaz;


#endif