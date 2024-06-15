#ifndef INICIALIZAR_CPU_H
#define INICIALIZAR_CPU_H

#include "c_gestor.h"

#define LRU 1
#define FIFO 2

void inicializar_cpu();
void iniciar_logs();
void iniciar_config();
void imprimir_config();

void crear_TLB();
int get_entradas_tlb();
int get_algoritmo_tlb();

/*
typedef struct tlb {
    t_tlbEntrada* entradas;
    unsigned int tamanio;
    t_list* victim_queue;
} t_tlb;

t_tlb* tlb;
int algoritmo_tlb;

typedef struct tlbEntrada{
    uint32_t pid;
    uint32_t pagina;
    int32_t marco;
    int estado;
}t_tlbEntrada;
*/


#endif 

