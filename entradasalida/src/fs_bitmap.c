
/* #include <../include/fs_bitmap.h>


int32_t encontrar_bloque_libre_en_bitmap()
{   
    // false = 0 --> libre
    // true = 1 --> ocupado
    uint32_t indice;
    bool estaOcupado;
    for (indice = 0; indice < (bitmap->tamanio * 8); indice++)
    {   
        estaOcupado = bitarray_test_bit(bitmap->bitarray, indice);//bitarray_test_bit se usa para verificar si un bloque esta libre o ocupado
        log_acceso_bitmap(indice, estaOcupado);
        // Si encuentra un bloque que esté en 0 devuelve la posición de ese bloque
        if (!estaOcupado)
        {
            return indice;
        }
    }
    // Si no encuentra un bloque libre, retorna -1
    return -1;
}

// Para pruebas
void mostrar_bitmap_en_pantalla()
{   
    // false = 0 --> libre
    // true = 1 --> ocupado
    uint32_t indice;
    bool estadoBloque;
    for (indice = 0; indice < (bitmap->tamanio * 8); indice++)
    {
        estadoBloque = bitarray_test_bit(bitmap->bitarray, indice);
        if (estadoBloque == 0)
        {
            log_info(es_logger, "Bloque %u: 0", indice);
        }
        else
        {
            log_info(es_logger, "Bloque %u: 1", indice);
        }
    }
}

void marcar_bloque_como_libre(uint32_t numeroDeBloque) // 0 --> libre
{
    bitarray_clean_bit(bitmap->bitarray, numeroDeBloque);
    // Sincronizar los cambios en el archivo y verificar que se haga de forma correcta
    if (msync(bitmap->direccion, bitmap->tamanio, MS_SYNC) == -1) {
        log_error(es_logger, "Error al sincronizar los cambios en el Bitmap");
    }
    log_acceso_bitmap(numeroDeBloque, 0);
}

void marcar_bloque_como_ocupado(uint32_t numeroDeBloque) // 1 --> ocupado
{
    bitarray_set_bit(bitmap->bitarray, numeroDeBloque);
    // Sincronizar los cambios en el archivo y verificar que se haga de forma correcta
    if (msync(bitmap->direccion, bitmap->tamanio, MS_SYNC) == -1) {
        log_error(es_logger, "Error al sincronizar los cambios en el Bitmap");
    }
    log_acceso_bitmap(numeroDeBloque, 1);
} */