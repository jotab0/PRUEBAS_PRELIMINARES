#include "../include/cicloInstruccion.h"


void realizarCicloInstruccion(){

    // FETCH (solicita y recibe instruccion de memoria)
    // solicitar
    //log obligatorio fetch

    // recibir (lo hace en un case de comunicaciones?)

    // DECODE Y EXECUTE
    decodeYExecute(); 

    // CHECK INTERRUPTION
    if(hay_interrupcion){ // bool que le llega de comunicaciones
        atenderInterrupcion();
        break;
    }

    // AUMENTAR PC
    if(strcmp(instruccion_dividida[0], "JNZ") != 0){
         PC ++;
    }

}

// esperar a que roy reciba el pcb para ver el pid y como detectar los registros

void decodeYExecute(){ 
    // strcmp compara cadenas
    if(strcmp(instruccion_dividida[0], "SET") == 0) { //SET(registro, valor)
        log_info(cpu_logger, "PID: <%d>, Ejecutando: <%s> - <%s> <%s>", PID, instruccion_dividida[0], instruccion_dividida[1], instruccion_dividida[2]);
        uint32_t* registro = detectar_registro(instruccion_dividida[1]); //registro
        *registro = atoi(instruccion_dividida[2]); //convierte el valor de la instruction_split[2] a un int y lo guarda en el registro

    } else if(strcmp(instruccion_dividida[0], "SUM") == 0){ //SUM(registroDestino, registroOrigen)
        log_info(cpu_logger, "PID: <%d>, Ejecutando: <%s> - <%s> <%s>", PID, instruccion_dividida[0], instruccion_dividida[1], instruccion_dividida[2]);
        uint32_t* registro_destino = detectar_registro(instruccion_dividida[1]); // registro destino
        uint32_t* registro_origen = detectar_registro(instruccion_dividida[2]); // registro origen
        *registro_destino = *registro_destino + *registro_origen;

    } else if(strcmp(instruccion_dividida[0], "SUB") == 0){ //SUB(registroDestino, registroOrigen)
        log_info(cpu_logger, "PID: <%d>, Ejecutando: <%s> - <%s> <%s>", PID, instruccion_dividida[0], instruccion_dividida[1], instruccion_dividida[2]);
        uint32_t* registro_destino = detectar_registro(instruccion_dividida[1]); // registro destino
        uint32_t* registro_origen = detectar_registro(instruccion_dividida[2]); // registro origen
        *registro_destino = *registro_destino - *registro_origen;

    } else if(strcmp(instruccion_dividida[0], "JNZ") == 0){ //JNZ(registro, instruccion)
        log_info(cpu_logger, "PID: <%d>, Ejecutando: <%s> - <%s> <%s>", PID, instruccion_dividida[0], instruccion_dividida[1], instruccion_dividida[2]);
        uint32_t* registro = detectar_registro(instruccion_dividida[1]);
        int valorNuevoPC = atoi(instruccion_dividida[2]);
        if(registro != 0){
            PC = valorNuevoPC;
        }

    } else if(strcmp(instruccion_dividida[0], "IO_GEN_SLEEP") == 0){ //IO_GEN_SLEEP(interfaz, unidades de trabajo)
        log_info(cpu_logger, "PID: <%d>, Ejecutando: <%s> - <%s> <%s>", PID, instruccion_dividida[0], instruccion_dividida[1], instruccion_dividida[2]);
        t_paquete* paquete = crear_paquete_con_buffer(INSTRUCCION_CPU); // nombre?
        cargar_string_a_paquete_con_buffer(paquete, instruccion_dividida[0]); // instruccion
        cargar_string_a_paquete_con_buffer(paquete, instruccion_dividida[1]); // interfaz (no se si se manda como string o de otra forma)
        cargar_int_a_paquete_con_buffer(paquete, atoi(instruccion_dividida[2])); // unidades de tiempo
        int resultado_envio = enviar_paquete(paquete, fd_kernel_dispatch); 
        eliminar_paquete(paquete);
    }
}

uint32_t* detectar_registro(char* registro){
    if(strcmp(registro, "AX") == 0){
        return AX;
    }
    else if(strcmp(registro, "BX") == 0){
        return BX;
    }
    else if(strcmp(registro, "CX") == 0){
        return CX;
    }
    else if(strcmp(registro, "DX") == 0){
        return DX;
    }
    else if(strcmp(registro, "EAX") == 0){
        return EAX;
    }
    else if(strcmp(registro, "EBX") == 0){
        return EBX;
    }
    else if(strcmp(registro, "ECX") == 0){
        return ECX;
    }
}




