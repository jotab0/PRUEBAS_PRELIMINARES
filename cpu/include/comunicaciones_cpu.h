#ifndef CPU_OPERACIONES_H_
#define CPU_OPERACIONES_H_

#include "c_gestor.h"

void esperar_kernel_cpu_dispatch();
void esperar_kernel_cpu_interrupt();
void esperar_memoria_cpu();

pthread_mutex_t mutex_manejo_contexto;

pthread_mutex_t mutex_manejo_contexto;

t_log* cpu_logger;
t_log* cpu_log_obligatorio;

#endif