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
			nueva_interfaz = _crear_instancia_interfaz(buffer,fd_conexion_entradasalida);

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
interfaz* _crear_instancia_interfaz(t_buffer* buffer, int* fd_conexion_entradasalida){
	
	interfaz* una_interfaz = NULL; 
	una_interfaz->nombre_interfaz = extraer_string_del_buffer(buffer);
	
	while(buffer->size > 0){
		instruccion_interfaz una_instruccion = extraer_int_del_buffer(buffer);
		list_add(una_interfaz->instrucciones_disponibles,&una_instruccion);
	} 

	una_interfaz->fd_conexion=fd_conexion_entradasalida;
	pthread_mutex_init(&una_interfaz->mutex_interfaz, NULL);
	sem_init(&una_interfaz->sem_interfaz,0,0);

	return una_interfaz;
}

int solicitar_instruccion_a_interfaz(pcb* un_pcb){
	
	// Lógica que hace que se ejecute instrucción
	interfaz* una_interfaz = NULL;
	// una_interfaz = _obtener_interfaz_con_nombre(un_pcb); Le tengo que pasar la interfaz por parámetro
	if(una_interfaz == NULL){
		//El tema de bloqueos de semáforos se resuelve al handlear la eliminación de la interfaz de la lista de interfaces conectadas
		return 1;
	}else{
		
		// Lógica para solicitar instrucción
		// ...
 		
		// Desbloqueo interfaz 
		pthread_mutex_unlock(&una_interfaz->mutex_interfaz);
		// Aviso que se realizó instrucción
		sem_post(&una_interfaz->sem_interfaz); 
		return 0;
	}
}

interfaz* _obtener_interfaz_con_nombre(pcb* un_pcb){
	
	bool _buscar_interfaz(interfaz* una_interfaz){
		
		char* nombre_encontrado = una_interfaz->nombre_interfaz;
		char* nombre_buscado = un_pcb->pedido_a_interfaz->nombre_interfaz;
		
		return strcmp(nombre_encontrado,nombre_buscado)==1;
	}

	interfaz* una_interfaz = NULL;
	if(list_any_satisfy(interfaces_conectadas,(void*)_buscar_interfaz)){
		return una_interfaz = list_find(interfaces_conectadas,(void*)_buscar_interfaz); // Por qué no me la reconoce?
	}
	else{
		log_error(kernel_logger,"Se solicitó interfaz que ya no se encuentra conectada");
		return una_interfaz;
	}
}