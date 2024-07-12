#include "../include/kernel.h"

// SERVIDOR DE: ENTRADASALIDA
// CLIENTE DE: MEMORIA, CPU 

void mandar_mensajes(){
    sleep(10);
    enviar_mensaje("Hola CPU dispatch, soy KERNEL",fd_cpu_dispatch);
    enviar_mensaje("Hola CPU itnerrupt, soy KERNEL",fd_cpu_interrupt);
    enviar_mensaje("Hola memoria, soy KERNEL",fd_memoria);
}

int main(int argc, char* argv[]){
   
    // Iicializa kernel:
    // - Logs
    // - Configs
    // - Listas
    // - Semáforos
    // - Mutexes
    inicializar_kernel();

    // Se crean las conexiones
    esperar_conexiones_cpu_dispatch();
    esperar_conexiones_cpu_interrupt();
    esperar_conexiones_memoria();
    ejecutar_en_hilo_detach((void*)esperar_conexiones_entradasalida,NULL); //Lo ejecuto en hilo detach por la espera de nuevo cliente, traba la ejecución del programa
    
    // Se inicializa la consola
    iniciar_consola();

    return EXIT_SUCCESS;
}