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

    // [INICIAR_PROCESO] [PATH]
    // Debe crear pcb en estado NEW y agregarlo a la lista
    // Debe avisar al plp (con sem_lista_new)
    // Instrucciones a ejecutar estarán en el PATH (Deben pasarse a memoria para que las deje disponibles)

}else if(strcmp(comando_consola[0], "FINALIZAR_PROCESO") == 0){

    // [FINALIZAR_PROCESO] [PID]
    // Debe liberar recursos, archivos y memoria

}else if(strcmp(comando_consola[0], "EJECUTAR_SCRIPT") == 0){

    // [EJECUTAR_SCRIPT] [PATH]
    // Debe abrir archivo a través de un path que estará en comando_consola[1]
    // Debe ejecutar 1 por 1 las instrucciones de kernel (Las mismas que están en consola)
    // Ver si encuentro forma de hacerlo en alguna fuente

}else if(strcmp(comando_consola[0], "DETENER_PLANIFICACION") == 0){

    // Debe pausar planificación de corto y largo plazo
    // Debe ignorar mensaje si planificación está activa

}else if(strcmp(comando_consola[0], "INICIAR_PLANIFICACION") == 0){

    // Debe retomar planificación de corto y largo plazo

}else if(strcmp(comando_consola[0], "MULTIPROGRAMACION") == 0){

    // [MULTIPROGRAMACIÓN] [VALOR]
    // Debe cambiar el grado de multiprogramación al VALOR indicado

}else if(strcmp(comando_consola[0], "PROCESO_ESTADO") == 0){

    // Debe listar por estado, todos los procesos en ese estado

}else{
    log_error(kernel_logger, "Comando no reconocido"); // Con la validación no debería llegar acá
    exit(EXIT_FAILURE);
}
    string_array_destroy(comando_consola);
}
