#ifndef ESPACIO_USUARIO_H_
#define ESPACIO_USUARIO_H_

#include "m_gestor.h"


void* espacio_usuario;
t_dictionary *  tablas; 
t_list *  lista_marcos;
int  cantidad_marcos;


//FUNCIONES QUE QUIERO COMPARTIR 

void inciar_espacio_usuario();
void finalizar_memoria();



#endif