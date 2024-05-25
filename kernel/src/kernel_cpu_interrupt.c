#include "../include/kernel_cpu_interrupt.h"

void esperar_conexiones_cpu_interrupt(){
	
	fd_cpu_interrupt = crear_conexion(IP_CPU, PUERTO_CPU_INTERRUPT);
    log_info(kernel_logger, "Conexion con CPU INTERRUPT exitosa.");

	pthread_t hilo_cpu_interrupt;
	int err = pthread_create(&hilo_cpu_interrupt, NULL,(void*)esperar_cpu_interrupt_kernel, NULL);
	if (err!=0){
		perror("Fallo de creación de hilo_cpu_interrupt(kernel)\n");
		exit(-3);
	}
	pthread_detach(hilo_cpu_interrupt);
}

void esperar_cpu_interrupt_kernel(){
    int estado_while = 1;
    while (estado_while) {
		log_trace(kernel_logger,"KERNEL: ESPERANDO MENSAJES DE CPU INTERRUPT...");
        int cod_op = recibir_operacion(fd_cpu_interrupt);
		switch (cod_op) {
		case MENSAJE:
		 	recibir_mensaje_tp0(fd_cpu_interrupt,kernel_logger);
			break;
		case PAQUETE:
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

// CASOS DE INTERRUPCIÓN:
//		- Por consola
//		- Por fin de quantum
pcb* _gestionar_interrupcion(){


		pthread_mutex_lock(&mutex_lista_exec);
		pcb* un_pcb = list_get(execute, 0);
		pthread_mutex_unlock(&mutex_lista_exec);

		
		t_paquete* paquete = NULL;
		paquete = crear_paquete_con_buffer(DESALOJAR_PROCESO_KCPU);
		enviar_paquete(paquete,fd_cpu_interrupt);
    	destruir_paquete(paquete);

		return un_pcb;

}