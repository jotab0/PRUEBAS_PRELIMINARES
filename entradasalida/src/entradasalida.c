#include "../include/entradasalida.h"

// SERVIDOR DE: -
// CLIENTE DE: KERNEL, MEMORIA 


int main(int argc, char* argv[]) {

    // Inicializar estructuras de ES
    inicializar_es();

    //Me conecto como Cliente a MEMORIA
    fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
    log_info(es_logger, "Conexion con Memoria exitosa.");
    
    //Me conecto como Cliente a KERNEL
    fd_kernel = crear_conexion(IP_KERNEL, PUERTO_KERNEL);
    log_info(es_logger, "Conexion con Kernel exitosa.");

     
    pthread_t hilo_cpu;
    pthread_create(&hilo_cpu,NULL,(void*)esperar_cpu,NULL);
    pthread_detach(esperar_cpu);
        
    pthread_t hilo_kernel;
    pthread_create(&hilo_kernel,NULL,(void*)esperar_kernel,NULL);
    pthread_join(esperar_kernel,NULL);
    
    return 0; 
}
