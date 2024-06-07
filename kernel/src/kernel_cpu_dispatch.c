#include "../include/kernel_cpu_dispatch.h"


void esperar_conexiones_cpu_dispatch(){
	
	fd_cpu_dispatch = crear_conexion(IP_CPU, PUERTO_CPU_DISPATCH);
    log_info(kernel_logger, "Conexion con CPU DISPATCH exitosa.");

	pthread_t hilo_cpu_dispatch;
	int err = pthread_create(&hilo_cpu_dispatch, NULL,(void*)esperar_cpu_dispatch_kernel, NULL);
	if (err!=0){
		perror("Fallo de creación de hilo_cpu_dispatch(kernel)\n");
		exit(-3);
	}
	pthread_detach(hilo_cpu_dispatch);
}



void esperar_cpu_dispatch_kernel(){
    int estado_while = 1;

    while (estado_while) {
		log_trace(kernel_logger,"KERNEL: ESPERANDO MENSAJES DE CPU DISPATCH...");
        // COD_OP + TAM + STREAM
		
		int cod_op = recibir_operacion(fd_cpu_dispatch);
		switch (cod_op) {
		// TAM + STREAM
		
		case ATENDER_INSTRUCCION_CPU:
		 	
			t_buffer* un_buffer = recibir_buffer(fd_cpu_dispatch);
			
			instruccion_interfaz instruccion_solicitada = extraer_int_del_buffer(un_buffer);
			char* interfaz_solicitada = extraer_string_del_buffer(un_buffer);
			// Podría ser una lista?
			void* recurso_necesario_de_instruccion = extraer_mensaje_de_buffer(un_buffer);
			int tamanio_recurso_necesario = extraer_int_del_buffer(un_buffer);

			pcb* pcb_recibido = NULL;
			pcb_recibido = obtener_contexto_pcb(un_buffer);

			pcb* un_pcb = extraer_pcb_de_lista_sistema(pcb_recibido);

			cambiar_estado_pcb(un_pcb, BLOCKED);

			actualizar_pcb(pcb_recibido,un_pcb);

			list_add_sync(blocked,un_pcb,&mutex_lista_blocked);
			
			un_pcb -> motivo_bloqueo = PEDIDO_A_INTERFAZ;

			un_pcb -> pedido_a_interfaz -> nombre_interfaz = interfaz_solicitada;
			un_pcb -> pedido_a_interfaz -> instruccion_a_interfaz = instruccion_solicitada;
			un_pcb -> pedido_a_interfaz -> recurso_necesario = recurso_necesario_de_instruccion;
			un_pcb -> pedido_a_interfaz -> tamanio_recurso = tamanio_recurso_necesario;

			manejar_bloqueo_de_proceso(un_pcb);	
			sem_post(&sem_cpu_libre);	

			destruir_buffer(un_buffer);

		break;

		case WAIT_KCPU:

			un_buffer = recibir_buffer(fd_cpu_dispatch);
			recurso recurso_solicitado = extraer_int_del_buffer(un_buffer);

			pcb_recibido = NULL;
			pcb_recibido = obtener_contexto_pcb(un_buffer);

			un_pcb = extraer_pcb_de_lista_sistema(pcb_recibido);

			actualizar_pcb(pcb_recibido,un_pcb);
			cambiar_estado_pcb(un_pcb, BLOCKED);
			list_add_sync(blocked,un_pcb,&mutex_lista_blocked);

			un_pcb -> motivo_bloqueo = WAIT;
			un_pcb -> pedido_recurso = recurso_solicitado;
			agregar_recurso(un_pcb,recurso_solicitado);

			manejar_bloqueo_de_proceso(un_pcb);	
			sem_post(&sem_cpu_libre);	

			destruir_buffer(un_buffer);

		break;

		case SIGNAL_KCPU:

			un_buffer = recibir_buffer(fd_cpu_dispatch);
			recurso_solicitado = extraer_int_del_buffer(un_buffer);
			int pid_recibido = extraer_int_del_buffer(un_buffer);

			//CONSULTA: Puedo inicializarlo en NULL cuando tengo variables enum dentro?
			un_pcb = NULL;
			un_pcb->pid = pid_recibido;
			
			un_pcb = buscar_pcb_en_sistema_(un_pcb);
			un_pcb -> motivo_bloqueo = SIGNAL;
			un_pcb -> pedido_recurso = recurso_solicitado;

			quitar_recurso(un_pcb,recurso_solicitado);
			manejar_bloqueo_de_proceso(un_pcb);

			destruir_buffer(un_buffer);

		break;

		case -1:
			log_error(kernel_logger, "CPU DISPATCH se desconecto. Terminando servidor");
			estado_while = 0;
            break;
		
		default:
			log_warning(kernel_logger,"Operacion desconocida de CPU DISPATCH");
			break;
		}
	}
}

void enviar_pcb_CPU_dispatch(pcb* un_pcb){

	t_paquete* un_paquete = NULL;
	un_paquete = crear_paquete_con_buffer(EJECUTAR_PROCESO_KCPU);
	cargar_int_a_paquete(un_paquete, un_pcb->pid);
	cargar_int_a_paquete(un_paquete, un_pcb->program_counter);
	cargar_int_a_paquete(un_paquete, un_pcb->tiempo_ejecutado);
	cargar_int_a_paquete(un_paquete, un_pcb->ticket);
	cargar_uint32_a_paquete(un_paquete, un_pcb->registros_CPU->AX);
	cargar_uint32_a_paquete(un_paquete, un_pcb->registros_CPU->BX);
	cargar_uint32_a_paquete(un_paquete, un_pcb->registros_CPU->CX);
	cargar_uint32_a_paquete(un_paquete, un_pcb->registros_CPU->DX);

	enviar_paquete(un_paquete, fd_cpu_dispatch); //RECORDAR: PAQUETE SE SERIALIZA ACÁ ADENTRO
	destruir_paquete(un_paquete);
}

// CONSULTAR: Si están bien las siguientes funciones con listas
void agregar_recurso (pcb* un_pcb, recurso un_recurso){

	bool _buscar_recurso(recursos_pcb* recurso_encontrado)
	{
		return recurso_encontrado->nombre_recurso == un_recurso;
	}

	recursos_pcb* recurso = NULL;

	if(list_is_empty(un_pcb->recursos_en_uso)){
		
		recurso->nombre_recurso = un_recurso;
		recurso->instancias_en_uso = 1;
		list_add(un_pcb->recursos_en_uso,recurso);

	}
	else{

		if(list_any_satisfy(un_pcb->recursos_en_uso, (void *)_buscar_recurso))
		{
			recurso = list_find(un_pcb->recursos_en_uso, (void *)_buscar_recurso);
			recurso->instancias_en_uso += 1;
		}
		else
		{
			recurso->nombre_recurso = un_recurso;
			recurso->instancias_en_uso = 1;
			list_add(un_pcb->recursos_en_uso,recurso);
		}
	}
}

void quitar_recurso (pcb* un_pcb, recurso un_recurso){

	bool _buscar_recurso(recursos_pcb* recurso_encontrado)
	{
		return recurso_encontrado->nombre_recurso == un_recurso;
	}

	recursos_pcb* recurso = NULL;

	if(list_any_satisfy(un_pcb->recursos_en_uso, (void *)_buscar_recurso))
	{
		recurso = list_find(un_pcb->recursos_en_uso, (void *)_buscar_recurso);
		
		if(recurso->instancias_en_uso > 0){

			recurso->instancias_en_uso -= 1;

		}
		else
		{
			list_remove_element(un_pcb->recursos_en_uso,recurso);
		}
	}
	
}