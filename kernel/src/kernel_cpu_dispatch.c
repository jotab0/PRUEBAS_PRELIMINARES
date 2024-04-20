#include "../include/kernel_cpu_dispatch.h"

static void iterator(char* value){
	log_info(kernel_logger,"%s",value);
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