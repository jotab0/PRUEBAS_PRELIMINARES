#ifndef CICLOINSTRUCCION_H_
#define CICLOINSTRUCCION_H_ 

#include "c_gestor.h"

t_log* cpu_logger;
t_log* cpu_log_obligatorio;

char** opcode_headers;

char** instruccion_elements;

//Semaforos
sem_t sem_control_fetch_decode;
sem_t sem_control_decode_execute;
sem_t sem_control_execute_checkinterruption; //hace falta este?

#endif
