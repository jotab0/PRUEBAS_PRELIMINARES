#ifndef CONTROL_PROCESOS_H_
#define CONTROL_PROCESOS_H_

#include "k_gestor.h"
#include "kernel_cpu_dispatch.h"
#include "kernel_memoria.h"
#include "shared_kernel.h"

void cambiar_estado_pcb(pcb* un_pcb, estado_pcb nuevo_estado);
pcb* crear_pcb(char* path, int size_path);
void planificar_corto_plazo();
void _programar_interrupcion_por_quantum_RR(pcb* un_pcb);
void _programar_interrupcion_por_quantum_VRR(pcb* un_pcb);
void _poner_en_ejecucion(pcb* un_pcb);
void planificador_corto_plazo();
void planificador_largo_plazo();

void _check_interrupt_pcp();
void _check_interrupt_plp();

#endif