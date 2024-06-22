#ifndef INTERFACES_KERNEL_H_
#define INTERFACES_KERNEL_H_

#include "../include/k_gestor.h"
#include "../include/shared_kernel.h"
#include "../include/exit_process.h"
#include "../include/kernel_entradasalida.h"

void manejar_pedido_a_interfaz (pcb* un_pcb);
bool _evaluar_diponibilidad_pedido (pcb* un_pcb);
interfaz* _traer_interfaz_solicitada(pcb* un_pcb);

#endif /* INTERFACES_KERNEL_H_ */
