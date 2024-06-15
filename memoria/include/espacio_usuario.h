#ifndef ESPACIO_USUARIO_H_
#define ESPACIO_USUARIO_H_

#include "m_gestor.h"
#include "paginacion.h"


void* espacio_usuario;
t_dictionary *  tablas; 
t_list *  lista_marcos;
int  cantidad_marcos;


//FUNCIONES QUE QUIERO COMPARTIR 

void inciar_espacio_usuario();
void finalizar_memoria();
void* pedido_lectura(int dir_fisica);
void* pedido_escritura(int dir_fisica);
void liberar_espacio_usuario(); 
int ampliar_tamanio_proceso(int nuevo_tamanio,t_proceso* proceso);
int reducir_tamanio_proceso(int nuevo_tamanio,t_proceso* proceso);



#endif