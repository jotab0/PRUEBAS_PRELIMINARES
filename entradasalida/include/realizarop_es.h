#ifndef REALIZAROP_ES
#define REALIZAROP_ES

#include "es_gestor.h"

void enviar_handshake(int socket, char* nombre_interfaz, char* tipo_interfaz);
configuracion_t cargar_configuracion(char* path);

#endif