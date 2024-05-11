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
		case MENSAJE:
		 	recibir_mensaje_tp0(*fd_conexion_entradasalida,kernel_logger);
			break;
		case HANDSHAKE_K_ES: // AGREGA A LISTA DE INTERFACES CONECTADAS
			
			t_buffer* buffer = NULL;
			buffer = recibir_buffer(*fd_conexion_entradasalida);

			interfaz* nueva_interfaz = NULL;
			nueva_interfaz = _crear_instancia_interfaz(buffer);

			list_add_sync(interfaces_conectadas,nueva_interfaz,&mutex_lista_interfaces);
			
			destruir_buffer(buffer);

			break;

		case -1:
			log_error(kernel_logger, "E/S se desconecto. Terminando servidor");
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
interfaz* _crear_instancia_interfaz(t_buffer* buffer){
	
	interfaz* una_interfaz = NULL; 
	una_interfaz->nombre_interfaz = extraer_string_del_buffer(buffer);
	
	while(buffer->size > 0){
		instruccion_interfaz una_instruccion = extraer_int_del_buffer(buffer);
		list_add(una_interfaz->instrucciones_disponibles,&una_instruccion);
	} 

	return una_interfaz;
}

void solicitar_instruccion_a_interfaz(pcb* un_pcb){
	
	// Prguntar que le debería mandar
	
	sem_post(&sem_solicitud_interfaz);
	// Que es lo que tiene que recibir la interfaz?
}