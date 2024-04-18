#include "../include/comunicaciones_cpu.h"

void esperar_kernel_cpu_interrupt(){
    int estado_while = 1;
    while (estado_while) {
		log_trace(cpu_logger,"CPU INTERRUPT: ESPERANDO MENSAJES DE KERNEL...");
        int cod_op = recibir_operacion(fd_kernel_interrupt);
		switch (cod_op) {
		case MENSAJE:
			break;
		case PAQUETE:
			break;
		case -1:
			log_error(cpu_logger, "KERNEL se desconecto de cpu interrupt. Terminando servidor");
			estado_while = 0;
            break;
		default:
			log_warning(cpu_logger,"Operacion desconocida");
			break;
		}
	}
}

void esperar_kernel_cpu_dispatch(){
    int estado_while = 1;
    while (estado_while) {
		log_trace(cpu_logger,"CPU DISPATCH: ESPERANDO MENSAJES DE KERNEL...");
        int cod_op = recibir_operacion(fd_kernel_dispatch);
		switch (cod_op) {
		case MENSAJE:
			break;
		case PAQUETE:
			break;
		case -1:
			log_error(cpu_logger, "KERNEL se desconecto de cpu dispatch. Terminando servidor");
			estado_while = 0;
            break;
		default:
			log_warning(cpu_logger,"Operacion desconocida");
			break;
		}
	}
}

void esperar_memoria_cpu(){
    int estado_while = 1;
    while (estado_while) {
		log_trace(cpu_logger,"CPU: ESPERANDO MENSAJES DE MEMORIA");
        int cod_op = recibir_operacion(fd_memoria);
		switch (cod_op) {
		case MENSAJE:
			break;
		case PAQUETE:
			break;
		case -1:
			log_error(cpu_logger, "MEMORIA se desconecto. Terminando servidor");
			estado_while= 0;
            break;
		default:
			log_warning(cpu_logger,"Operacion desconocida");
			break;
		}
	}
}