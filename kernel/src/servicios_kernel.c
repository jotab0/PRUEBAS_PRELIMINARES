#include "../include/servicios_kernel.h"

int asignar_pid(){
    int valor_pid;

    pthread_mutex_lock(&mutex_pid); // PORQUE NECESITO ASEGURAR QUE SE EJECUTE ESTA FUNCIÓN DE A UNO POR VEZ (Zona Crítica)
    valor_pid = identificador_PID;
    identificador_PID++;
    pthread_mutex_unlock(&mutex_pid);

    return valor_pid;
}