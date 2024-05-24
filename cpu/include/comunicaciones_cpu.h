#ifndef CPU_OPERACIONES_H_
#define CPU_OPERACIONES_H_

#include "c_gestor.h"
#include <string.h>

//char** instruccion_dividida;

void esperar_kernel_cpu_dispatch();
void esperar_kernel_cpu_interrupt();
void esperar_memoria_cpu();

void recibir_instruccion(t_buffer*);
char** string_split(char*, char*);
int contar_delimitadores(char*, char*);

void recibir_pcb_del_kernel(t_buffer*);
void iniciar_estructuras_para_recibir_pcb(t_buffer*);
void mostrar_pcb();

void atender_interrupcion(t_buffer*);

pthread_mutex_t mutex_manejo_contexto;

typedef struct{
    int proceso_pid;
    int proceso_pc;
    int proceso_tiempo_ejecutado;
    int proceso_ticket;
    uint32_t AX;
    uint32_t BX; 
    uint32_t CX;
    uint32_t DX;
}t_contexto;

t_contexto* contexto;

//t_log* cpu_logger;
//t_log* cpu_log_obligatorio;

//char* motivo_bloqueo;

#endif