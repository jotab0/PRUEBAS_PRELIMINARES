#ifndef ENCARGARSE_CPU_H_
#define ENCARGARSE_CPU_H_

#include "m_gestor.h"
#include "../include/operaciones_proceso.h"
#include "../include/shared_memoria.h"
#include "../include/paginacion.h"
#include "../include/espacio_usuario.h"

void encargarse_cpu(int cliente_socket_cpu);
void retardo_respuesta();

#endif