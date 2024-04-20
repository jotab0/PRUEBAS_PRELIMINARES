#include "../include/memoria.h"

// SERVIDOR DE: CPU, KERNEL, ENTRADASALIDA
// CLIENTE DE: - 

// MENSAJES DE PRUEBA
void mandar_mensaje_a_cpu(){
    enviar_mensaje("Hola CPU", fd_cpu);
}

int main(int argc, char* argv[]) {
    
    // INCIA MEMORIA
    memoria_logger = log_create("Memoria.log", "Memoria_log", true, LOG_LEVEL_INFO);
    if (memoria_logger == NULL){
        perror ("No se pudo crear log para la memoria");
        exit(EXIT_FAILURE);
    }

<<<<<<< HEAD
    memoria_logger_extra = log_create("Memoria_extra_log.log", "Memoria_extra_log", true, LOG_LEVEL_TRACE);
    if (memoria_logger == NULL){
        perror ("No se pudo crear log extra para la memoria");
        exit(EXIT_FAILURE);
=======
    // mensaje a cpu
    pthread_t hilo_mensaje_a_cpu;
    err = pthread_create(&hilo_mensaje_a_cpu,NULL,(void*)mandar_mensaje_a_cpu,NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_mensaje_a_cpu(memoria)\n");
        return -3;
    }
    pthread_detach(hilo_mensaje_a_cpu);


    pthread_t hilo_es;
    err = pthread_create(&hilo_es,NULL,(void*)esperar_es_memoria,NULL);
    if (err!=0){
        perror("Fallo de creación de hilo_es(memoria))\n");
        return -3;
>>>>>>> ramaCami
    }

    memoria_config = config_create(path_config_Memoria);

	if (memoria_config == NULL) {
        perror ("No se pudo crear el config para la memoria");
		exit(EXIT_FAILURE);
	}

    PUERTO_ESCUCHA = config_get_string_value(memoria_config,"PUERTO_ESCUCHA");
    TAM_MEMORIA = config_get_int_value(memoria_config,"TAM_MEMORIA");
    TAM_PAGINA = config_get_int_value(memoria_config,"TAM_PAGINA");
    PATH_INSTRUCCIONES= config_get_string_value(memoria_config,"PATH_INSTRUCCIONES");
    RETARDO_RESPUESTA = config_get_int_value(memoria_config,"RETARDO_RESPUESTA");

    log_info(memoria_logger,"PATH INSTRUCCIONES: %s",PATH_INSTRUCCIONES);

    // INICIAR SERVIDOR DE MEMORIA
    
    //int fd_kernel = iniciar_servidor()

    return 0;
}
