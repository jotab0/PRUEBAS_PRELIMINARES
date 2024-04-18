#include "../include/kernel_memoria.h"

void atender_kernel_memoria(){
    bool control_key = 1;
    while(control_key){
        log_trace(kernel_logger,"ESPERANDO MENSAJES DE MEMORIA");
        int cod_op = recibir_operacion(fd_memoria);
        switch (cod_op){
            case MENSAJE:
                //
                break;
            case PAQUETE:
                //
                break;
            case -1:
                log_error(kernel_logger, "Desconexión de MEMORIA");
                control_key = 0;
                //exit(EXIT_FAILURE);
                break;
            default:
            log_warning(kernel_logger, "Operacion desconocida de MEMORIA");
            break;
        }
    }
}