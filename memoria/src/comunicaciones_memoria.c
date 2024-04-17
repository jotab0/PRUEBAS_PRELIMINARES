
# include "../include/comunicaciones_memoria.h"

void esperar_cpu(){
    int estado_while = 0;
    while (estado_while) { //Bucle que queda esperando la operación
        int cod_op = recibir_operacion(fd_cpu);
		switch (cod_op) {
		case MENSAJE:
			break;
		case PAQUETE:
			break;
		case -1:
			log_error(memoria_logger, "CPU se desconecto. Terminando servidor");
			estado_while = 0;
            break;
		default:
			log_warning(memoria_logger,"Operacion desconocida");
			break;
		}
	}
}

void esperar_kernel(){
    int estado_while = 0;
    while (estado_while) { //Bucle que queda esperando la operación
        int cod_op = recibir_operacion(fd_kernel);
		switch (cod_op) {
		case MENSAJE:
			break;
		case PAQUETE:
			break;
		case -1:
			log_error(memoria_logger, "KERNEL se desconecto. Terminando servidor");
			estado_while = 0;
            break;
		default:
			log_warning(memoria_logger,"Operacion desconocida");
			break;
		}
	}
}

void esperar_es(){
    int estado_while = 0;
    while (estado_while) { //Bucle que queda esperando la operación
        int cod_op = recibir_operacion(fd_es);
		switch (cod_op) {
		case MENSAJE:
			break;
		case PAQUETE:
			break;
		case -1:
			log_error(memoria_logger, "ENTRADASALIDA se desconecto. Terminando servidor");
			estado_while = 0;
            break;
		default:
			log_warning(memoria_logger,"Operacion desconocida");
			break;
		}
	}
}
