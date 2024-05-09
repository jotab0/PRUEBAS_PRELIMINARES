#ifndef OPERACIONES_PROCESP_H_
#define OPERACIONES_PROCESO_H_

#include "m_gestor.h"

t_proceso* obtener_proceso_por_pid(int pid);
char*  extraer_instruccion_por_ip(t_proceso* proceso, int ip_proceso);

#endif