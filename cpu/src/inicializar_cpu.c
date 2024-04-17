#include "../include/inicializar_cpu.h"

// iniciar estructuras
void inicializar_cpu() {
    iniciar_logs();
    iniciar_config();
    imprimir_config();
}

void iniciar_logs(){
    cpu_logger = log_create("CPU.log", "CPU_log", true, LOG_LEVEL_TRACE);
    if (cpu_logger == NULL){
        perror ("No se pudo crear log para CPU");
        exit(EXIT_FAILURE);
    }

    cpu_logger_extra = log_create("CPU_extra.log", "CPU_extra_log", true, LOG_LEVEL_TRACE);
    if (cpu_logger_extra == NULL){
        perror ("No se pudo crear log extra para CPU");
        exit(EXIT_FAILURE);
    }
}

void iniciar_config(){
    cpu_config = config_create(path_config_CPU);
	if (cpu_config == NULL) {
        perror ("No se pudo crear el config");
		exit(EXIT_FAILURE);
	}

    // SE ASIGNAN LOS VALORES A VARIABLES GLOBALES A PARTIR DE ARCHIVO DE CONFIGURACIÓN
    IP_MEMORIA = config_get_string_value(cpu_config,"IP_MEMORIA");
    PUERTO_MEMORIA = config_get_string_value(cpu_config,"PUERTO_MEMORIA");
    PUERTO_ESCUCHA_DISPATCH = config_get_string_value(cpu_config,"PUERTO_ESCUCHA_DISPATCH");
    PUERTO_ESCUCHA_INTERRUPT = config_get_string_value(cpu_config,"PUERTO_ESCUCHA_INTERRUPT");
    CANTIDAD_ENTRADAS_TLB = config_get_int_value(cpu_config,"CANTIDAD_ENTRADAS_TLB");
    ALGORITMO_TLB = config_get_string_value(cpu_config,"ALGORITMO_TLB");
}

void imprimir_config(){
    log_info(cpu_logger,"IP MEMORIA: %s",IP_MEMORIA);
    log_info(cpu_logger,"PUERTO MEMORIA: %s",PUERTO_MEMORIA);
    // Agregar los para debugear
}

   