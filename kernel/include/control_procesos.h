#ifndef CONTROL_PROCESOS_H_
#define CONTROL_PROCESOS_H_

#include "k_gestor.h"
#include "kernel_cpu_dispatch.h"
#include "kernel_cpu_interrupt.h"
#include "kernel_entradasalida.h"
#include "kernel_memoria.h"
#include "shared_kernel.h"

void cambiar_estado_pcb(pcb* un_pcb, estado_pcb nuevo_estado);
pcb* crear_pcb(char* path);
void agregar_a_ready(pcb* un_pcb);

void planificar_corto_plazo();
void _programar_interrupcion_por_quantum_RR(pcb* un_pcb);
void _programar_interrupcion_por_quantum_VRR(pcb* un_pcb);
void _poner_en_ejecucion(pcb* un_pcb);
void planificador_corto_plazo();
void _check_interrupt_pcp();

void planificador_largo_plazo();
void _check_interrupt_plp();

void manejar_bloqueo_de_proceso(pcb* un_pcb);
void manejar_pedido_a_interfaz (pcb* un_pcb);
bool _evaluar_diponibilidad_pedido (pcb* un_pcb);
interfaz* _traer_interfaz_solicitada(pcb* un_pcb);


void planificar_proceso_exit(pcb* un_pcb);
void planificar_proceso_exit_en_hilo(pcb* un_pcb);

void manejar_pedido_de_recurso(pcb *pcb_recibido);
void control_request_de_recursos(instancia_recurso* un_recurso);
void agregar_recurso_a_pcb (pcb* un_pcb, char* un_recurso);
void manejar_signal_de_recurso(pcb *pcb_recibido);
void quitar_recurso (pcb* un_pcb, char* un_recurso);

#endif