#include "../include/shared_memoria.h"



void retardo_respuesta(){
    usleep(RETARDO_RESPUESTA*1000);
}

//---------------------------------------------------------------------------------------------------------
// ACCESO A ESPACIO USUARIO 

// Lectura // 

void resolver_solicitud_leer_bloque(t_buffer *unBuffer) {
    int pid = extraer_int_del_buffer(unBuffer);
    int dir_fisica = extraer_int_del_buffer(unBuffer);

    uint32_t* valor_leido = malloc(sizeof(uint32_t));
	uint32_t dato_retorno;

	memcpy(valor_leido, espacio_usuario + dir_fisica, sizeof(uint32_t));
	dato_retorno = *valor_leido;

	free(valor_leido);
    log_info(memoria_logger, "PID: <%d> - Accion: LEER- Direccion fisica: <%d> - Tamaño <%d>", pid, dir_fisica, sizeof(uint32_t));
}


// Escritura // 

void resolver_solicitud_escribir_bloque(t_buffer *unBuffer) {
    //tengo que sacar la instruccion 
}


//---------------------------------------------------------------------------------------------------------