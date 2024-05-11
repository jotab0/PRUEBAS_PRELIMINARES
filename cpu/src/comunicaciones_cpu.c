#include "../include/comunicaciones_cpu.h"

void iterator(char* value){
	log_info(cpu_logger,"%s",value);
}

void esperar_kernel_cpu_interrupt(){
    int estado_while = 1;
	t_list* lista;
    while (estado_while) {
		log_trace(cpu_logger,"CPU INTERRUPT: ESPERANDO MENSAJES DE KERNEL...");
        int cod_op = recibir_operacion(fd_kernel_interrupt);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje_tp0(fd_kernel_interrupt, cpu_logger);
			break;
		case PAQUETE:
			lista = recibir_paquete(fd_kernel_interrupt);
			log_info(cpu_logger,"Me llegaron los siguientes mensajes:\n");
			list_iterate(lista,(void*)iterator);
			break;
		case -1:
			log_error(cpu_logger, "KERNEL se desconecto de cpu interrupt. Terminando servidor");
			estado_while = 0;
            break;
		default:
			log_warning(cpu_logger,"Operacion desconocida de KERNEL (Interrupt)");
			break;
		}
	}
}

void esperar_kernel_cpu_dispatch(){
    int estado_while = 1;
	t_list* lista;
    while (estado_while) {
		t_list* lista;
		log_trace(cpu_logger,"CPU DISPATCH: ESPERANDO MENSAJES DE KERNEL...");
        int cod_op = recibir_operacion(fd_kernel_dispatch);
		switch (cod_op) {
		case MENSAJE:
		 	recibir_mensaje_tp0(fd_kernel_dispatch,cpu_logger);
			break;
		case PAQUETE:
			lista = recibir_paquete(fd_kernel_dispatch);
			log_info(cpu_logger,"Me llegaron los siguientes mensajes:\n");
			list_iterate(lista,(void*)iterator);
			break;
		case -1:
			log_error(cpu_logger, "KERNEL se desconecto de cpu dispatch. Terminando servidor");
			estado_while = 0;
            break;
		default:
			log_warning(cpu_logger,"Operacion desconocida de KERNEL (Distaptch)");
			break;
		}
	}
}

void esperar_memoria_cpu(){
    int estado_while = 1;
	t_list* lista;
    while (estado_while) {
		log_trace(cpu_logger,"CPU: ESPERANDO MENSAJES DE MEMORIA");
		t_list* lista;
        int cod_op = recibir_operacion(fd_memoria);
		switch (cod_op) {
		case MENSAJE:
		 	recibir_mensaje_tp0(fd_memoria,cpu_logger);
			break;
		case PAQUETE:
			lista = recibir_paquete(fd_memoria);
			log_info(cpu_logger,"Me llegaron los siguientes mensajes:\n");
			list_iterate(lista,(void*)iterator);
			break;
		case -1:
			log_error(cpu_logger, "MEMORIA se desconecto. Terminando servidor");
			estado_while= 0;
            break;
		default:
			log_warning(cpu_logger,"Operacion desconocida de MEMORIA");
			break;
		}
	}
}

void recibir_pcb_del_kernel(t_buffer* unBuffer){
	pthread_mutex_lock(&mutex_manejo_contexto);
	iniciar_estructuras_para_recibir_pcb(unBuffer);
	pthread_mutex_unlock(&mutex_manejo_contexto);

	mostrar_pcb();

	log_info(cpu_logger, "PID del proceso antes del WHILE, %d", contexto->proceso_pid);

	while(1){
		
	}
}


void mostrar_pcb(){
	log_warning(cpu_logger, "[PID: %d] [PC: %d] [TIEMPO EJECUTADO: %u] [REGISTROS: %u|%u|%u|%u]",
	contexto->proceso_pid,
	contexto->proceso_pc,
	contexto->proceso_tiempo_ejecutado,
	contexto->AX,
	contexto->BX,
	contexto->CX,
	contexto->DX);
}

void iniciar_estructuras_para_recibir_pcb(t_buffer* unBuffer){
	contexto = malloc(sizeof(t_contexto));

	contexto->proceso_pid = extraer_int_del_buffer(unBuffer);
	contexto->proceso_pc=extraer_int_del_buffer(unBuffer);
	contexto->proceso_tiempo_ejecutado=extraer_int_del_buffer(unBuffer);
	contexto->proceso_ticket=extraer_int_del_buffer(unBuffer);
	
	u_int32_t* registro_cpu = (u_int32_t*)extraer_uint32_del_buffer(unBuffer);
	contexto->AX = *registro_cpu;
	
	registro_cpu = (u_int32_t*)extraer_uint32_del_buffer(unBuffer);
	contexto->BX = *registro_cpu;

	registro_cpu = (u_int32_t*)extraer_uint32_del_buffer(unBuffer);
	contexto->CX = *registro_cpu;

	registro_cpu = (u_int32_t*)extraer_uint32_del_buffer(unBuffer);
	contexto->DX = *registro_cpu;

	free(unBuffer);
}