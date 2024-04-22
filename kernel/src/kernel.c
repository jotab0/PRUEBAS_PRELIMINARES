#include "../include/kernel.h"

// SERVIDOR DE: ENTRADASALIDA
// CLIENTE DE: MEMORIA, CPU 

void mandar_mensajes(){
    sleep(10);
    enviar_mensaje("Hola CPU dispatch, soy KERNEL",fd_cpu_dispatch);
    enviar_mensaje("Hola CPU itnerrupt, soy KERNEL",fd_cpu_interrupt);
    enviar_mensaje("Hola memoria, soy KERNEL",fd_memoria);
    enviar_mensaje("Hola E/S, soy KERNEL",fd_entradasalida);
}

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


    //Atender los mensajes de CPU - Dispatch
    pthread_t hilo_cpu_dispatch;
    int err = pthread_create(&hilo_cpu_dispatch, NULL, (void*)esperar_cpu_interrupt_kernel, NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_cpu_dispatch(kernel)\n");
        return -3;
    }
    pthread_detach(hilo_cpu_dispatch);

    //Atender los mensajes de CPU - Interrupt

    pthread_t hilo_cpu_interrupt;
    err = pthread_create(&hilo_cpu_interrupt, NULL, (void*)esperar_cpu_dispatch_kernel, NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_cpu_interrupt(kernel)\n");
        return -3;
    }
    pthread_detach(hilo_cpu_interrupt);


    pthread_t hilo_mensajes;
    err = pthread_create(&hilo_mensajes,NULL,(void*)mandar_mensajes,NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_mensaje_a_cpu(memoria)\n");
        return -3;
    }
    pthread_detach(hilo_mensajes);

    //Atender los mensajes de EntradaSalida

    pthread_t hilo_entradasalida;
    err = pthread_create(&hilo_entradasalida, NULL, (void*)esperar_entradasalida_kernel, NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_entradasalida(kernel)\n");
        return -3;
    }
    pthread_detach(hilo_entradasalida);
    
    //Atender los mensajes de Memoria
    pthread_t hilo_memoria;
    err = pthread_create(&hilo_memoria, NULL, (void*)esperar_memoria_kernel, NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_memoria(kernel)\n");
        return -3;
    }
    pthread_detach(hilo_memoria);

    //Iniciar la consola interactiva
    iniciar_consola_interactiva();

    log_debug(kernel_log_debug, "Advertencia de salida de Kernel");

    //COMUNICACIÓN

    // INICIAR CONSOLA INTERACTIVA (Tiene que ser antes del hilo con el join (Quizá puede ir primera))

    return EXIT_SUCCESS;
}