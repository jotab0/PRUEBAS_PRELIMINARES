#include "../include/entradasalida.h"

// SERVIDOR DE: -
// CLIENTE DE: KERNEL, MEMORIA 

void mandar_mesaje_a_memoria(){
    enviar_mensaje("Hola memoria: Entrada Salida",fd_memoria);
} 

int main(int argc, char* argv[]) {

    // Inicializar estructuras de ES
    inicializar_es();

    //Me conecto como Cliente a MEMORIA
    fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
    log_info(es_logger, "Conexion con Memoria exitosa.");
    
    //Me conecto como Cliente a KERNEL
    fd_kernel = crear_conexion(IP_KERNEL, PUERTO_KERNEL);
    log_info(es_logger, "Conexion con Kernel exitosa.");
    
    //Hilo: mensaje a memoria
    
    pthread_t hilo_mensaje_a_memoria;
    int err = pthread_create(&hilo_mensaje_a_memoria,NULL,(void*)mandar_mesaje_a_memoria,NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_k_interrupt(cpu)\n");
        return -3;
    }
    pthread_detach(hilo_mensaje_a_memoria);


    pthread_t hilo_memoria;
    err = pthread_create(&hilo_memoria,NULL,(void*)esperar_memoria_es,NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_memoria(entradasalida))\n");
        return -3;
    }
    pthread_detach(hilo_memoria);
  
    pthread_t hilo_kernel;
    err = pthread_create(&hilo_kernel,NULL,(void*)esperar_kernel_es,NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_kernel(entradasalida)\n");
        return -3;
    }
    pthread_join(hilo_kernel,NULL);

    return 0; 
}
