#ifndef CICLOINSTRUCCION_CPU_H
#define CICLOINSTRUCCION_CPU_H

#include "c_gestor.h"

#include <sys/time.h>
#include <stdint.h>

void realizarCicloInstruccion();
void ciclo_de_instruccion_fetch();
void decodeYExecute();

uint32_t* detectar_registro(char*);
void enviarContextoAKernel(t_paquete*);

bool hayQueDesalojar;
//bool hay_interrupcion;
bool hay_interrupcion_quantum;
bool hay_interrupcion_exit;

//char* motivo_bloqueo;

struct timeval inicio, fin;

char* leer_valor_memoria(int, int);
void escribir_valor_memoria(int, char*, int);
int traducir(int);
int mmu(int, int, int);
int buscar_en_TLB(int);
void agregar_entrada_TLB(int, int);

//t_log* cpu_logger;
//t_log* cpu_log_obligatorio;

char** opcode_headers;

//char** instruccion_dividida;

/*
//Semaforos
sem_t sem_control_fetch_decode;
sem_t sem_control_decode_execute;
sem_t sem_control_execute_checkinterruption; //hace falta este?
*/

/*
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
*/

#endif
