#include "../include/kernel_cpu_dispatch.h"

static void iterator(char* value){
	log_info(kernel_logger,"%s",value);
}

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
	t_list* lista;

    while (estado_while) {
		log_trace(kernel_logger,"KERNEL: ESPERANDO MENSAJES DE CPU DISPATCH...");
        int cod_op = recibir_operacion(fd_cpu_dispatch);
		switch (cod_op) {
		case MENSAJE:
		 	recibir_mensaje_tp0(fd_cpu_dispatch,kernel_logger);
			break;
		case PAQUETE:
			lista = recibir_paquete(fd_cpu_dispatch);
			log_info(kernel_logger,"Me llegaron los siguientes mensajes:\n");
			list_iterate(lista,(void*)iterator);
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

	t_paquete* un_paquete = crear_paquete_con_buffer(EJECUTAR_PROCESO_KCPU);
	cargar_int_a_paquete(un_paquete, un_pcb->pid);
	cargar_int_a_paquete(un_paquete, un_pcb->program_counter);
	cargar_uint32_a_paquete(un_paquete, &(un_pcb->registros_CPU->AX));
	cargar_uint32_a_paquete(un_paquete, &(un_pcb->registros_CPU->BX));
	cargar_uint32_a_paquete(un_paquete, &(un_pcb->registros_CPU->CX));
	cargar_uint32_a_paquete(un_paquete, &(un_pcb->registros_CPU->DX));

	enviar_paquete(un_paquete, fd_cpu_dispatch); //RECORDAR: PAQUETE SE SERIALIZA ACÁ ADENTRO
	eliminar_paquete(un_paquete);
}