#ifndef KERNEL_CPU_DISPATCH_H_
#define KERNEL_CPU_DISPATCH_H_

#include "../include/k_gestor.h"

void esperar_cpu_dispatch_kernel();
void enviar_pcb_CPU_dispatch(pcb* un_pcb);

#endif /* KERNEL_CPU_DISPATCH_H_ */