#ifndef KERNEL_ENTRADASALIDA_H_
#define KERNEL_ENTRADASALIDA_H_

#include "../include/k_gestor.h"
#include "../include/shared_kernel.h"

void esperar_entradasalida_kernel(int *fd_conexion_entradasalida);
void esperar_conexiones_entradasalida();
interfaz* _crear_instancia_interfaz(t_buffer* buffer);
void solicitar_instruccion_a_interfaz(pcb* un_pcb);

#endif /* KERNEL_ENTRADASALIDA_H_ */