#ifndef CONSOLA_H_
#define CONSOLA_H_

#include "../include/k_gestor.h"
#include "../include/servicios_kernel.h"

#include <utils/include/shared.h>

void iniciar_consola_interactiva();
bool validacion_de_instruccion_de_consola(char* leido);
void atender_instruccion_validada(char* leido);
void f_iniciar_proceso(t_buffer* un_buffer);

#endif