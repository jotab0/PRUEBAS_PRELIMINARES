#include "../include/encargarse_kernel.h"

void encargarse_kernel(int cliente_socket_kernel){

        int numero = 1; 
        while(numero){
            t_buffer* unBuffer;
            int codigo_operacion = recibir_operacion(cliente_socket_kernel);

            switch(codigo_operacion){

                case INICIAR_ESTRUCTURA: 
                unBuffer = recibir_un_paquete(cliente_socket_kernel);
                iniciar_estructura_proceso(unBuffer);
                break;

            }

        }

    }

//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
//FUNCIONES NECESARIAS

  // 1) saco los datos segun me mando kernel :path -> size -> pid 
 //  2) creo proceso con el formato del struct 
//   3) lo sumo a mi litsa de procesos

t_proceso* iniciar_estructura_proceso(t_buffer* unBuffer){
    char* path  = extraer_string_del_buffer(unBuffer);
    int tamanio = extraer_int_del_buffer(unBuffer);
    int pid     = extraer_int_del_buffer(unBuffer);
        
    t_proceso* nuevo_proceso = malloc(sizeof(t_proceso));
    nuevo_proceso->pid_proceso = pid;
    nuevo_proceso->size = tamanio;
    nuevo_proceso->pathInstrucciones = path;
    nuevo_proceso->lista_de_instrucciones = obtener_instrucciones_del_archivo(nuevo_proceso->pathInstrucciones);

        
    list_add(lista_procesos, nuevo_proceso);
    respuesta_kernel_de_solicitud_iniciar_proceso();

    return nuevo_proceso;
}

void  respuesta_kernel_de_solicitud_iniciar_proceso(){
    t_paquete* un_paquete = crear_paquete_con_buffer(RTA_INICIAR_ESTRUCTURA);
    cargar_string_a_paquete(un_paquete, "Correcto");
    enviar_paquete(un_paquete, fd_kernel);
    eliminar_paquete(un_paquete);
}


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


