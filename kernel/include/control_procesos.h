#ifndef CONTROL_PROCESOS_H_
#define CONTROL_PROCESOS_H_

#include "k_gestor.h"
#include "kernel_cpu_dispatch.h"
#include "servicios_kernel.h"

void cambiar_estado(pcb* un_pcb, estado_pcb nuevo_estado);
pcb* crear_pcb(char* path, char* size, char* prioridad);
void atender_FIFO_RR();
void _programar_interrupcion_por_quantum();


int TAM_QUANTUM = 50;

int ticket_actual = 0;
int identificador_PID = 0;
int contador_pcbs = 0;
bool flag_exit = false;


#endif