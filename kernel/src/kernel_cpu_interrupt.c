#include "../include/kernel_cpu_interrupt.h"

void esperar_cpu_interrupt_kernel(){
    int estado_while = 1;
    while (estado_while) {
		log_trace(kernel_logger,"KERNEL: ESPERANDO MENSAJES DE CPU INTERRUPT...");
        int cod_op = recibir_operacion(fd_cpu_interrupt);
		switch (cod_op) {
		case MENSAJE:
			break;
		case PAQUETE:
			break;
		case -1:
			log_error(kernel_logger, "CPU INTERRUPT se desconecto. Terminando servidor");
			estado_while = 0;
            break;
		default:
			log_warning(kernel_logger,"Operacion desconocida");
			break;
		}
	}
}