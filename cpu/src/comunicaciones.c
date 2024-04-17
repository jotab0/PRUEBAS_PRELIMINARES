#include "../include/comunicaciones.h"

void esperar_kernel_dispatch(){
    int estado_while = 0;
    while (estado_while) { //Bucle que queda esperando la operación
        int cod_op = recibir_operacion(fd_cpu_dispatch);
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

void esperar_kernel_interrupt(){
    int estado_while = 0;
    while (estado_while) { //Bucle que queda esperando la operación
        int cod_op = recibir_operacion(fd_cpu_interrupt);
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

void esperar_memoria(){
    int estado_while= 0;
    while (estado_while) { //Bucle que queda esperando la operación
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