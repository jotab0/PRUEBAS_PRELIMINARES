#include "../include/encargarse_es.h"




//---------------------------------------------------------------------------------------------------------

void enviar_datos_leidos_es(char* datos_leidos){
    retardo_respuesta();
    t_paquete* un_paquete = crear_paquete_con_buffer(SOLICITUD_LECTURA_MEMORIA_BLOQUE);
    cargar_string_a_paquete(un_paquete, datos_leidos);
    enviar_paquete(un_paquete, fd_es);
    eliminar_paquete(un_paquete);
    free(datos_leidos);
       
}

void resolver_solicitud_leer_bloque_es(t_buffer* unBuffer){

    char* datos_leidos = resolver_solicitud_leer_bloque(unBuffer);
    enviar_datos_leidos_es(datos_leidos);

}


//---------------------------------------------------------------------------------------------------------

void enviar_respuesta_escritura_en_espacio_usuario_es(char* respuesta){
    retardo_respuesta();
    t_paquete* un_paquete = crear_paquete_con_buffer(SOLICITUD_ESCRITURA_MEMORIA_BLOQUE);
    cargar_string_a_paquete(un_paquete, respuesta);
    enviar_paquete(un_paquete, fd_es);
    eliminar_paquete(un_paquete);
}

void resolver_solicitud_escribir_bloque_es(t_buffer* unBuffer){

    char* respuesta = resolver_solicitud_escribir_bloque(unBuffer);
    enviar_respuesta_escritura_en_espacio_usuario_es(respuesta);
}


//---------------------------------------------------------------------------------------------------------

void encargarse_es(int cliente_socket_es){

        int numero = 1; 
        
        while(numero){
            t_buffer* unBuffer;
            int codigo_operacion = recibir_operacion(cliente_socket_es);

            switch(codigo_operacion){

                case SOLICITUD_LECTURA_MEMORIA_BLOQUE: 
                unBuffer = recibir_buffer(cliente_socket_es);
                resolver_solicitud_leer_bloque_es(unBuffer);
                break;

                case SOLICITUD_ESCRITURA_MEMORIA_BLOQUE:
                unBuffer = recibir_buffer(cliente_socket_es);
                resolver_solicitud_escribir_bloque_es(unBuffer);
                break; 

            }

            free(unBuffer);
        }


    }
