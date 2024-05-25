#include "../include/encargarse_cpu.h"

void enviar_tamanio_pagina(int cliente_socket);
void resolver_solicitud_instruccion(t_buffer *unBuffer);

void encargarse_cpu(int cliente_socket_cpu){
        enviar_tamanio_pagina(cliente_socket_cpu);

        int numero =1;
        while(numero){
            t_buffer* unBuffer;
		    int codigo_operacion = recibir_operacion(cliente_socket_cpu);
            switch(codigo_operacion){
                case SOLICITUD_INSTRUCCION:
                    unBuffer = recibir_un_paquete(fd_cpu);
					resolver_solicitud_instruccion(unBuffer);
                    break;

                case -1:
				log_error(memoria_logger, "SE DESCONECTO CPU");
				close(cliente_socket_cpu);
				numero= 0;
                return;

                default:
				log_error(memoria_logger, "NO ES UNA OPERACION");
				break;
			}
		free(unBuffer);
        }
}

//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
//FUNCIONES NECESARIAS


void mandar_instruccion_a_cpu(char* instruccion){
    //retardo_respuesta_cpu_fs();
    t_paquete* paquete = crear_paquete_con_buffer(SOLICITUD_INSTRUCCION);
    cargar_string_a_paquete(paquete, instruccion);
    enviar_paquete(paquete, fd_cpu);
    eliminar_paquete(paquete);    
}


//------------------------------------------------------------------------------------------------------------


void enviar_tamanio_pagina(int cliente_socket){
    t_paquete* un_paquete = crear_paquete_con_buffer(SOLICITUD_INFO_MEMORIA);
    cargar_int_a_paquete(un_paquete, TAM_PAGINA);
    enviar_paquete(un_paquete, fd_cpu);
    eliminar_paquete(un_paquete);
}

/*
 // DEFINICION DE TIPO FUNCION DE PUNTERO PARA LAS OPERACIONES DE CPU
 typedef void (*operacion_handler_t)(t_buffer*);

 // FUNCIONES DE MANEJO DE OPERACIONES 
 
 void solicitud_info_memoria(t_buffer *unBuffer);
 void resolver_solicitud_instruccion(t_buffer* unBuffer);
 void resolver_solicitud_ejecucion(t_buffer* unBuffer);
 void resolver_solicitud_consulta_pagina(t_buffer* unBuffer);
 void resolver_solicitud_leer_bloque(t_buffer* unBuffer);
 void resolver_solicitud_escribir_bloque(t_buffer* unBuffer);


 // ARRAY DE PUNTEROS A FUNCIONES 

 operacion_handler_t funcion_handler[] ={
    [SOLICITUD_INFO_MEMORIA]   = solicitud_info_memoria,
    [SOLICITUD_INSTRUCCION]    = resolver_solicitud_instruccion,
    [SOLICITUD_EJECUCION]      = resolver_solicitud_ejecucion,
    [SOLICITUD_CONSULTA_PAG]   = resolver_solicitud_consulta_pagina,
    [SOLICITUD_LECTURA_MEMORIA_BLOQUE]   = resolver_solicitud_leer_bloque,
    [SOLICITUD_ESCRITURA_MEMORIA_BLOQUE] = resolver_solicitud_escribir_bloque
 };



void encargarse_cpu(int cliente_socket_cpu){
    enviar_tamanio_pagina(cliente_socket_cpu);

    while (true) {
        int codigo_operacion;
        t_buffer* unBuffer;

        codigo_operacion = recibir_operacion(cliente_socket_cpu);
        if (codigo_operacion == -1) {
            log_error(memoria_logger, "SE DESCONECTO CPU");
            close(cliente_socket_cpu);
            return;
        }

        unBuffer = recibir_paquete_completo(cliente_socket_cpu);
        if (unBuffer == NULL) {
            log_error(memoria_logger, "Error al recibir el paquete");
            close(cliente_socket_cpu);
            return;
        }

        if (codigo_operacion >= 0 && codigo_operacion < sizeof(funcion_handler) / sizeof(operacion_handler_t)) {
            if (funcion_handler[codigo_operacion]) {
                funcion_handler[codigo_operacion](unBuffer);
            } else {
                log_error(memoria_logger, "ERROR: No se conoce la operacion: %d", codigo_operacion);
                close(cliente_socket_cpu);
                free(unBuffer);
                return;
            }
        }
        free(unBuffer); 
    }
}*/

//---------------------------------------------------------------------------------------------------------
// Implementación de funciones de manejo de operaciones

void solicitud_info_memoria(t_buffer *unBuffer) {
   unBuffer = recibir_buffer(fd_cpu);
}


void resolver_solicitud_instruccion(t_buffer *unBuffer) {

    // Obtener PID e IP desde el buffer
    int pid = extraer_int_del_buffer(unBuffer);
    int ip  = extraer_int_del_buffer(unBuffer);

    // Buscar el proceso correspondiente al PID
    t_proceso* proceso = obtener_proceso_por_pid(pid); 
    if (proceso == NULL) {
        log_error(memoria_logger, "No se encontró el proceso con PID: %d", pid);
        return;
    }

    // Obtener la instrucción específica usando el IP
    char* instruccion = extraer_instruccion_por_ip(proceso, ip);
    if (instruccion == NULL) {
        log_error(memoria_logger, "No se encontró la instrucción en el IP: %d para el PID: %d", ip, pid);
        return;   
    }
    // Registrar información del proceso y la instrucción
    log_info(memoria_logger, "Proceso [PID: %d, IP: %d]: %s", pid, ip, instruccion);
    mandar_instruccion_a_cpu(instruccion);
}


//---------------------------------------------------------------------------------------------------------

void resolver_solicitud_ejecucion(t_buffer *unBuffer) {
    
}

void resolver_solicitud_consulta_pagina(t_buffer *unBuffer) {
    
}

void resolver_solicitud_leer_bloque(t_buffer *unBuffer) {
    
}

void resolver_solicitud_escribir_bloque(t_buffer *unBuffer) {
    
}