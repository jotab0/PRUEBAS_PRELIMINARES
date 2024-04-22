#include "../include/kernel_cpu_interrupt.h"
static void iterator(char* value){
	log_info(kernel_logger,"%s",value);
}
void esperar_cpu_interrupt_kernel(){
    int estado_while = 1;
	t_list* lista;
    while (estado_while) {
		log_trace(kernel_logger,"KERNEL: ESPERANDO MENSAJES DE CPU INTERRUPT...");
        int cod_op = recibir_operacion(fd_cpu_interrupt);
		switch (cod_op) {
		case MENSAJE:
		 	recibir_mensaje_tp0(fd_cpu_interrupt,kernel_logger);
			break;
		case PAQUETE:
			lista = recibir_paquete(fd_cpu_interrupt);
			log_info(kernel_logger,"Me llegaron los siguientes mensajes:\n");
			list_iterate(lista,(void*)iterator);
			break;
		case -1:
			log_error(kernel_logger, "CPU INTERRUPT se desconecto. Terminando servidor");
			estado_while = 0;
            break;
		default:
			log_warning(kernel_logger,"Operacion desconocida de CPU INTERRUPT");
			break;
		}
	}
}