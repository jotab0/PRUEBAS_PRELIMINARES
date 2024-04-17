#ifndef SHARED_H_
#define SHARED_H_

#include <stdlib.h>
#include <stdio.h>

//las agregue 16/04 17:15hs
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#include <commons/log.h> 
#include <commons/config.h>  // de esta manera agregamos librerías de las commons

typedef enum {
    MENSAJE,
    PAQUETE,
    HANDSHAKE,
    RTA_HANDSHAKE,
      //KERNEL-MEMORIA
    CREAR_PROCESO,
    RTA_CREAR_PROCESO
    // SEPARAR SEGUN TIPO:
    //KERNEL-CPU
    //KERNEL-ES
    //ES-MEMORIA
    //CPU-MEMORIA
}op_code;

typedef struct 
{
    int size;
    void* stream;
}t_buffer;

typedef struct{
    op_code codigo_operacion;
    t_buffer* buffer;
}t_paquete;



int crear_conexion(char *ip, char* puerto);
int iniciar_servidor(char* puerto,t_log* logger,char* mensaje);
int esperar_cliente(int socket_servidor,t_log* logger,char* mensaje);
int recibir_operacion(int socket_cliente);

#endif