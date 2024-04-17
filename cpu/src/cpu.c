#include "../include/cpu.h"

// SERVIDOR DE: KERNEL (x2)
// CLIENTE DE:  MEMORIA

int main(void){
    
    // INICIAR CPU
    inicializar_cpu();

    // CONEXIONES CPU
    fd_cpu_dispatch = iniciar_servidor(PUERTO_ESCUCHA_DISPATCH, cpu_logger, "!! Servidor CPU-DISPATCH iniciado !!");
    fd_cpu_interrupt = iniciar_servidor(PUERTO_ESCUCHA_INTERRUPT, cpu_logger, "!! Servidor CPU-DISPATCH iniciado !!");
   

    fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
    log_info(cpu_logger, "Conexion con MEMORIA exitosa");


    fd_kernel_dispatch = esperar_cliente(fd_cpu_dispatch, cpu_logger, "KERNEL - Dispatch");
    fd_kernel_interrupt = esperar_cliente(fd_cpu_interrupt, cpu_logger, "KERNEL - Interrupt");

    // COMUNICACIÓN
    pthread_t hilo_k_dispatch;
    pthread_create(&hilo_k_dispatch,NULL,(void*)esperar_kernel_dispatch,NULL);
    pthread_detach(esperar_kernel_dispatch);
                        //1//             //3//                 //4//
                        //HILO//          //Funcion deseada//   //Puntero si hace falta//
    pthread_t hilo_k_interrupt;
    pthread_create(&hilo_k_interrupt,NULL,(void*)esperar_kernel_interrupt,NULL);
    pthread_detach(esperar_kernel_interrupt);

    pthread_t hilo_memoria;
    pthread_create(&hilo_memoria,NULL,(void*)esperar_memoria,NULL);
    pthread_join(esperar_memoria,NULL);


	return EXIT_SUCCESS;
}

