#ifndef SERVICIOS_KERNEL_H_
#define SERVICIOS_KERNEL_H_

#include "../include/k_gestor.h"

int asignar_pid();
int generar_ticket();
void list_add_sync(t_list* lista, void* un_elemento, pthread_mutex_t* mutex);
void list_add_pcb_sync(t_list* lista, pcb* un_pcb, pthread_mutex_t* mutex, estado_pcb nuevo_estado);
void actualizar_pcb(pcb* pcb_desactualizado,pcb* pcb_nuevo);
interfaz* obtener_interfaz_con_nombre(char* nombre_interfaz);
void destruir_pcb(pcb* un_pcb);

// Elimina pcb de lista y devuelve true si pudo hacerlo
bool _eliminar_pcb_de_lista_sync(pcb* un_pcb, t_list* una_lista, pthread_mutex_t* mutex);

// Se fija si pcb está en la lista
bool pcb_esta_en_lista(pcb* un_pcb, t_list* una_lista, pthread_mutex_t* mutex);

// Extrae pcb que coincida con pid
pcb* extraer_pcb_de_lista(int pid, t_list* una_lista, pthread_mutex_t* mutex_lista);
pcb* buscar_pcb_en_lista(int pid, t_list* una_lista, pthread_mutex_t* mutex_lista);

pcb* extraer_pcb_de_lista_sistema(pcb* un_pcb);

pcb* buscar_pcb_en_sistema_(int pid);

void agregar_a_ready(pcb* un_pcb);
void cambiar_estado_pcb(pcb* un_pcb, estado_pcb nuevo_estado);

// MANEJO DE RECURSOS
void liberar_recursos_pcb (pcb* un_pcb);
void liberar_recursos(pcb* un_pcb);
void _signal_recurso_exit(char* nombre_recurso, int cantidad_instanciada);
void eliminar_de_lista_recurso(pcb* un_pcb);

void obtener_contexto_pcb(t_buffer* un_buffer, pcb* un_pcb);

#endif