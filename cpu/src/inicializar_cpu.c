#include "../include/inicializar_cpu.h"

// iniciar estructuras
void inicializar_cpu() {
    iniciar_logs();
    iniciar_config();
    tlb = crear_TLB();
    imprimir_config();
    pedir_tamanio_pagina();
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

void inicializar_mutexes(){
	pthread_mutex_init(&mutex_manejo_contexto, NULL);
}

t_tlb* crear_TLB(){
    tlb = malloc(sizeof(t_tlb));
    tlb->tamanio = get_entradas_tlb();
    tlb->entradas = (t_tlbEntrada*) calloc(tlb->tamanio, sizeof(t_tlbEntrada));
    // mutex tlb

    // pongo todas las entradas como libres
    for (int i = 0; i < tlb->tamanio; i++){
        tlb->entradas[i].estado = LIBRE;
    }

    algoritmo_tlb = get_algoritmo_tlb();
    ordenCargaGlobal = 0;

    return tlb;

}
   
int get_entradas_tlb(){
    int entradas;
    entradas = config_get_int_value(cpu_config, "CANTIDAD_ENTRADAS_TLB");
    return entradas;
}

int get_algoritmo_tlb(){
    char* algoritmo;
    algoritmo = config_get_string_value(cpu_config, "ALGORITMO_TLB");
    if(strcmp(algoritmo, "FIFO") == 0){
        return FIFO;
    } else{
        return LRU;
    }
    free(algoritmo);
}

void pedir_tamanio_pagina(){
    // le solicito a memoria el tamanio de las pags
    t_paquete* unPaquete = crear_paquete_con_buffer(SOLICITUD_INFO_MEMORIA);
    //cargar_int_a_paquete(unPaquete, contexto->proceso_pid);
    enviar_paquete(unPaquete, fd_memoria);
    eliminar_paquete(unPaquete);
}
