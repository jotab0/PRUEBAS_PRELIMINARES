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
    while (estado_while) {
		log_trace(cpu_logger,"CPU: ESPERANDO MENSAJES DE MEMORIA");
        int cod_op = recibir_operacion(fd_memoria);
		switch (cod_op) {
		case SOLICITUD_INSTRUCCION:
			t_buffer* unBuffer = recibir_buffer(fd_memoria);
			recibir_instruccion(unBuffer);
			destruir_buffer(unBuffer);
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

// atender memoria

void recibir_instruccion(t_buffer* unBuffer){
	char* instruccion = extraer_string_del_buffer(unBuffer);
	log_warning(cpu_logger, "Instruccion recibida: [%s]", instruccion);
	// string split divide una cadena en subcadenas (para dividir el cod_op de los operadores?)
	instruccion_dividida = string_split(instruccion, " ");
	free(instruccion);
}
