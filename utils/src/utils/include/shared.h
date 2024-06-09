#ifndef SHARED_H_
#define SHARED_H_

#include <stdlib.h>
#include <stdio.h>

//las agregue 16/04 17:15hs
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netdb.h>
#include <string.h>
#include <semaphore.h>

#include <commons/log.h> 
#include <commons/config.h>  // de esta manera agregamos librerías de las commons

typedef enum {
    MENSAJE,
    PAQUETE,
    HANDSHAKE,
    RTA_HANDSHAKE,
    //**KERNEL-MEMORIA**
    CREAR_PROCESO,
    RTA_CREAR_PROCESO,
    INICIAR_ESTRUCTURA,
    RTA_INICIAR_ESTRUCTURA,
    LIBERAR_ESTRUCTURAS,
    // SEPARAR SEGUN TIPO:
    //**KERNEL-CPU**
    EJECUTAR_PROCESO_KCPU,

    DESALOJAR_PROCESO_KCPU,
    INTERRUPCION,
    ATENDER_INSTRUCCION_CPU,
    ATENDER_INTERRUPCION,

    //**KERNEL-ES**
    HANDSHAKE_K_ES,
    RESPUESTA_INSTRUCCION_KES,

    //**ES-MEMORIA**

    //**CPU-MEMORIA**
    SOLICITUD_INFO_MEMORIA,
    SOLICITUD_INSTRUCCION,
    RTA_AJUSTAR_TAMANIO,
    SOLICITUD_CONSULTA_PAG,
    SOLICITUD_PAGINA,
    AJUSTAR_TAMANIO,
    SOLICITUD_LECTURA_MEMORIA_BLOQUE,
    SOLICITUD_ESCRITURA_MEMORIA_BLOQUE
}op_code;

typedef struct {
    int size;
    void* stream; // TAM + MSJE + ...
}t_buffer;

typedef enum{
	OK,
	ERROR
}resultado_operacion;

typedef struct{
    op_code codigo_operacion;
    t_buffer* buffer;
}t_paquete;

typedef enum{
	MEMORIA,
	FILESYSTEM,
	CPU,
	KERNEL
}modulo_identificador;

typedef enum{
	IO_GEN_SLEEP,
    IO_STDIN_READ,
    INSTRUCCION_IO_NO_DEFINIDA
}instruccion_interfaz;

typedef struct{
	char* pseudo_codigo;
    char* primer_parametro;
    char* segundo_parametro;
}t_instruccion_codigo;



int crear_conexion(char *ip, char* puerto);
int iniciar_servidor(char* puerto,t_log* logger,char* mensaje);
int esperar_cliente(int socket_servidor,t_log* logger,char* mensaje);
int recibir_operacion(int socket_cliente);
t_buffer* crear_buffer();
void destruir_buffer(t_buffer* buffer);
void cargar_mensaje_a_buffer(t_buffer* buffer, void* mensaje, int tam_mensaje);
void cargar_int_a_buffer(t_buffer* buffer, int valor);
void cargar_uint32_a_buffer(t_buffer* buffer, uint32_t valor);
void cargar_string_a_buffer(t_buffer* buffer, char* string);
void* extraer_mensaje_de_buffer(t_buffer* buffer);
int extraer_int_del_buffer(t_buffer* buffer);
char* extraer_string_del_buffer(t_buffer* buffer);
uint32_t extraer_uint32_del_buffer(t_buffer* buffer);
void destruir_paquete(t_paquete* paquete);
void* serializar_paquete(t_paquete* paquete);
void enviar_paquete(t_paquete* paquete, int fd);
t_buffer* recibir_buffer(int socket_cliente);
// LITERAL tp0
void recibir_mensaje_tp0(int socket_cliente, t_log* logger);
t_list* recibir_paquete(int socket_cliente);
void enviar_mensaje(char* mensaje, int socket_cliente);
void* recibir_buffer_tp0(int* size, int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
t_paquete* crear_paquete_con_buffer(op_code codigo_operacion);
void crear_buffer_en_paquete(t_paquete* paquete);
void cargar_int_a_paquete(t_paquete* paquete, int valor);
void cargar_uint32_a_paquete(t_paquete* paquete, uint32_t valor);
void cargar_string_a_paquete(t_paquete* paquete, char* string);
//FUNCIONES VARIAS
void ejecutar_en_hilo_detach(void (*una_funcion)(void*) ,void* struct_argumento);
void ejecutar_en_hilo_join(void (*f)(void*) ,void* struct_arg);
t_buffer* recibir_un_paquete(int conexion);


#endif
