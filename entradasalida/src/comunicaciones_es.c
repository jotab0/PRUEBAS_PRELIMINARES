# include "../include/comunicaciones_es.h"

void iterator(char* value){
	log_info(es_logger,"%s",value);
}

void esperar_kernel_es(){
    int estado_while = 1;
	t_list* lista;
    while (estado_while) {
		log_trace(es_logger,"E/S: ESPERANDO MENSAJES DE KERNEL...");
        int cod_op = recibir_operacion(fd_kernel);
		switch (cod_op) {
		case MENSAJE:
		 	recibir_mensaje_tp0(fd_kernel,es_logger);
			break;
		case PAQUETE:
			lista = recibir_paquete(fd_kernel);
			log_info(es_logger,"Me llegaron los siguientes mensajes:\n");
			list_iterate(lista,(void*)iterator);
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

void esperar_memoria_es(){
    int estado_while = 1;
	t_list* lista;
    while (estado_while) {
		log_trace(es_logger,"E/S: ESPERANDO MENSAJES DE MEMORIA...");
        int cod_op = recibir_operacion(fd_memoria);
        switch (cod_op) {
		case MENSAJE:
		 	recibir_mensaje_tp0(fd_memoria,es_logger);
			break;
		case PAQUETE:
			lista = recibir_paquete(fd_memoria);
			log_info(es_logger,"Me llegaron los siguientes mensajes:\n");
			list_iterate(lista,(void*)iterator);
			break;
			case -1:
				log_error(es_logger, "MEMORIA se desconecto. Terminando servidor");
				estado_while = 0;
            break;
		default:
			log_warning(es_logger,"Operacion desconocida");
		break;
		}
	}
}
