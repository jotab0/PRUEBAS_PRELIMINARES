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
    log_info(kernel_logger, "Conexion con memoria exitosa.");

    //Atender los mensajes de CPU - Dispatch
    pthread_t hilo_cpu_dispatch;
    pthread_create(&hilo_cpu_dispatch, NULL, (void*)atender_kernel_cpu_dispatch, NULL);
    pthread_detach(hilo_cpu_dispatch);

    //Atender los mensajes de CPU - Interrupt

    pthread_t hilo_cpu_interrupt;
    pthread_create(&hilo_cpu_interrupt, NULL, (void*)atender_kernel_cpu_interrupt, NULL);
    pthread_detach(hilo_cpu_interrupt);

    //Atender los mensajes de EntradaSalida

    pthread_t hilo_entradasalida;
    pthread_create(&hilo_entradasalida, NULL, (void*)atender_kernel_entradasalida, NULL);
    pthread_detach(hilo_entradasalida);
     
    //Atender los mensajes de Memoria

    pthread_t hilo_memoria;
    pthread_create(&hilo_memoria, NULL, (void*)atender_kernel_memoria, NULL);
    pthread_join(hilo_memoria, NULL);

    //log_debug(kernel_logger_extra, "Advertencia de salida");

    //Iniciar la consola interactiva
    
    return EXIT_SUCCESS;
}