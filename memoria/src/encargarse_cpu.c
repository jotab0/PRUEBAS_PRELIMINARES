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
                    unBuffer = recibir_buffer(fd_cpu);
					resolver_solicitud_instruccion(unBuffer);
                    break;

                case SOLICITUD_PAGINA:
                    unBuffer = recibir_buffer(fd_cpu);
					resolver_solicitud_consulta_pagina(unBuffer);
                    break;

                case SOLICITUD_ESCRITURA_MEMORIA_BLOQUE:
                    unBuffer = recibir_buffer(fd_cpu);
                    resolver_solicitud_escribir_bloque(unBuffer);
                    break;

                case SOLICITUD_LECTURA_MEMORIA_BLOQUE:
                    unBuffer = recibir_buffer(fd_cpu);
                    resolver_solicitud_leer_bloque(unBuffer);
                    break;

                case AJUSTAR_TAMANIO:
                    unBuffer = recibir_buffer(fd_cpu);
                    resolver_ajustar_tamanio(unBuffer);
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

// Implementación de funciones de manejo de operaciones

/*
void solicitud_info_memoria(t_buffer *unBuffer) {
   unBuffer = recibir_buffer(fd_cpu);
}
*/
//------------------------------------------------------------------------------------------------------------


void enviar_tamanio_pagina(int cliente_socket){
    t_paquete* un_paquete = crear_paquete_con_buffer(SOLICITUD_INFO_MEMORIA);
    cargar_int_a_paquete(un_paquete, TAM_PAGINA);
    enviar_paquete(un_paquete, fd_cpu);
    eliminar_paquete(un_paquete);
}


//---------------------------------------------------------------------------------------------------------

void mandar_instruccion_a_cpu(char* instruccion){
    retardo_respuesta();
    t_paquete* paquete = crear_paquete_con_buffer(SOLICITUD_INSTRUCCION);
    cargar_string_a_paquete(paquete, instruccion);
    enviar_paquete(paquete, fd_cpu);
    eliminar_paquete(paquete);    
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
    // Registro la información del proceso y de la instrucción
    log_info(memoria_logger, "Proceso [PID: %d, IP: %d]: %s", pid, ip, instruccion);
    mandar_instruccion_a_cpu(instruccion);
}


//---------------------------------------------------------------------------------------------------------

void mandar_num_marco(int numero_marco){
    retardo_respuesta();
    t_paquete* un_paquete = crear_paquete_con_buffer(SOLICITUD_PAGINA);
    cargar_int_a_paquete(un_paquete, numero_marco);
    enviar_paquete(un_paquete, fd_cpu);
    eliminar_paquete(un_paquete);
}


void resolver_solicitud_consulta_pagina(t_buffer *unBuffer) {
    int pid = extraer_int_del_buffer(unBuffer);
    int num_pagina  = extraer_int_del_buffer(unBuffer);

    t_proceso* proceso = obtener_proceso_por_pid(pid);
    int num_marco_buscado = devolver_numero_de_marco(proceso,num_pagina);

    mandar_num_marco(num_marco_buscado);
}

//---------------------------------------------------------------------------------------------------------

void mandar_resultado_resize(int resultado){
    retardo_respuesta();
    t_paquete* paquete = crear_paquete_con_buffer(RTA_AJUSTAR_TAMANIO);
    cargar_int_a_paquete(paquete, resultado);
    enviar_paquete(paquete, fd_cpu);
    eliminar_paquete(paquete);    
}

void resolver_ajustar_tamanio(unBuffer){
    int pid = extraer_int_del_buffer(unBuffer);
    int tamanio_nuevo = extraer_int_del_buffer(unBuffer);
    int resultado;

    t_proceso* proceso = obtener_proceso_por_pid(pid);

    if(tamanio_nuevo > proceso->size){
        resultado = ampliar_tamanio_proceso(tamanio_nuevo,proceso);
    }
    else{
        resultado = reducir_tamanio_proceso(tamanio_nuevo,proceso);
    }
    
    mandar_resultado_resize(resultado);
}


