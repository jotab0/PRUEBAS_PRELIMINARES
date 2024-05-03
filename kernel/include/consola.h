#ifndef CONSOLA_H_
#define CONSOLA_H_

#include "../include/k_gestor.h"
#include "../include/shared_kernel.h"

#include <utils/include/shared.h>

void iniciar_consola();
bool validacion_de_instruccion_de_consola(char* leido);
void atender_instruccion(char* leido);
void f_iniciar_proceso(t_buffer* un_buffer);

#endif