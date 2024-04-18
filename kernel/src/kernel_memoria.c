#include "../include/kernel_memoria.h"

void esperar_memoria_kernel(){
    bool control_key = 1;
    while(control_key){
        log_trace(kernel_logger,"KERNEL: ESPERANDO MENSAJES DE MEMORIA...");
        int cod_op = recibir_operacion(fd_memoria);
        switch (cod_op){
            case MENSAJE:
                //
                break;
            case PAQUETE:
                //
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