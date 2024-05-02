#include "../include/kernel_memoria.h"
static void iterator(char* value){
	log_info(kernel_logger,"%s",value);
}

void esperar_conexiones_memoria(){
	
	fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
    log_info(kernel_logger, "Conexion con MEMORIA exitosa.");

	pthread_t hilo_memoria;
    int err = pthread_create(&hilo_memoria, NULL, (void*)esperar_memoria_kernel, NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_memoria(kernel)\n");
    }
    pthread_detach(hilo_memoria);
}


void esperar_memoria_kernel(){
    bool control_key = 1;
    t_list* lista;
    while(control_key){
        log_trace(kernel_logger,"KERNEL: ESPERANDO MENSAJES DE MEMORIA...");
        int cod_op = recibir_operacion(fd_memoria);
        switch (cod_op){
            case MENSAJE:
                recibir_mensaje_tp0(fd_memoria,kernel_logger);
            break;
            case PAQUETE:
                lista = recibir_paquete(fd_memoria);
                log_info(kernel_logger,"Me llegaron los siguientes mensajes:\n");
                list_iterate(lista,(void*)iterator);
			break;
            case -1:
                log_error(kernel_logger, "MEMORIA se desconecto. Terminando servidor");
                control_key = 0;
                //exit(EXIT_FAILURE);
                break;
            default:
            log_warning(kernel_logger, "Operacion desconocida de MEMORIA");
            break;
        }
    }
}