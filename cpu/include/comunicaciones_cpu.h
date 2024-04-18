#ifndef CPU_OPERACIONES_H_
#define CPU_OPERACIONES_H_

#include "c_gestor.h"

void esperar_kernel_dispatch_cpu();
void esperar_kernel_interrupt_cpu();
void esperar_memoria_cpu();

#endif