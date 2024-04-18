#include "../include/kernel.h"

// SERVIDOR DE: ENTRADASALIDA
// CLIENTE DE: MEMORIA, CPU 

int main(int argc, char* argv[]) {
   
    //Inicializa Kernel
    inicializar_kernel();


    //Conecta con CPU
    fd_cpu_dispatch = crear_conexion(IP_CPU, PUERTO_CPU_DISPATCH);
    log_info(kernel_logger, "Conexion con CPU DISPATCH exitosa.");
    fd_cpu_interrupt = crear_conexion(IP_CPU, PUERTO_CPU_INTERRUPT);
    log_info(kernel_logger, "Conexion con CPU INTERRUPT exitosa.");

    //Conecta con Memoria
    fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
    log_info(kernel_logger, "Conexion con MEMORIA exitosa.");

    fd_kernel = iniciar_servidor(PUERTO_ESCUCHA, kernel_logger, "!! Servidor ENTRADA-SALIDA iniciado !!");
    fd_entradasalida = esperar_cliente(fd_kernel, kernel_logger, "ENTRADA-SALIDA");

/*
    //Atender los mensajes de CPU - Dispatch
    pthread_t hilo_cpu_dispatch;
    int err = pthread_create(&hilo_cpu_dispatch, NULL, (void*)atender_kernel_cpu_dispatch, NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_cpu_dispatch(kernel)\n");
        return -3;
    }
    pthread_detach(hilo_cpu_dispatch);

    //Atender los mensajes de CPU - Interrupt

    pthread_t hilo_cpu_interrupt;
    err = pthread_create(&hilo_cpu_interrupt, NULL, (void*)atender_kernel_cpu_interrupt, NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_cpu_interrupt(kernel)\n");
        return -3;
    }
    pthread_detach(hilo_cpu_interrupt);

    //Atender los mensajes de EntradaSalida

    pthread_t hilo_entradasalida;
    err = pthread_create(&hilo_entradasalida, NULL, (void*)atender_kernel_entradasalida, NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_entradasalida(kernel)\n");
        return -3;
    }
    pthread_detach(hilo_entradasalida);
     
    //Atender los mensajes de Memoria
*/
    pthread_t hilo_memoria;
    int err = pthread_create(&hilo_memoria, NULL, (void*)atender_kernel_memoria, NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_memoria(kernel)\n");
        return -3;
    }
    pthread_join(hilo_memoria,NULL);

    //log_debug(kernel_logger_extra, "Advertencia de salida");

    //Iniciar la consola interactiva
    
    return EXIT_SUCCESS;
}