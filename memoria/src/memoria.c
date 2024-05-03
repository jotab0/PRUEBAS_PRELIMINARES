#include "../include/memoria.h"

// SERVIDOR DE: CPU, KERNEL, ENTRADASALIDA
// CLIENTE DE: - 

// MENSAJES DE PRUEBA
void mandar_mensajes(){
    sleep(10);
    enviar_mensaje("Hola CPU, soy memoria", fd_cpu);
    enviar_mensaje("Hola KERNEL, soy memoria", fd_kernel);
    enviar_mensaje("Hola E/S, soy memoria", fd_es);   
}

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
  
  
///// HILOS /////

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

    pthread_t hilo_mensaje_a_cpu;
    err = pthread_create(&hilo_mensaje_a_cpu,NULL,(void*)mandar_mensajes,NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_mensaje_a_cpu(memoria)\n");
        return -3;
    }
    pthread_detach(hilo_mensaje_a_cpu);


    pthread_t hilo_kernel;
    err = pthread_create(&hilo_kernel,NULL,(void*)esperar_kernel_memoria,NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_kernel(memoria))\n");
        return -3;
    }
    pthread_join(hilo_kernel,NULL);
    
    //int fd_kernel = iniciar_servidor()

 ///// PROCESOS /////

    t_list* list_procesos_recibidos = list_create();

    return 0;
}


///// COMUNICACIONES CON LOS MODULOS -> ATENDER LOS PEDIDOS 

    
    void cliente_segun_modulo(int conexion, t_buffer* unBuffer){
        int identificador_modulo = int_del_buffer(unBuffer);

        switch (identificador_modulo) {
		case CPU:
			fd_cpu = conexion;
			log_info(memoria_logger, "CPU se conecto correctamente");
			encargarse_cpu(fd_cpu);

			break; 
        }
    }

