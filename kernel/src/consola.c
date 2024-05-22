#include "../include/consola.h"

void iniciar_consola(){
    char* leido;
    leido = readline("> ");
    bool validacion_leido;

    while(strcmp(leido, "\0") != 0){
        validacion_leido = validacion_de_instruccion_de_consola(leido);
        while(!validacion_leido){
            log_error(kernel_logger, "Comando de consola no reconocido");
            free(leido);
            leido = readline("> ");
            validacion_leido = validacion_de_instruccion_de_consola(leido);
        }
        atender_instruccion(leido);
        free(leido);
        leido=readline("> ");
    }
    free(leido);
}

bool validacion_de_instruccion_de_consola(char* leido){
    bool resultado_validacion = false;

    //FALTA hacer mas controles de validacion [PAG 12 DEL TP, VERIFICAR PARAMETROS Y DEMAS]

    char** comando_consola = string_split(leido, " "); // Vectoriza string

    if(strcmp(comando_consola[0], "EJECUTAR_SCRIPT") == 0){
        resultado_validacion = true;
    }else if(strcmp(comando_consola[0], "INICIAR_PROCESO") == 0){ 
        resultado_validacion = true;
    }else if(strcmp(comando_consola[0], "FINALIZAR_PROCESO") == 0){
        resultado_validacion = true;
    }else if(strcmp(comando_consola[0], "DETENER_PLANIFICACION") == 0){
        resultado_validacion = true;
    }else if(strcmp(comando_consola[0], "INICIAR_PLANIFICACION") == 0){
        resultado_validacion = true;
    }else if(strcmp(comando_consola[0], "MULTIPROGRAMACION") == 0){
        resultado_validacion = true;
    }else if(strcmp(comando_consola[0], "PROCESO_ESTADO") == 0){
        resultado_validacion = true;
    }else{
        log_error(kernel_logger, "Comando no reconocido");
        resultado_validacion = false;
    }
    string_array_destroy(comando_consola);
    return resultado_validacion;
}

void atender_instruccion(char* leido){
    char** comando_consola = string_split(leido, " ");

if(strcmp(comando_consola[0], "INICIAR_PROCESO") == 0){ 

}else if(strcmp(comando_consola[0], "FINALIZAR_PROCESO") == 0){

}else if(strcmp(comando_consola[0], "EJECUTAR_SCRIPT") == 0){

}else if(strcmp(comando_consola[0], "DETENER_PLANIFICACION") == 0){

}else if(strcmp(comando_consola[0], "INICIAR_PLANIFICACION") == 0){

}else if(strcmp(comando_consola[0], "MULTIPROGRAMACION") == 0){

}else if(strcmp(comando_consola[0], "PROCESO_ESTADO") == 0){

}else{
    log_error(kernel_logger, "Comando no reconocido"); // Con la validación no debería llegar acá
    exit(EXIT_FAILURE);
}
    string_array_destroy(comando_consola);
}
