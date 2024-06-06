#include "../include/encargarse_cpu.h"

void enviar_tamanio_pagina(int cliente_socket);
void resolver_solicitud_instruccion(t_buffer *unBuffer);
void retardo_respuesta();

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
    
    t_paquete* paquete = crear_paquete_con_buffer(SOLICITUD_INSTRUCCION);
    cargar_string_a_paquete(paquete, instruccion);
    enviar_paquete(paquete, fd_cpu);
    eliminar_paquete(paquete);    
}


//------------------------------------------------------------------------------------------------------------

void retardo_respuesta(){
    sleep(RETARDO_RESPUESTA);
}

void enviar_tamanio_pagina(int cliente_socket){
    t_paquete* un_paquete = crear_paquete_con_buffer(SOLICITUD_INFO_MEMORIA);
    cargar_int_a_paquete(un_paquete, TAM_PAGINA);
    enviar_paquete(un_paquete, fd_cpu);
    eliminar_paquete(un_paquete);
}


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