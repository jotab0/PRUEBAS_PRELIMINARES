#include <commons/log.h>
#include <commons/config.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include </usr/include/commons/collections/list.h>




//-------------------------------------------------------------------------------------

typedef struct{
	char* pseudo_codigo;
    char* primer_parametro;
    char* segundo_parametro;
}t_instruccion_codigo;

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


t_list* procesar_archivo(char* path_archivo){
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





int main(){

    char* archivo = "/home/utnso/Documents/Pruebas/archivo_instrucciones";
    t_list* lista_instrucciones = obtener_instrucciones_del_archivo(archivo);

    for(int i=0; i<18; i++){
        t_instruccion_codigo* una_instruccion;
        una_instruccion = list_get(lista_instrucciones, i);

        printf("INSTRUCCION <%d> - Pseudo Codigo: <%s> - Primer parametro <%s> - Segundo parametro <%s>\n",i, una_instruccion->pseudo_codigo, una_instruccion->primer_parametro, una_instruccion->segundo_parametro);
    }
}
