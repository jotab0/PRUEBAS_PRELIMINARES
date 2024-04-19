#include "../include/cpu.h"

// SERVIDOR DE: KERNEL (x2)
// CLIENTE DE:  MEMORIA

// MENSAJES DE PRUEBA
void mandar_mensaje_a_k_dispatch(){
    enviar_mensaje("Hola K-Dispatch", fd_kernel_dispatch);
}

void mandar_mensaje_a_k_interrupt(){
    enviar_mensaje("Hola K-Interrupt", fd_kernel_interrupt);
}

void mandar_mesaje_a_memoria(){
    enviar_mensaje("Hola memoria",fd_memoria);
}

int main(void){
    
    // INICIAR CPU
    inicializar_cpu();

    // CONEXIONES CPU

    // iniciar server cpu-dispatch
    fd_cpu_dispatch = iniciar_servidor(PUERTO_ESCUCHA_DISPATCH, cpu_logger, "!! Servidor CPU-DISPATCH iniciado !!");
    //iniciar server cpu-interrupt
    fd_cpu_interrupt = iniciar_servidor(PUERTO_ESCUCHA_INTERRUPT, cpu_logger, "!! Servidor CPU-DISPATCH iniciado !!");
   
    //conectarse como cliente con memoria
    fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
    log_info(cpu_logger, "Conexion con MEMORIA exitosa");


    //esperar al cliente kernel-dispatch
    fd_kernel_dispatch = esperar_cliente(fd_cpu_dispatch, cpu_logger, "KERNEL - Dispatch");
    //esperar al cliente kernel-interrupt
    fd_kernel_interrupt = esperar_cliente(fd_cpu_interrupt, cpu_logger, "KERNEL - Interrupt");

    sleep(15);
    printf("Ya esperé");
    // COMUNICACIÓN

    pthread_t hilo_mensaje_a_k_dispatch;
    int err = pthread_create(&hilo_mensaje_a_k_dispatch,NULL,(void*)mandar_mensaje_a_k_dispatch,NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_mensaje_a_k_dispatch(cpu)\n");
        return -3;
    }
    pthread_detach(hilo_mensaje_a_k_dispatch);

    //atender los mensajes de kernel-dispatch
    pthread_t hilo_k_dispatch;
    err = pthread_create(&hilo_k_dispatch,NULL,(void*)esperar_kernel_cpu_dispatch,NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_k_dispatch(cpu)\n");
        return -3;
    }
    pthread_detach(hilo_k_dispatch);

    pthread_t hilo_mensaje_a_k_interrupt;
    err = pthread_create(&hilo_mensaje_a_k_interrupt,NULL,(void*)mandar_mensaje_a_k_interrupt,NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_mensaje_a_k_interrupt(cpu)\n");
        return -3;
    }
    pthread_detach(hilo_mensaje_a_k_interrupt);

    // atender los mensajes de kernel-interrupt
                        //1//             //3//                 //4//
                        //HILO//          //Funcion deseada//   //Puntero si hace falta//
    pthread_t hilo_k_interrupt;
    err = pthread_create(&hilo_k_interrupt,NULL,(void*)esperar_kernel_cpu_interrupt,NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_k_interrupt(cpu)\n");
        return -3;
    }
    pthread_detach(hilo_k_interrupt);

    sleep(10);
    pthread_t hilo_mensaje_a_memoria;
    err = pthread_create(&hilo_mensaje_a_memoria,NULL,(void*)mandar_mesaje_a_memoria,NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_k_interrupt(cpu)\n");
        return -3;
    }
    pthread_detach(hilo_mensaje_a_memoria);
   
   // atender los mensajes de memoria
    pthread_t hilo_memoria;
    err = pthread_create(&hilo_memoria,NULL,(void*)esperar_memoria_cpu,NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_memoria(cpu)\n");
        return -3;
    }
    pthread_join(hilo_memoria,NULL);


    
	return EXIT_SUCCESS;
}

