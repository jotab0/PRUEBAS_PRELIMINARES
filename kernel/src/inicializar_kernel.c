#include "../include/inicializar_kernel.h"

void inicializar_kernel(){
    inicializar_logs();
    inicializar_configs();
    imprimir_configs();
}

void inicializar_logs(){
    
    kernel_logger = log_create("Kernel.log", "Kernel_log", true, LOG_LEVEL_TRACE);
    if (kernel_logger == NULL){
        perror ("No se pudo crear log para el Kernel");
        exit(EXIT_FAILURE);
    }

    kernel_logger_extra = log_create("Kernel_extra.log", "Kernel_extra_log", true, LOG_LEVEL_TRACE);
    if (kernel_logger == NULL){
        perror ("No se pudo crear log extra para el Kernel");
        exit(EXIT_FAILURE);
    }
}

void inicializar_configs(){
    
    kernel_config = config_create("/home/utnso/Documents/tp-2024-1c-ubuntunel/kernel/Kernel.config");

	if (kernel_config == NULL) {
        perror ("No se pudo crear el config para el módulo kernel");
		exit(EXIT_FAILURE);
	}
        PUERTO_ESCUCHA = config_get_string_value(kernel_config,"PUERTO_ESCUCHA");
        IP_MEMORIA = config_get_string_value(kernel_config,"IP_MEMORIA");
        PUERTO_MEMORIA = config_get_string_value(kernel_config,"PUERTO_MEMORIA");
        IP_CPU = config_get_string_value(kernel_config,"IP_CPU");
        PUERTO_CPU_DISPATCH = config_get_string_value(kernel_config,"PUERTO_CPU_DISPATCH");
        PUERTO_CPU_INTERRUPT = config_get_string_value(kernel_config,"PUERTO_CPU_INTERRUPT");
        ALGORITMO_PLANIFICACION = config_get_string_value(kernel_config,"ALGORITMO_PLANIFICACION");
        QUANTUM = config_get_int_value(kernel_config,"QUANTUM"); 
        RECURSOS = config_get_array_value(kernel_config,"RECURSOS");
        INSTANCIAS_RECURSOS = config_get_array_value(kernel_config,"INSTANCIAS_RECURSOS");
        GRADO_MULTIPROGRAMACION = config_get_int_value(kernel_config,"GRADO_MULTIPROGRAMACION");
}

void imprimir_configs(){
    log_info(kernel_logger, "KERNEL ALGORITMO_PLANIFICACION: %s", ALGORITMO_PLANIFICACION);
}

    
