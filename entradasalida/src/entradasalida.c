#include "../include/entradasalida.h"

// SERVIDOR DE: -
// CLIENTE DE: KERNEL, MEMORIA 


void mandar_mesajes(){
    sleep(10);
    enviar_mensaje("Hola memoria, soy E/S",fd_memoria);
    enviar_mensaje("Hola kernel, soy E/S",fd_kernel);
}


int main(int argc, char* argv[]) {

    // Inicializar estructuras de ES
    inicializar_es();

    if (argc != 3) {
        fprintf(stderr, "Uso: %s <nombre_interfaz> <archivo_configuracion>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char* nombre_interfaz = argv[1];
    char* es_config = argv[2];

    //configuracion_t config = cargar_configuracion(archivo_configuracion);

    enviar_handshake(nombre_interfaz, TIPO_INTERFAZ);

    atender_es_kernel();

    //Me conecto como Cliente a MEMORIA
    fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
    log_info(es_logger, "Conexion con Memoria exitosa.");
    
    //Me conecto como Cliente a KERNEL
    fd_kernel = crear_conexion(IP_KERNEL, PUERTO_KERNEL);
    log_info(es_logger, "Conexion con Kernel exitosa.");
    
    pthread_t hilo_memoria;
    int err = pthread_create(&hilo_memoria,NULL,(void*)esperar_memoria_es,NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_memoria(entradasalida))\n");
        return -3;
    }
    pthread_detach(hilo_memoria);

    pthread_t hilo_mensajes;
    err = pthread_create(&hilo_mensajes,NULL,(void*)mandar_mesajes,NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_k_interrupt(cpu)\n");
        return -3;
    }
    pthread_detach(hilo_mensajes);
  
    pthread_t hilo_kernel;
    err = pthread_create(&hilo_kernel,NULL,(void*)esperar_kernel_es,NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_kernel(entradasalida)\n");
        return -3;
    }
    pthread_join(hilo_kernel,NULL);

    return 0; 
}

void enviar_handshake(char* nombre_interfaz, char* TIPO_INTERFAZ) {
    t_paquete* un_paquete = crear_paquete_con_buffer(HANDSHAKE_K_ES);
    cargar_string_a_paquete(un_paquete, nombre_interfaz);
    cargar_string_a_paquete(un_paquete, TIPO_INTERFAZ);

    enviar_paquete(un_paquete, fd_kernel);

    eliminar_paquete(un_paquete);
}

/* configuracion_t cargar_configuracion(char* archivo_configuracion) {
    configuracion_t config;
    FILE *file = fopen(archivo_configuracion, "rt");
    if (file == NULL) {
        perror("Error al abrir el archivo de configuración");
        exit(EXIT_FAILURE);
    }

    fscanf(file, "TIPO_INTERFAZ=%s\n", config.tipo_interfaz);
    fscanf(file, "TIEMPO_UNIDAD_TRABAJO=%d\n", &config.tiempo_unidad_trabajo);
    fscanf(file, "IP_KERNEL=%s\n", config.ip_kernel);
    fscanf(file, "PUERTO_KERNEL=%d\n", &config.puerto_kernel);

    fclose(file);
    return config;
} */