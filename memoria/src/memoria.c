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
    
    //Cuano la CPU no rompe, queda esperando en ambos hilos
    pthread_t hilo_cpu;
    int err = pthread_create(&hilo_cpu,NULL,(void*)esperar_cpu_memoria,NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_cpu(memoria))\n");
        return -3;
    }
    pthread_detach(hilo_cpu);


    pthread_t hilo_es;
    err = pthread_create(&hilo_es,NULL,(void*)esperar_es_memoria,NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_es(memoria))\n");
        return -3;
    }
    pthread_detach(hilo_es);
    
    pthread_t hilo_kernel;
    err = pthread_create(&hilo_kernel,NULL,(void*)esperar_kernel_memoria,NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_kernel(memoria))\n");
        return -3;
    }
    pthread_join(hilo_kernel,NULL);


    return EXIT_SUCCESS;
}