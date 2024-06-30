#include <commons/log.h>
#include <commons/config.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <commons/collections/list.h>
#include <commons/string.h>




//-------------------------------------------------------------------------------------

typedef struct{
	char* pseudo_codigo;
    char* primer_parametro;
    char* segundo_parametro;
    
}t_instruccion_codigo;

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
//-------------------------------------------------------------------------------------



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
