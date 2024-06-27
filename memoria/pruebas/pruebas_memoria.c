#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include "../include/paginacion.h"
#include "../include/inicializar_memoria.h"
#include "../include/operaciones_proceso.h"
#include "../include/memoria.h"
#include "../include/shared_memoria.h"
#include "../include/m_gestor.h"

void simulate_kernel_requests() {
    // Simulate kernel requests
    printf("\nSimulating KERNEL requests...\n");

    // Example: Request to initialize memory
    printf("1. Initializing memory...\n");
    inicializar_memoria();

    // Example: Request to allocate memory for a process
    printf("2. Allocating memory for process...\n");
    t_proceso *new_process = malloc(sizeof(t_proceso));
    new_process->pid_proceso = 1;
    new_process->size = 1024; // Example size
    new_process->pathInstrucciones = "example_path";
    new_process->tabla_paginas = list_create();
    pthread_mutex_init(&(new_process->mutex_tabla_paginas), NULL);

    inicializar_tabla_de_paginas(new_process);
    asignar_marco_disponible_a_proceso_vacio(new_process);

    free(new_process);
}

void simulate_cpu_requests() {
    // Simulate CPU requests
    printf("\nSimulating CPU requests...\n");

    // Example: Request to read from user space
    printf("1. Reading from user space...\n");
    uint32_t physical_address = 0;
    char *read_data = leer_valor_del_espacio_usuario(10, physical_address);
    if (read_data) {
        printf("Data read: %s\n", read_data);
        free(read_data);
    } else {
        printf("Failed to read data from user space.\n");
    }

    // Example: Request to write to user space
    printf("2. Writing to user space...\n");
    char data_to_write[] = "Hello, world!";
    escribir_valor_en_espacio_usuario(data_to_write, sizeof(data_to_write), physical_address, NULL);
}

void simulate_io_requests() {
    // Simulate I/O requests
    printf("\nSimulating ENTRADA-SALIDA requests...\n");

    // Example: Request to delay response
    printf("1. Delaying response...\n");
    retardo_respuesta();
}

int main() {
    // Simulate requests from different modules
    simulate_kernel_requests();
    simulate_cpu_requests();
    simulate_io_requests();

    return 0;
}
