#include "../include/kernel_entradasalida.h"
static void iterator(char* value){
	log_info(kernel_logger,"%s",value);
}


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
	t_list* lista;
    while (estado_while) {
		log_trace(kernel_logger,"KERNEL: ESPERANDO MENSAJES DE E/S...");
        int cod_op = recibir_operacion(*fd_conexion_entradasalida);
		switch (cod_op) {
		case MENSAJE:
		 	recibir_mensaje_tp0(*fd_conexion_entradasalida,kernel_logger);
			break;
		case PAQUETE:
			lista = recibir_paquete(*fd_conexion_entradasalida);
			log_info(kernel_logger,"Me llegaron los siguientes mensajes:\n");
			list_iterate(lista,(void*)iterator);
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