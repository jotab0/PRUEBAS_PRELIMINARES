#include "../include/kernel_entradasalida.h"

void esperar_conexiones_entradasalida(){ // PERMITE LA ESCUCHA A TRAVÉS DE MÚLTIPLES CANALES
	fd_kernel = iniciar_servidor(PUERTO_ESCUCHA, kernel_logger, "!! Servidor ENTRADA-SALIDA iniciado !!");
	while(1){
        
        int *fd_conexion_entradasalida = malloc(sizeof(int));
        *fd_conexion_entradasalida = esperar_cliente(fd_kernel, kernel_logger, "ENTRADA-SALIDA");

        pthread_t hilo_entradasalida;
        int err = pthread_create(&hilo_entradasalida, NULL,(void*)esperar_entradasalida_kernel, (int*)fd_conexion_entradasalida);
        if (err!=0){
            perror("Fallo de creación de hilo_entradasalida(kernel)\n");
            exit(-3);
        }
        pthread_detach(hilo_entradasalida);
    }
}

void esperar_entradasalida_kernel(int* fd_conexion_entradasalida){
    int estado_while = 1;
    while (estado_while) {
		log_trace(kernel_logger,"KERNEL: ESPERANDO MENSAJES DE E/S...");
        int cod_op = recibir_operacion(*fd_conexion_entradasalida);
		switch (cod_op) {

		case RESPUESTA_INSTRUCCION_KES: // [NOMBRE INTERFAZ][RESULTADO OPERACION]
		 	
			t_buffer* buffer = NULL;
			buffer = recibir_buffer(*fd_conexion_entradasalida);

			char* nombre_interfaz = extraer_string_del_buffer(buffer);
			int resultado_operacion = extraer_int_del_buffer(buffer);

			interfaz* una_interfaz = obtener_interfaz_con_nombre(nombre_interfaz);
			una_interfaz->resultado_operacion_solicitada = resultado_operacion;

			sem_post(&una_interfaz->sem_instruccion_interfaz);
			
			destruir_buffer(buffer);
			
			break;

		case HANDSHAKE_K_ES: // AGREGA A LISTA DE INTERFACES CONECTADAS
			
			buffer = NULL;
			buffer = recibir_buffer(*fd_conexion_entradasalida);

			interfaz* nueva_interfaz = NULL;
			nueva_interfaz = _crear_instancia_interfaz(buffer,fd_conexion_entradasalida);

			list_add_sync(interfaces_conectadas,nueva_interfaz,&mutex_lista_interfaces);
			
			destruir_buffer(buffer);

			break;

		case -1:
			log_error(kernel_logger, "E/S se desconecto. Terminando servidor");
			// Acá debería liberar todos los mutexes frenados, como puedo hacerlo de una?
			// El semáforo tengo que liberar uno solo porque siempre frenan de a uno ahí
			estado_while = 0;
            break;
			
		default:
			log_warning(kernel_logger,"Operacion desconocida de E/S");
			break;
		}
	}
	free(fd_conexion_entradasalida);
}

// LA IDEA ES CREAR HILO POR PEDIDO
interfaz* _crear_instancia_interfaz(t_buffer* buffer, int* fd_conexion_entradasalida){
	
	interfaz* una_interfaz = NULL; 
	una_interfaz->nombre_interfaz = extraer_string_del_buffer(buffer);
	una_interfaz->resultado_operacion_solicitada = OK;
	
	while(buffer->size > 0){
		instruccion_interfaz una_instruccion = extraer_int_del_buffer(buffer);
		list_add(una_interfaz->instrucciones_disponibles,&una_instruccion);
	} 
	
	una_interfaz->fd_conexion=fd_conexion_entradasalida;
	pthread_mutex_init(&una_interfaz->mutex_interfaz, NULL);
	sem_init(&una_interfaz->sem_interfaz,0,0);
	sem_init(&una_interfaz->sem_instruccion_interfaz,0,0);

	return una_interfaz;
}

int solicitar_instruccion_a_interfaz(pcb* un_pcb, interfaz* una_interfaz){
	
	t_paquete* paquete = NULL;
	
	paquete = crear_paquete_con_buffer(un_pcb->pedido_a_interfaz->instruccion_a_interfaz);
	cargar_int_a_paquete(paquete,un_pcb->pid);
	
	enviar_paquete(paquete,*(una_interfaz->fd_conexion));
	sem_wait(&una_interfaz->sem_instruccion_interfaz);
	
	// Desbloqueo interfaz que bloquie en "_evaluar_diponibilidad_pedido"
	pthread_mutex_unlock(&una_interfaz->mutex_interfaz);
	
	// Aviso que se realizó instrucción
	sem_post(&una_interfaz->sem_interfaz); 

	return una_interfaz->resultado_operacion_solicitada;
}

