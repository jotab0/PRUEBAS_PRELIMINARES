#ifndef CPU_OPERACIONES_H_
#define CPU_OPERACIONES_H_

#include "c_gestor.h"

char** instruccion_dividida;

void esperar_kernel_cpu_dispatch();
void esperar_kernel_cpu_interrupt();
void esperar_memoria_cpu();

void recibir_instruccion(t_buffer*);

#endif