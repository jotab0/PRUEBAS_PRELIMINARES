#ifndef SERVICIOS_KERNEL_H_
#define SERVICIOS_KERNEL_H_

#include "../include/k_gestor.h"

int asignar_pid();
int generar_ticket();
void list_add_sync(t_list* lista, void* un_elemento, pthread_mutex_t* mutex);

#endif