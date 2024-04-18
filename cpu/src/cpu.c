#include "../include/cpu.h"

// SERVIDOR DE: KERNEL (x2)
// CLIENTE DE:  MEMORIA

int main(void){
    
    // INICIAR CPU
    inicializar_cpu();


    fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
    log_info(cpu_logger, "Conexion con MEMORIA exitosa");

    // CONEXIONES CPU
    fd_cpu_dispatch = iniciar_servidor(PUERTO_ESCUCHA_DISPATCH, cpu_logger, "!! Servidor CPU-DISPATCH iniciado !!");
    fd_cpu_interrupt = iniciar_servidor(PUERTO_ESCUCHA_INTERRUPT, cpu_logger, "!! Servidor CPU-DISPATCH iniciado !!");
   
    fd_kernel_dispatch = esperar_cliente(fd_cpu_dispatch, cpu_logger, "KERNEL - Dispatch");
    fd_kernel_interrupt = esperar_cliente(fd_cpu_interrupt, cpu_logger, "KERNEL - Interrupt");

    // COMUNICACIÓN
    pthread_t hilo_memoria;
    int err = pthread_create(&hilo_memoria,NULL,(void*)esperar_memoria_cpu,NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_memoria(cpu)\n");
        return -3;
    }
    pthread_join(hilo_memoria,NULL);

    /*
    pthread_t hilo_k_dispatch;
    err = pthread_create(&hilo_k_dispatch,NULL,(void*)esperar_kernel_dispatch_cpu,NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_k_dispatch(cpu)\n");
        return -3;
    }
    pthread_detach(hilo_k_dispatch);
                        //1//             //3//                 //4//
                        //HILO//          //Funcion deseada//   //Puntero si hace falta//

                        
    pthread_t hilo_k_interrupt;
    err = pthread_create(&hilo_k_interrupt,NULL,(void*)esperar_kernel_interrupt_cpu,NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_k_interrupt(cpu)\n");
        return -3;
    }
    pthread_join(hilo_k_interrupt,NULL);
*/    

	return EXIT_SUCCESS;
}

