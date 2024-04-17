# include "../include/comunicaciones_es.h"


void esperar_cpu(){
    int estado_while = 0;
    while (estado_while) { //Bucle que queda esperando la operación
        int cod_op = recibir_operacion(fd_memoria);
		switch (cod_op) {
		case MENSAJE:
			break;
		case PAQUETE:
			break;
		case -1:
			log_error(es_logger, "CPU se desconecto. Terminando servidor");
			estado_while = 0;
            break;
		default:
			log_warning(es_logger,"Operacion desconocida");
			break;
		}
	}
}

void esperar_kernel(){
    int estado_while = 0;
    while (estado_while) { //Bucle que queda esperando la operación
        int cod_op = recibir_operacion(fd_memoria);
        switch (cod_op) {
		case MENSAJE:
			break;
		case PAQUETE:
			break;
		case -1:
			log_error(es_logger, "KERNEL se desconecto. Terminando servidor");
			estado_while = 0;
            break;
		default:
			log_warning(es_logger,"Operacion desconocida");
			break;
		}
	}
}
