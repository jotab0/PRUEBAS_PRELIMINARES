#include "../include/memoria.h"


// SERVIDOR DE: CPU, KERNEL, ENTRADASALIDA
// CLIENTE DE: - 



int main(int argc, char* argv[]) {
    
    // Inicializar estructuras de memoria
    inicializar_memoria();

    // Iniciar servidor de memoria
    fd_memoria = iniciar_servidor(PUERTO_ESCUCHA,memoria_logger,"!! Servidor MEMORIA iniciada !!");  
    
    //CONECTAR CON CLIENTES
    // Esperar conexion de CPU
    fd_cpu = esperar_cliente(fd_memoria, memoria_logger,"CPU");

    // Esperar conexion de kernel 
    fd_kernel = esperar_cliente(fd_memoria, memoria_logger,"Kernel");
    
    // Esperar conexion de entrada y salida
    fd_es = esperar_cliente(fd_memoria, memoria_logger,"E/S");
    
    pthread_t hilo_cpu;
    pthread_create(&hilo_cpu,NULL,(void*)esperar_cpu,NULL);
    pthread_detach(esperar_cpu);
    
    pthread_t hilo_kernel;
    pthread_create(&hilo_kernel,NULL,(void*)esperar_kernel,NULL);
    pthread_detach(esperar_kernel);

    pthread_t hilo_es;
    pthread_create(&hilo_es,NULL,(void*)esperar_es,NULL);
    pthread_join(esperar_cpu,NULL);
    // FINALIZAR MEMORIA 

    return EXIT_SUCCESS;
}