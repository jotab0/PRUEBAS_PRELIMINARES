#ifndef CICLOINSTRUCCION_CPU_H
#define CICLOINSTRUCCION_CPU_H

#include "c_gestor.h"

void realizarCicloInstruccion();
void decodeYExecute();

uint32_t* detectar_registro();

#endif
