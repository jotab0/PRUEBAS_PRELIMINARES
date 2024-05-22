#include "../include/cicloInstruccion.h"


void realizarCicloInstruccion(){

    while(1){

    contexto->proceso_tiempo_ejecutado++;

    // FETCH (solicita y recibe instruccion de memoria)
    // solicitar
    //log obligatorio fetch
    ciclo_de_instruccion_fetch();

    // recibir (lo hace en un case de comunicaciones?)
        //DECODE 
    //ciclo_de_instruccion_decode();

    // DECODE Y EXECUTE
    decodeYExecute(); 

/*
    // AUMENTAR PC
    if(strcmp(instruccion_dividida[0], "JNZ") != 0){
         contexto->PC++;
    }
*/

    if(hayQueDesalojar){ //llamada a IO, desaloja voluntariamente
        break;
    }


    // CHECK INTERRUPTION
    if(hay_interrupcion_quantum){ // bool que le llega de comunicaciones
        t_paquete* unPaquete = crear_paquete_con_buffer(ATENDER_INTERRUPCION);

        motivo_bloqueo = "Interrupcion quantum";
        cargar_string_a_super_paquete(unPaquete, motivo_bloqueo);

        enviarContextoAKernel(unPaquete);
        
        break;                 
    }

/*
    if(hay_interrupcion_IO){
        t_paquete* unPaquete = crear_paquete_con_buffer(ATENDER_INTERRUPCION);

        motivo_bloqueo = "Interrupcion IO";
        cargar_string_a_super_paquete(unPaquete, motivo_bloqueo);

        enviarContextoAKernel(unPaquete);
        
        break;
    }
*/
    }
}

void ciclo_de_instruccion_fetch(){

    log_info(cpu_log_obligatorio, "PID: <%d> - FETCH - Program Counter: <%d>", contexto->proceso_pid, contexto->proceso_pc);
    t_paquete* un_paquete = crear_paquete_con_buffer(SOLICITUD_INSTRUCCION);
    cargar_int_a_paquete(un_paquete, contexto->proceso_pc);
    cargar_int_a_paquete(un_paquete, contexto->proceso_pid);
    enviar_paquete(un_paquete, fd_memoria);
    eliminar_paquete(un_paquete);
}

/*
void ciclo_de_instruccion_decode(){
    if(validador_de_header_instruccion(instruccion_dividida[0])){
        log_info(cpu_logger, "Instruccion validada: [%s] OK", instruccion_dividida[0]); //ej instruccion_elements[0]=MOV, instruccion_elements[1]=AX, instruccion_elements[2]=BX
        sem_post(&sem_control_decode_execute);
    }
}

//verificar si una instrucción recibida es válida o no
void validador_de_header_instruccion(char* header_instruccion){
    log_info(cpu_logger, "Header a evaluar: %s", header_instruccion);
    bool respuesta=false;
    int i=0;
        while(opcode_headers[i] != NULL){
            if(strcmp(opcode_headers[i], header_instruccion) == 0){
                respuesta = true;
            }
            i++;
        }
    return respuesta;
}
*/

void decodeYExecute(){ 
    // strcmp compara cadenas
    if(strcmp(instruccion_dividida[0], "SET") == 0) { //SET(registro, valor)
        log_info(cpu_logger, "PID: <%d>, Ejecutando: <%s> - <%s> <%s>", contexto->proceso_pid, instruccion_dividida[0], instruccion_dividida[1], instruccion_dividida[2]);
        uint32_t* registro = detectar_registro(instruccion_dividida[1]); //registro
        *registro = atoi(instruccion_dividida[2]); //convierte el valor de la instruction_split[2] a un int y lo guarda en el registro
        contexto->proceso_pc++; // aumenta PC

    } else if(strcmp(instruccion_dividida[0], "SUM") == 0){ //SUM(registroDestino, registroOrigen)
        log_info(cpu_logger, "PID: <%d>, Ejecutando: <%s> - <%s> <%s>", contexto->proceso_pid, instruccion_dividida[0], instruccion_dividida[1], instruccion_dividida[2]);
        uint32_t* registro_destino = detectar_registro(instruccion_dividida[1]); // registro destino
        uint32_t* registro_origen = detectar_registro(instruccion_dividida[2]); // registro origen
        *registro_destino = *registro_destino + *registro_origen;
        contexto->proceso_pc++; // aumenta PC

    } else if(strcmp(instruccion_dividida[0], "SUB") == 0){ //SUB(registroDestino, registroOrigen)
        log_info(cpu_logger, "PID: <%d>, Ejecutando: <%s> - <%s> <%s>", contexto->proceso_pid, instruccion_dividida[0], instruccion_dividida[1], instruccion_dividida[2]);
        uint32_t* registro_destino = detectar_registro(instruccion_dividida[1]); // registro destino
        uint32_t* registro_origen = detectar_registro(instruccion_dividida[2]); // registro origen
        *registro_destino = *registro_destino - *registro_origen;
        contexto->proceso_pc++; // aumenta PC

    } else if(strcmp(instruccion_dividida[0], "JNZ") == 0){ //JNZ(registro, instruccion)
        log_info(cpu_logger, "PID: <%d>, Ejecutando: <%s> - <%s> <%s>", contexto->proceso_pid, instruccion_dividida[0], instruccion_dividida[1], instruccion_dividida[2]);
        uint32_t* registro = detectar_registro(instruccion_dividida[1]);
        int valorNuevoPC = atoi(instruccion_dividida[2]);
        if(registro != 0){
            contexto->proceso_pc = valorNuevoPC;
        }

    } else if(strcmp(instruccion_dividida[0], "IO_GEN_SLEEP") == 0){ //IO_GEN_SLEEP(interfaz, unidades de trabajo)
        log_info(cpu_logger, "PID: <%d>, Ejecutando: <%s> - <%s> <%s>", contexto->proceso_pid, instruccion_dividida[0], instruccion_dividida[1], instruccion_dividida[2]);
        
        t_paquete* unPaquete = crear_paquete_con_buffer(ATENDER_INSTRUCCION_CPU);

        cargar_string_a_super_paquete(unPaquete, instruccion_dividida[0]); // instruccion
        cargar_string_a_super_paquete(unPaquete, instruccion_dividida[1]); // interfaz 
        cargar_int_a_super_paquete(unPaquete, atoi(instruccion_dividida[2])); // unidades de tiempo
        
        motivo_bloqueo = "Llamada a IO";
        cargar_string_a_super_paquete(unPaquete, motivo_bloqueo);

        contexto->proceso_pc++; // aumenta PC

        enviarContextoAKernel(unPaquete);

        hayQueDesalojar = true;
    }
}

uint32_t* detectar_registro(char* registro){
    if(strcmp(registro, "AX") == 0){
        return contexto->AX;
    }
    else if(strcmp(registro, "BX") == 0){
        return contexto->BX;
    }
    else if(strcmp(registro, "CX") == 0){
        return contexto->CX;
    }
    else if(strcmp(registro, "DX") == 0){
        return contexto->DX;
    }
}

void enviarContextoAKernel(t_paquete* unPaquete){
    
    cargar_int_a_super_paquete(unPaquete, contexto->proceso_pc);

    cargar_uint32_a_super_paquete(unPaquete, contexto->AX);
    cargar_uint32_a_super_paquete(unPaquete, contexto->BX);
    cargar_uint32_a_super_paquete(unPaquete, contexto->CX);
    cargar_uint32_a_super_paquete(unPaquete, contexto->DX);
    
    cargar_int_a_super_paquete(unPaquete, contexto->proceso_tiempo_ejecutado);
    cargar_int_a_super_paquete(unPaquete, contexto->proceso_ticket);

    int resultado_envio = enviar_paquete_con_buffer(unPaquete, fd_kernel_dispatch); 
    eliminar_paquete(unPaquete);

}





