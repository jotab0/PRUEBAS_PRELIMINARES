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

t_list* procesar_archivo(const char* path_archivo) {
    FILE* archivo = fopen(path_archivo, "rt");
    t_list* instrucciones = list_create();
    char* instruccion_definitiva = NULL;
    int i = 0;
    if (archivo == NULL) {
        perror("No se encontró el archivo");
        return instrucciones;
    }

    char* linea_instruccion = malloc(256 * sizeof(int));
    while (fgets(linea_instruccion, 256, archivo)) {
    
    	int size_linea_actual = strlen(linea_instruccion);
    	if(size_linea_actual > 2){
    		if(linea_instruccion[size_linea_actual - 1] == '\n'){
				char* linea_limpia = string_new();
				string_n_append(&linea_limpia, linea_instruccion, size_linea_actual - 1);
				free(linea_instruccion);
				linea_instruccion = malloc(256 * sizeof(int));
				strcpy(linea_instruccion,linea_limpia);
    		}
    	}
    	

        char** l_instrucciones = string_split(linea_instruccion, " ");
        log_info(memoria_logger, "Intruccion: [%s]", linea_instruccion);
        
        while (l_instrucciones[i]) {
        	i++;
        }
        t_instruccion_codigo* pseudo_cod = malloc(sizeof(t_instruccion_codigo));
        pseudo_cod->pseudo_codigo = strdup(l_instrucciones[0]);
        pseudo_cod->primer_parametro = (i > 1) ? strdup(l_instrucciones[1]) : NULL;
        pseudo_cod->segundo_parametro = (i > 2) ? strdup(l_instrucciones[2]) : NULL;

        if (i == 3) {
        	instruccion_definitiva = string_from_format("%s %s %s", pseudo_cod->pseudo_codigo, pseudo_cod->primer_parametro, pseudo_cod->segundo_parametro);
        } else if (i == 2) {
            instruccion_definitiva = string_from_format("%s %s", pseudo_cod->pseudo_codigo, pseudo_cod->primer_parametro);
        } else {
            instruccion_definitiva = strdup(pseudo_cod->pseudo_codigo);
        }

        list_add(instrucciones, instruccion_definitiva);
        for (int j = 0; j < i; j++) {
            free(l_instrucciones[j]);
        }
        free(l_instrucciones);
        free(pseudo_cod->pseudo_codigo);

		if(pseudo_cod->primer_parametro) free(pseudo_cod->primer_parametro);
		if(pseudo_cod->segundo_parametro) free(pseudo_cod->segundo_parametro);
		free(pseudo_cod);
        i = 0; 
    }

    fclose(archivo);
    free(linea_instruccion);
    return instrucciones;
}


//------------------------------------------------------------------------------------

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


t_list* obtener_instrucciones_del_archivo(char* path_archivo_instrucciones){
    t_list* instrucciones = list_create();
    instrucciones = procesar_archivo(path_archivo_instrucciones);
    if (!instrucciones) {
        fprintf(stderr, "No se pudo procesar el archivo de instrucciones.\n");
        return NULL;
    }
    return instrucciones;
}

//-------------------------------------------------------------------------------------



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