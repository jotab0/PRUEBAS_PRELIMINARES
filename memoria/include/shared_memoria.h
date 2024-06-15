#ifndef SHARED_MEMORIA_H_
#define SHARED_MEMORIA_H_

# include "m_gestor.h"

void retardo_respuesta();
void resolver_solicitud_leer_bloque(t_buffer *unBuffer);
void resolver_solicitud_escribir_bloque(t_buffer *unBuffer);


#endif