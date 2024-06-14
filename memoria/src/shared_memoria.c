#include "../include/shared_memoria.h"



void retardo_respuesta(){
    usleep(RETARDO_RESPUESTA*1000);
}

//---------------------------------------------------------------------------------------------------------
// ACCESO A ESPACIO USUARIO 

// Lectura // 


char* leer_valor_del_espacio_usuario(int tamanio, uint32_t direc_fisica){

    char* puntero_a_datos = espacio_usuario + direc_fisica;
    char* dato_leido = malloc(sizeof(char)*tamanio);
    memcpy(dato_leido, puntero_a_datos, tamanio);
    return dato_leido;
}


char* resolver_solicitud_leer_bloque(t_buffer *unBuffer) {
    int pid = extraer_int_del_buffer(unBuffer);
    uint32_t direc_fisica = extraer_uint32_del_buffer(unBuffer);
    int tamanio = extraer_int_del_buffer(unBuffer);

    char* datos_a_leer = leer_valor_del_espacio_usuario(tamanio, direc_fisica);
    char* datos_leidos = malloc(tamanio + 1);
    memcpy(datos_a_leer,datos_leidos, tamanio);
    datos_leidos[tamanio] = '\0';

    free(datos_a_leer);

    log_info(memoria_logger, "PID: <%d> - Accion: <LEER> - Direccion FIsica: <%d> - Tamaño: <%d>", pid, direc_fisica, tamanio);

    return datos_leidos;

}



//---------------------------------------------------------------------------------------------------------
// Escritura // 


char* resolver_solicitud_escribir_bloque(t_buffer *unBuffer) {
    int pid = extraer_int_del_buffer(unBuffer);
    uint32_t direc_fisica = extraer_uint32_del_buffer(unBuffer);
    int tamanio = extraer_int_del_buffer(unBuffer);
    
    char* datos_a_escribir = malloc(sizeof(char)*tamanio);
    memcpy(datos_a_escribir, unBuffer, sizeof(char)*tamanio); 
    
    char* puntero_a_espacio_usuario = espacio_usuario + direc_fisica;
    memcpy(puntero_a_espacio_usuario, datos_a_escribir, tamanio);

    log_info(memoria_logger, "PID: <%d> - Accion: <ESCRIBIR> - Direccion Fisica: <%d> - Tamaño: <%d>", pid, direc_fisica, tamanio);
    free(datos_a_escribir);
    
    char* resultado = "OK";

    return resultado;
}


//---------------------------------------------------------------------------------------------------------
