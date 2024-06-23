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
			ejecutar_en_hilo_detach((void*)control_request_de_interfaz,nueva_interfaz);
			
			destruir_buffer(buffer);

			break;

		case -1:
		
			log_error(kernel_logger, "E/S se desconecto. Terminando servidor");
			
			limpiar_interfaz(fd_conexion_entradasalida);

			estado_while = 0;
            break;
			
		default:
			log_warning(kernel_logger,"Operacion desconocida de E/S");
			break;
		}
	}
	free(fd_conexion_entradasalida);
}


interfaz* _crear_instancia_interfaz(t_buffer* buffer, int* fd_conexion_entradasalida){ // CONSULTAR: Si está bien creada la instancia 
	
	interfaz* una_interfaz = malloc(sizeof(interfaz)); 
	
	una_interfaz->nombre_interfaz = extraer_string_del_buffer(buffer);
	una_interfaz->resultado_operacion_solicitada = OK;
	una_interfaz->instrucciones_disponibles = list_create();
	
	while(buffer->size > 0){
		instruccion_interfaz una_instruccion = extraer_int_del_buffer(buffer);
		list_add(una_interfaz->instrucciones_disponibles,&una_instruccion);
	} 
	
	una_interfaz->fd_conexion=fd_conexion_entradasalida;
	pthread_mutex_init(&una_interfaz->mutex_interfaz, NULL);
	sem_init(&una_interfaz->sem_interfaz,0,1);
	sem_init(&una_interfaz->sem_request_interfaz,0,0);
	sem_init(&una_interfaz->sem_instruccion_interfaz,0,0);

	return una_interfaz;
}

int solicitar_instruccion_a_interfaz(pcb* un_pcb, interfaz* una_interfaz){
	
	t_paquete* paquete = NULL;
	
	paquete = crear_paquete_con_buffer(un_pcb->pedido_a_interfaz->instruccion_a_interfaz);
	
	cargar_datos_auxiliares_en_paquete(un_pcb->pedido_a_interfaz->instruccion_a_interfaz,un_pcb,paquete);
	
	enviar_paquete(paquete,*(una_interfaz->fd_conexion));
	
	sem_wait(&una_interfaz->sem_instruccion_interfaz);
	sem_post(&una_interfaz->sem_interfaz);  

	return una_interfaz->resultado_operacion_solicitada;
}

void cargar_datos_auxiliares_en_paquete(instruccion_interfaz instruccion, pcb* un_pcb, t_paquete* un_paquete){
	
	t_list* datos_auxiliares = un_pcb->pedido_a_interfaz->datos_auxiliares_interfaz;
	switch (instruccion)
	{
		case IO_GEN_SLEEP:

			int* un_tiempo = list_remove(datos_auxiliares,0);
			
			cargar_string_a_paquete(un_paquete,un_pcb->pedido_a_interfaz->nombre_interfaz);
			cargar_int_a_paquete(un_paquete,un_pcb->pid);
			cargar_int_a_paquete(un_paquete,*un_tiempo);

			break;
		
		case IO_STDIN_READ:

			
			cargar_string_a_paquete(un_paquete,un_pcb->pedido_a_interfaz->nombre_interfaz);
			cargar_int_a_paquete(un_paquete,un_pcb->pid);
			int* parametro = list_remove(datos_auxiliares,0);
			cargar_int_a_paquete(un_paquete,*parametro);
			parametro = list_remove(datos_auxiliares,1);
			cargar_int_a_paquete(un_paquete,*parametro);
		

		case IO_STDOUT_WRITE:

			cargar_string_a_paquete(un_paquete,un_pcb->pedido_a_interfaz->nombre_interfaz);
			cargar_int_a_paquete(un_paquete,un_pcb->pid);
			parametro = list_remove(datos_auxiliares,0);
			cargar_int_a_paquete(un_paquete,*parametro);
			parametro = list_remove(datos_auxiliares,1);
			cargar_int_a_paquete(un_paquete,*parametro);

		case IO_FS_CREATE:

			char* nombre_archivo = list_remove(datos_auxiliares,0);
			cargar_string_a_paquete(un_paquete,nombre_archivo);
			cargar_int_a_paquete(un_paquete,un_pcb->pid);

		case IO_FS_DELETE:

			nombre_archivo = list_remove(datos_auxiliares,0);
			cargar_string_a_paquete(un_paquete, nombre_archivo);
			cargar_int_a_paquete(un_paquete,un_pcb->pid);

		case IO_FS_TRUNCATE:

			nombre_archivo = list_remove(datos_auxiliares,0);
			cargar_string_a_paquete(un_paquete, nombre_archivo);
			cargar_int_a_paquete(un_paquete,un_pcb->pid);
			parametro = list_remove(datos_auxiliares,1);
			cargar_int_a_paquete(un_paquete,*parametro);
			

		case IO_FS_WRITE:

			nombre_archivo = list_remove(datos_auxiliares,0);
			cargar_string_a_paquete(un_paquete, nombre_archivo);
			cargar_int_a_paquete(un_paquete,un_pcb->pid);
			parametro = list_remove(datos_auxiliares,1);
			cargar_int_a_paquete(un_paquete,*parametro);
			parametro = list_remove(datos_auxiliares,2);
			cargar_int_a_paquete(un_paquete,*parametro);
			parametro = list_remove(datos_auxiliares,3);
			cargar_int_a_paquete(un_paquete,*parametro);

		case IO_FS_READ:

			nombre_archivo = list_remove(datos_auxiliares,0);
			cargar_string_a_paquete(un_paquete, nombre_archivo);
			cargar_int_a_paquete(un_paquete,un_pcb->pid);
			parametro = list_remove(datos_auxiliares,1);
			cargar_int_a_paquete(un_paquete,*parametro);
			parametro = list_remove(datos_auxiliares,2);
			cargar_int_a_paquete(un_paquete,*parametro);
			parametro = list_remove(datos_auxiliares,3);
			cargar_int_a_paquete(un_paquete,*parametro);

			break;

		default:
			log_error(kernel_logger,"ERROR: Se recibió instrucción de E/S que no está disponible");
			exit(EXIT_FAILURE);
			break;
	}
}

void limpiar_interfaz(int *fd_interfaz){
	
	bool _coincide_fd (interfaz* una_interfaz){
		return *(una_interfaz->fd_conexion) == *fd_interfaz;
	}

	interfaz* una_interfaz = NULL;
	pthread_mutex_lock(&mutex_lista_interfaces);
	una_interfaz = list_remove_by_condition(interfaces_conectadas,(void *)_coincide_fd);
	pthread_mutex_unlock(&mutex_lista_interfaces);

	if(una_interfaz != NULL){

		pthread_mutex_lock(&una_interfaz->mutex_interfaz);
		int tamanio_lista_pcbs = list_size(una_interfaz->lista_procesos_en_cola);
		for(int i = 0;i<tamanio_lista_pcbs;i++){
			pcb* un_pcb = list_remove(una_interfaz->lista_procesos_en_cola,i);
			list_destroy(un_pcb->pedido_a_interfaz->datos_auxiliares_interfaz);
			free(un_pcb->pedido_a_interfaz);
			un_pcb->pedido_a_interfaz = NULL;
			planificar_proceso_exit_en_hilo(un_pcb);
			sleep(1);
		}

		list_destroy(una_interfaz->instrucciones_disponibles);
		list_destroy(una_interfaz->lista_procesos_en_cola);
		free(una_interfaz->fd_conexion);
		pthread_mutex_unlock(&una_interfaz->mutex_interfaz);

		free(una_interfaz);
	}
}