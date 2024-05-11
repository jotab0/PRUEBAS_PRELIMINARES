#ifndef KERNEL_ENTRADASALIDA_H_
#define KERNEL_ENTRADASALIDA_H_

#include "../include/k_gestor.h"
#include "../include/shared_kernel.h"
#include "../include/control_procesos.h"

void esperar_entradasalida_kernel(int *fd_conexion_entradasalida);
void esperar_conexiones_entradasalida();
interfaz* _crear_instancia_interfaz(t_buffer* buffer, int* fd_conexion_entradasalida);
int solicitar_instruccion_a_interfaz(pcb* un_pcb, interfaz* una_interfaz);


#endif /* KERNEL_ENTRADASALIDA_H_ */