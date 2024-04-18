#include "../include/comunicaciones_cpu.h"

void esperar_kernel_dispatch_cpu(){
    int estado_while = 1;
    while (estado_while) {
		log_info(cpu_logger, "Hilo cpu dispatch <- kerenel creado");
        int cod_op = recibir_operacion(fd_kernel_dispatch);
		switch (cod_op) {
		case MENSAJE:
			break;
		case PAQUETE:
			break;
		case -1:
			log_error(cpu_logger, "KERNEL (Dispatch) se desconecto. Terminando servidor");
			estado_while = 0;
            break;
		default:
			log_warning(cpu_logger,"Operacion desconocida");
			break;
		}
	}
}

void esperar_kernel_interrupt_cpu(){
    int estado_while = 1;
    while (estado_while) {
		log_info(cpu_logger, "Hilo cpu interrupt <- kerenel creado"); //Bucle que queda esperando la operación
        int cod_op = recibir_operacion(fd_kernel_interrupt);
		switch (cod_op) {
		case MENSAJE:
			break;
		case PAQUETE:
			break;
		case -1:
			log_error(cpu_logger, "KERNEL (Interrupt) se desconecto. Terminando servidor");
			estado_while= 0;
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
		log_trace(cpu_logger,"ESPERANDO MENSAJES DE MEMORIA");
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