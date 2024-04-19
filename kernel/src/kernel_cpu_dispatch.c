#include "../include/kernel_cpu_dispatch.h"

void esperar_cpu_dispatch_kernel(){
    int estado_while = 1;
    while (estado_while) {
		log_trace(kernel_logger,"KERNEL: ESPERANDO MENSAJES DE CPU DISPATCH...");
        int cod_op = recibir_operacion(fd_cpu_dispatch);
		switch (cod_op) {
		case MENSAJE:
			log_trace(kernel_logger,"Entre al MENSAJE");
			char* mensaje = recibir_buffer(fd_cpu);
			log_info(kernel_logger, "Me llego el mensaje %s", mensaje);
			break;
		case PAQUETE:
			break;
		case -1:
			log_error(kernel_logger, "CPU DISPATCH se desconecto. Terminando servidor");
			estado_while = 0;
            break;
		default:
			log_warning(kernel_logger,"Operacion desconocida");
			break;
		}
	}
}