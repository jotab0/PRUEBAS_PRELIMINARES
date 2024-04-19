
# include "../include/comunicaciones_memoria.h"

void iterator(char* value){
	log_info(memoria_logger,"%s",value);
}

void esperar_cpu_memoria(){
    int estado_while = 1;
	t_list* lista;
    while (estado_while) {
        log_trace(memoria_logger,"MEMORIA: ESPERANDO MENSAJES DE CPU...");
		int cod_op = recibir_operacion(fd_cpu);
		switch (cod_op) {
		case MENSAJE:
		 	recibir_mensaje_tp0(fd_cpu,memoria_logger);
			break;
		case PAQUETE:
			lista = recibir_paquete(fd_cpu);
			log_info(memoria_logger,"Me llegaron los siguientes mensajes:\n");
			list_iterate(lista,(void*)iterator);
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

void esperar_kernel_memoria(){
    int estado_while = 1;
    while (estado_while) {
		log_trace(memoria_logger,"MEMORIA: ESPERANDO MENSAJES DE KERNEL...");
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

void esperar_es_memoria(){
    int estado_while = 1;
    while (estado_while) {
		log_trace(memoria_logger,"MEMORIA: ESPERANDO MENSAJES DE E/S...");
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

