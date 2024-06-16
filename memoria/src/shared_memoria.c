#include "../include/shared_memoria.h"



void retardo_respuesta(){
    usleep(RETARDO_RESPUESTA*1000);
}

//---------------------------------------------------------------------------------------------------------
// ACCESO A ESPACIO USUARIO 

// Lectura // 


char* leer_valor_del_espacio_usuario(int tamanio, uint32_t direc_fisica) {
    int bytes_leidos = 0;  
    char* dato_leido = malloc(tamanio);  

    while (bytes_leidos < tamanio) {
        int pagina_actual = (direc_fisica + bytes_leidos) / TAM_PAGINA;
        int desplazamiento_en_pagina = (direc_fisica + bytes_leidos) % TAM_PAGINA;
        int espacio_en_pagina = TAM_PAGINA - desplazamiento_en_pagina;

        char* puntero_a_datos = (char*)espacio_usuario + direc_fisica + bytes_leidos;

        int bytes_a_leer = (tamanio - bytes_leidos > espacio_en_pagina) ? espacio_en_pagina : (tamanio - bytes_leidos);
        
        pthread_mutex_lock(&mutex_espacio_usuario);
        memcpy(dato_leido + bytes_leidos, puntero_a_datos, bytes_a_leer);
        pthread_mutex_unlock(&mutex_espacio_usuario);
      
        bytes_leidos += bytes_a_leer;
    }

    return dato_leido;
}

char* resolver_solicitud_leer_bloque(t_buffer *unBuffer) {
    int pid = extraer_int_del_buffer(unBuffer);
    uint32_t direc_fisica = extraer_uint32_del_buffer(unBuffer);
    int tamanio = extraer_int_del_buffer(unBuffer);

    char* datos_leidos = leer_valor_del_espacio_usuario(tamanio, direc_fisica);
    datos_leidos[tamanio] = '\0';
    free(datos_leidos);

    log_info(memoria_logger, "PID: <%d> - Accion: <LEER> - Direccion FIsica: <%d> - Tamaño: <%d>", pid, direc_fisica, tamanio);

    return datos_leidos;

}



//---------------------------------------------------------------------------------------------------------
// Escritura // 


char* traducir_direccion_fisica(uint32_t direc_fisica, t_proceso* proceso) {
    int pagina = direc_fisica / TAM_PAGINA;
    int desplazamiento = direc_fisica % TAM_PAGINA;

    // Busca la entrada de la tabla de páginas correspondiente
    t_tabla_de_pagina* entrada_pagina = NULL;
    if (pagina < list_size(proceso->tabla_paginas)) {

        pthread_mutex_lock(&(proceso->mutex_tabla_paginas));
        entrada_pagina = list_get(proceso->tabla_paginas, pagina);
        pthread_mutex_unlock(&(proceso->mutex_tabla_paginas));
    }

    if (!entrada_pagina) {
        log_info(memoria_logger, "Error: Página no encontrada en la tabla de páginas\n");
        return NULL;
    }

    t_marco* marco = NULL;
    marco = buscar_marco_segun_numero(entrada_pagina->num_marco);

    if (!marco) {
        log_info(memoria_logger, "Error: Marco no encontrado para la página\n");
        return NULL;
    }

    // Calcula la dirección física base del marco
    char* direccion_fisica_base = (char*)espacio_usuario + (marco->nro_marco * TAM_PAGINA);
    return direccion_fisica_base + desplazamiento;
}

// Estructuras y funciones preexistentes


void escribir_valor_en_espacio_usuario(const char* datos, int tamanio, uint32_t direc_fisica, t_proceso* proceso) {
    int bytes_restantes = tamanio;
    uint32_t offset = direc_fisica;

    while (bytes_restantes > 0) {
        int pagina_actual = offset / TAM_PAGINA;
        int desplazamiento_en_pagina = offset % TAM_PAGINA;
        int espacio_en_pagina = TAM_PAGINA - desplazamiento_en_pagina;

        int bytes_a_escribir = (bytes_restantes > espacio_en_pagina) ? espacio_en_pagina : bytes_restantes;

        char* puntero_a_espacio_usuario = traducir_direccion_fisica(offset, proceso);
        if (!puntero_a_espacio_usuario) {
            log_error(memoria_logger, "Error: Dirección física no válida\n");
            return;
        }
        
        pthread_mutex_lock(&mutex_espacio_usuario);
        memcpy(puntero_a_espacio_usuario, datos, bytes_a_escribir);
        pthread_mutex_unlock(&mutex_espacio_usuario);

        offset += bytes_a_escribir;
        datos += bytes_a_escribir;
        bytes_restantes -= bytes_a_escribir;
    }
}

char* resolver_solicitud_escribir_bloque(t_buffer *unBuffer) {
    int pid = extraer_int_del_buffer(unBuffer);
    uint32_t direc_fisica = extraer_uint32_del_buffer(unBuffer);
    int tamanio = extraer_int_del_buffer(unBuffer);

    char* datos_a_escribir = malloc(tamanio);
    memcpy(datos_a_escribir, unBuffer, tamanio); 
    
    t_proceso* proceso = obtener_proceso_por_pid(pid);
    if (!proceso) {
        free(datos_a_escribir);
        log_error(memoria_logger, "Error: Proceso no encontrado\n");
        return "ERROR";
    }

    escribir_valor_en_espacio_usuario(datos_a_escribir, tamanio, direc_fisica, proceso);

    log_info(memoria_logger, "PID: <%d> - Accion: <ESCRIBIR> - Direccion Fisica: <%d> - Tamaño: <%d>", pid, direc_fisica, tamanio);
    free(datos_a_escribir);
    
    return "OK";
}


//---------------------------------------------------------------------------------------------------------
