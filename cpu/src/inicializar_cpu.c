#include "../include/inicializar_cpu.h"

// iniciar estructuras
void inicializar_cpu() {
    iniciar_logs();
    iniciar_config();
    imprimir_config();
    inicializar_mutexs();
    inicializar_semaforos();
    tlb = crear_TLB();
    pedir_tamanio_pagina();
    inicializar_variables();
}

void inicializar_variables(){
    char* respuesta_marco_escritura = NULL;
    char* respuesta_marco_lectura = NULL;
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

void inicializar_mutexs(){
	pthread_mutex_init(&mutex_manejo_contexto, NULL);
    pthread_mutex_init(&mutex_instruccion, NULL);
    pthread_mutex_init(&mutex_interrupcion_quantum, NULL);
    pthread_mutex_init(&mutex_interrupcion_exit, NULL);
    pthread_mutex_init(&mutex_tlb, NULL);
    pthread_mutex_init(&mutex_ordenCargaGlobal, NULL);
    pthread_mutex_init(&mutex_marco, NULL);
    pthread_mutex_init(&mutex_rta_lectura, NULL);
    pthread_mutex_init(&mutex_rta_escritura, NULL);
    pthread_mutex_init(&mutex_resultado, NULL);
}

void inicializar_semaforos(){
    sem_init(&sem_pedido_tamanio_pag, 0, 0);
    sem_init(&sem_pedido_instruccion, 0, 0);
    sem_init(&sem_pedido_marco, 0, 0);
    sem_init(&sem_rta_resize, 0, 0);
    sem_init(&sem_solicitud_lectura, 0, 0);
    sem_init(&sem_solicitud_escritura, 0, 0);
    sem_init(&sem_proceso, 0, 0);
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
    } else if(strcmp(algoritmo, "LRU") == 0){
        return LRU;
    } else {
        log_error(cpu_logger, "Algoritmo de reemplazo no valido");
        return 0;
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
