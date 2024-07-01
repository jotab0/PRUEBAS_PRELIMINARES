#ifndef REALIZAROP_ES
#define REALIZAROP_ES

#include "../include/es_gestor.h"


// =========== ATENDER KERNEL ===============

void atender_es_kernel();
void atender_peticion_de_gen(t_buffer* un_buffer);
void atender_peticion_de_stdin(t_buffer* un_buffer);
void atender_peticion_de_stdout(t_buffer* un_buffer);

// =========== ENVIAR A KERNEL ============================

void respuesta_de_operacion_gen_a_kernel(char* nombre_interfaz, int pid);
void respuesta_de_operacion_stdin_a_kernel(char* nombre_interfaz, int pid);
void respuesta_de_operacion_stdout_a_kernel(char* nombre_interfaz, int pid);

// =========== ENVIAR A MEMORIA ===============

void enviar_contenido_a_memoria_stdin(int pid, int una_direccion,int tam_contenido_ingresado, char* contenido_ingresado);
void enviar_contenido_a_memoria_stdout(int pid, int una_direccion, int tamanio_direcc);

// =========== REC MEMORIA ===============
char* recibir_datos_a_escribir_de_memoria(t_buffer* un_buffer);

void mostrar_datos_leidos(char* datos_leidos, int tamanio_direcc);

#endif