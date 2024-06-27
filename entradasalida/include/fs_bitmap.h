#ifndef FS_BITMAP
#define FS_BITMAP

#include "../include/es_gestor.h"

//acordarse de agregar bitarray de las commons--
int32_t bitmap_encontrar_bloque_libre(void);

void bitmap_mostrar_por_pantalla(void);

void bitmap_marcar_bloque_libre(uint32_t numeroBloque);

void bitmap_marcar_bloque_ocupado(uint32_t numeroBloque);



#endif