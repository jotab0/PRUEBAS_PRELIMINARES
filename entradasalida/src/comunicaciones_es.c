# include "../include/comunicaciones_es.h"

void iterator(char* value){
	log_info(es_logger,"%s",value);
}

void enviar_handshake(){
	
	t_paquete* un_paquete = crear_paquete_con_buffer(HANDSHAKE_K_ES);

	cargar_string_a_paquete(un_paquete,"una_interfaz");
	cargar_int_a_paquete(un_paquete,IO_GEN_SLEEP);
	cargar_int_a_paquete(un_paquete,IO_FS_CREATE);
	cargar_int_a_paquete(un_paquete,IO_FS_WRITE);
	
	log_info(es_logger,"Enviando interfaz a kernel...");
	enviar_paquete(un_paquete,fd_kernel);
	destruir_paquete(un_paquete);
}

void esperar_kernel_es(){
    int estado_while = 1;
	sleep(5);
	enviar_handshake();
    while (estado_while) {
		log_trace(es_logger,"E/S: ESPERANDO MENSAJES DE KERNEL...");
        int cod_op = recibir_operacion(fd_kernel);
		switch (cod_op) {

		case IO_GEN_SLEEP:

			t_buffer* un_buffer = recibir_buffer(fd_kernel);
			char* nombre_interfaz = extraer_string_del_buffer(un_buffer);
			int pid = extraer_int_del_buffer(un_buffer);
			int un_tiempo = extraer_int_del_buffer(un_buffer);

			log_info(es_logger,"Llegó solicitud para %s de IO_GEN_SLEEP por parte de proceso con PID: %d", nombre_interfaz,pid);
			log_info(es_logger,"Tiempo solicitado: %d", un_tiempo);

			destruir_buffer(un_buffer);

			t_paquete* un_paquete = crear_paquete_con_buffer(RESPUESTA_INSTRUCCION_KES);
			cargar_string_a_paquete(un_paquete,nombre_interfaz);
			cargar_int_a_paquete(un_paquete,OK);

			sleep(10);

			enviar_paquete(un_paquete,fd_kernel);
			destruir_paquete(un_paquete);
		
			break;

		case IO_FS_WRITE:
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

    while (estado_while) {
		log_trace(es_logger,"E/S: ESPERANDO MENSAJES DE MEMORIA...");
        int cod_op = recibir_operacion(fd_memoria);
        switch (cod_op) {
		case MENSAJE:
		 	recibir_mensaje_tp0(fd_memoria,es_logger);
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

