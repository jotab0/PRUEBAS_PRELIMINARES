#ifndef ENCARGARSE_KERNEL_H_
#define ENCARGARSE_KERNEL_H_

#include "m_gestor.h"
#include "paginacion.h"
#include "operaciones_proceso.h"

void encargarse_kernel(int cliente_socket_cpu);
void  respuesta_kernel_de_solicitud_iniciar_proceso();

t_proceso* iniciar_estructura_proceso(t_buffer* unBuffer);

t_list* obtener_instrucciones_del_archivo(char* path_archivo_instrucciones);
t_list* procesar_archivo(const char* path_archivo);


#endif