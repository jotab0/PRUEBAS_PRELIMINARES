#include "../include/kernel_entradasalida.h"

void esperar_entradasalida_kernel(){
    int estado_while = 1;
    while (estado_while) {
		log_trace(kernel_logger,"KERNEL: ESPERANDO MENSAJES DE E/S...");
        int cod_op = recibir_operacion(fd_entradasalida);
		switch (cod_op) {
		case MENSAJE:
			break;
		case PAQUETE:
			break;
		case -1:
			log_error(kernel_logger, "E/S se desconecto. Terminando servidor");
			estado_while = 0;
            break;
		default:
			log_warning(kernel_logger,"Operacion desconocida");
			break;
		}
	}
}