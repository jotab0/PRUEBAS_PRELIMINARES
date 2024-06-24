#include "../include/encargarse_kernel.h"


//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
//FUNCIONES NECESARIAS


//-------------------------------------------------------------------------------------
void enviar_respuesta_liberar_estructuras(int resultado){
    retardo_respuesta();
    t_paquete* un_paquete = crear_paquete_con_buffer(RTA_LIBERAR_ESTRUCTURA);
    cargar_int_a_paquete(un_paquete,resultado);
    enviar_paquete(un_paquete, fd_kernel);
    eliminar_paquete(un_paquete);
}


void liberar_estructura_proceso(t_buffer* unBuffer){
    int pid = extraer_int_del_buffer(unBuffer);
    t_proceso* proceso = obtener_proceso_por_pid(pid);
    int cantidad_paginas = cantidad_paginas_necesarias(proceso->size);
    int resultado;

    if(list_remove_element(lista_procesos, proceso)){
        destruir_proceso(proceso);
        log_info(memoria_logger, "DESTRUCCION DE: PID: <%d> - TAMANIO: <%d> ", pid, cantidad_paginas);
        resultado = 0;
    }else{
        log_error(memoria_logger, "Erorr: el proceso con el PID: <%d> no fue encontrado", pid);
        resultado = -1;
    }

    enviar_respuesta_liberar_estructuras(resultado);


}


//-------------------------------------------------------------------------------------
  // 1) saco los datos segun me mando kernel :path -> pid 
 //  2) creo proceso con el formato del struct 
//   3) lo sumo a mi litsa de procesos

void iniciar_estructura_proceso(t_buffer* unBuffer){
    char* path  = extraer_string_del_buffer(unBuffer);
    int pid     = extraer_int_del_buffer(unBuffer);
        
    crear_proceso_nuevo( pid,  path);
    respuesta_kernel_de_solicitud_iniciar_proceso();
}

void  respuesta_kernel_de_solicitud_iniciar_proceso(){
    retardo_respuesta();
    t_paquete* un_paquete = crear_paquete_con_buffer(RTA_INICIAR_ESTRUCTURA);
    cargar_int_a_paquete(un_paquete, 1);
    enviar_paquete(un_paquete, fd_kernel);
    eliminar_paquete(un_paquete);
}



//-------------------------------------------------------------------------------------


t_list* obtener_instrucciones_del_archivo(char* path_archivo_instrucciones){
    t_list* instrucciones = list_create();
    instrucciones = procesar_archivo(path_archivo_instrucciones);
    if (!instrucciones) {
        fprintf(stderr, "No se pudo procesar el archivo de instrucciones.\n");
        return NULL;
    }
    return instrucciones;
}

//------------------------------------

char** dividir_cadena(const char* cadena, const char* delimitador) {
    char** resultado = NULL;
    size_t contador = 0;
    char* token = strtok((char*)cadena, delimitador);

    while (token) {
        resultado = realloc(resultado, sizeof(char*) * ++contador);
        if (!resultado) {
            fprintf(stderr, "Error al asignar memoria para dividir_cadena\n");
            exit(EXIT_FAILURE);
        }
        resultado[contador - 1] = strdup(token);
        token = strtok(NULL, delimitador);
    }
    resultado = realloc(resultado, sizeof(char*) * (contador + 1));
    resultado[contador] = NULL;
    return resultado;
}

void free_string_array(char** array) {
    if (!array)
        return;

    for (int i = 0; array[i]; ++i)
        free(array[i]);
    free(array);
}
//-------------------------------------------------------------------------------------

t_list* procesar_archivo(const char* path_archivo){
    FILE* archivo = fopen(path_archivo, "rt");
    if (!archivo) {
        perror("Error al abrir el archivo");
        return NULL;
    }

    t_list* instrucciones = list_create();
    char linea_instruccion[256];
    while (fgets(linea_instruccion, sizeof(linea_instruccion), archivo)) {
        int size_linea_actual = strlen(linea_instruccion);
        if (size_linea_actual > 0 && linea_instruccion[size_linea_actual - 1] == '\n') {
            linea_instruccion[size_linea_actual - 1] = '\0'; // Eliminar el salto de línea
        }

        char** l_instrucciones = dividir_cadena(linea_instruccion, " ");
        if (!l_instrucciones) {
            fprintf(stderr, "Error al dividir la línea de instrucción: %s\n", linea_instruccion);
            continue;
        }

        t_instruccion_codigo* instruccion = malloc(sizeof(t_instruccion_codigo));
        if (!instruccion) {
            perror("Error al asignar memoria para la instrucción");
            free_string_array(l_instrucciones);
            continue;
        }

        instruccion->pseudo_codigo = strdup(l_instrucciones[0]);
        instruccion->primer_parametro = (l_instrucciones[1]) ? strdup(l_instrucciones[1]) : NULL;
        instruccion->segundo_parametro = (l_instrucciones[2]) ? strdup(l_instrucciones[2]) : NULL;

        list_add(instrucciones, instruccion);

        free_string_array(l_instrucciones);
        fclose(archivo); 
  }
  return instrucciones;
}

//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
//FUNCION PRINCIPAL



void encargarse_kernel(int cliente_socket_kernel){

        int numero = 1; 
        
        while(numero){
            t_buffer* unBuffer;
            int codigo_operacion = recibir_operacion(cliente_socket_kernel);

            switch(codigo_operacion){

                case INICIAR_ESTRUCTURA: 
                unBuffer = recibir_buffer(cliente_socket_kernel);
                iniciar_estructura_proceso(unBuffer);
                destruir_buffer(unBuffer);
                break;

                case LIBERAR_ESTRUCTURAS:
                unBuffer = recibir_buffer(cliente_socket_kernel);
                liberar_estructura_proceso(unBuffer);
                destruir_buffer(unBuffer);
                break; 

            }

            free(unBuffer);
        }


    }