#include "../include/cicloInstruccion.h"

void iniciar_tiempo(){
    gettimeofday(&inicio, NULL);
}

void calcularTiempoEjecutado(){
    gettimeofday(&fin, NULL);
    long segundos = fin.tv_sec - inicio.tv_sec;
    long microsegundos = fin.tv_usec - inicio.tv_usec;
    double tiempo_transcurrido = segundos * 1000000 + microsegundos;
    contexto->proceso_tiempo_ejecutado += tiempo_transcurrido;
}

void realizarCicloInstruccion(){

    while(1){

    iniciar_tiempo();

    hayQueDesalojar = false;

    //contexto->proceso_tiempo_ejecutado++;

    // FETCH (solicita y recibe instruccion de memoria)
    // solicitar
    //log obligatorio fetch
    ciclo_de_instruccion_fetch();

    // recibir (lo hace en un case de comunicaciones?)
        //DECODE 
    //ciclo_de_instruccion_decode();

    // semaforo wait para esperar que llegue la instruccion

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

    // mutex
    // CHECK INTERRUPTION

    if(hay_interrupcion){
        t_paquete* unPaquete = crear_paquete_con_buffer(ATENDER_INTERRUPCION);

        cargar_int_a_paquete(unPaquete, motivo_interrupcion);

        enviarContextoAKernel(unPaquete);
        
        break;
    }

    /*
    if(hay_interrupcion_consola){ // bool que le llega de comunicaciones
        t_paquete* unPaquete = crear_paquete_con_buffer(ATENDER_INTERRUPCION);

        motivo_bloqueo = "Consola";
        cargar_string_a_paquete(unPaquete, motivo_bloqueo);

        enviarContextoAKernel(unPaquete);
        
        break;                 
    } else if(hay_interrupcion_quantum){
        t_paquete* unPaquete = crear_paquete_con_buffer(ATENDER_INTERRUPCION);

        motivo_bloqueo = "Quantum";
        cargar_string_a_paquete(unPaquete, motivo_bloqueo);

        enviarContextoAKernel(unPaquete);
        
        break;
    } else if(hay_interrupcion_exit){
        t_paquete* unPaquete = crear_paquete_con_buffer(ATENDER_INTERRUPCION);

        motivo_bloqueo = "Exit";
        cargar_string_a_paquete(unPaquete, motivo_bloqueo);

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
    //mutex por cada instruccion dividida?
    if(strcmp(instruccion_dividida[0], "SET") == 0) { //SET(registro, valor)
        log_info(cpu_logger, "PID: <%d>, Ejecutando: <%s> - <%s> <%s>", contexto->proceso_pid, instruccion_dividida[0], instruccion_dividida[1], instruccion_dividida[2]);
       
        contexto->proceso_pc++; // aumenta PC
       
        uint32_t* registro = detectar_registro(instruccion_dividida[1]); //registro
        *registro = atoi(instruccion_dividida[2]); //convierte el valor de la instruction_split[2] a un int y lo guarda en el registro

    } else if(strcmp(instruccion_dividida[0], "SUM") == 0){ //SUM(registroDestino, registroOrigen)
        log_info(cpu_logger, "PID: <%d>, Ejecutando: <%s> - <%s> <%s>", contexto->proceso_pid, instruccion_dividida[0], instruccion_dividida[1], instruccion_dividida[2]);
        
        contexto->proceso_pc++; // aumenta PC
        
        uint32_t* registro_destino = detectar_registro(instruccion_dividida[1]); // registro destino
        uint32_t* registro_origen = detectar_registro(instruccion_dividida[2]); // registro origen
        
        *registro_destino = *registro_destino + *registro_origen;

    } else if(strcmp(instruccion_dividida[0], "SUB") == 0){ //SUB(registroDestino, registroOrigen)
        log_info(cpu_logger, "PID: <%d>, Ejecutando: <%s> - <%s> <%s>", contexto->proceso_pid, instruccion_dividida[0], instruccion_dividida[1], instruccion_dividida[2]);
        
        contexto->proceso_pc++; // aumenta PC
        
        uint32_t* registro_destino = detectar_registro(instruccion_dividida[1]); // registro destino
        uint32_t* registro_origen = detectar_registro(instruccion_dividida[2]); // registro origen
        
        *registro_destino = *registro_destino - *registro_origen;
        
    } else if(strcmp(instruccion_dividida[0], "JNZ") == 0){ //JNZ(registro, instruccion)
        log_info(cpu_logger, "PID: <%d>, Ejecutando: <%s> - <%s> <%s>", contexto->proceso_pid, instruccion_dividida[0], instruccion_dividida[1], instruccion_dividida[2]);
        
        uint32_t* registro = detectar_registro(instruccion_dividida[1]);
        int valorNuevoPC = atoi(instruccion_dividida[2]);
        
        if(*registro != 0){
            contexto->proceso_pc = valorNuevoPC;
        }

    } else if(strcmp(instruccion_dividida[0], "IO_GEN_SLEEP") == 0){ //IO_GEN_SLEEP(interfaz, unidades de trabajo)
        log_info(cpu_logger, "PID: <%d>, Ejecutando: <%s> - <%s> <%s>", contexto->proceso_pid, instruccion_dividida[0], instruccion_dividida[1], instruccion_dividida[2]);
        
        contexto->proceso_pc++; // aumenta PC

        int instruccion_interfaz = 0;

        t_paquete* unPaquete = crear_paquete_con_buffer(ATENDER_INSTRUCCION_CPU);

        cargar_int_a_paquete(unPaquete, instruccion_interfaz); // instruccion
        cargar_string_a_paquete(unPaquete, instruccion_dividida[1]); // interfaz 
        cargar_int_a_paquete(unPaquete, atoi(instruccion_dividida[2])); // unidades de tiempo

        //motivo_bloqueo = "IO";
        //cargar_string_a_paquete(unPaquete, motivo_bloqueo);

        enviarContextoAKernel(unPaquete);

        hayQueDesalojar = true;

    } else if(strcmp(instruccion_dividida[0], "MOV_IN") == 0){ //MOV_IN(registro datos, registro direccion)
        log_info(cpu_logger, "PID: <%d>, Ejecutando: <%s> - <%s> <%s>", contexto->proceso_pid, instruccion_dividida[0], instruccion_dividida[1], instruccion_dividida[2]);

        contexto->proceso_pc++; // aumenta PC        

        uint32_t* registro_datos = detectar_registro(instruccion_dividida[1]); //registro datos, aca tengo que guardar el valor leido
        uint32_t* registro_direccion = detectar_registro(instruccion_dividida[2]); // registro direccion que contiene la direccion logica

        int direccion_logica = (int)*registro_direccion;

        char* valor = leer_valor_memoria(direccion_logica, sizeof(*registro_datos));

        if(strcmp(valor, "ERROR") != 0){ // si no hubo PF
            *registro_datos = (uint32_t)(uintptr_t)valor;
        }

    } else if (strcmp(instruccion_dividida[0], "MOV_OUT") == 0){ // MOV_OUT(registro direccion, registro datos)
        log_info(cpu_logger, "PID: <%d>, Ejecutando: <%s> - <%s> <%s>", contexto->proceso_pid, instruccion_dividida[0], instruccion_dividida[1], instruccion_dividida[2]);

        contexto->proceso_pc++; // aumenta PC  
        
        uint32_t* registro_direccion = detectar_registro(instruccion_dividida[1]); // registro direccion (direccion de memoria en la que hay que escribir el valor)
        uint32_t* registro_datos = detectar_registro(instruccion_dividida[2]); // registro datos (valor a escribir)

        int direccion_logica = (int)*registro_direccion; // ?? antes lo tenia asi: direccion_logica = atpi(instruccion_dividida[2]) pero dice que es un registro que contiene la direccion
        char* valor_a_escribir = (char*)(uintptr_t)registro_datos;

        escribir_valor_memoria(direccion_logica, valor_a_escribir, strlen(valor_a_escribir)); // se fija aca adentro si hubo PF

    } else if (strcmp(instruccion_dividida[0], "RESIZE") == 0){ // RESIZE(tamaño)
        log_info(cpu_logger, "PID: <%d>, Ejecutando: <%s> - <%s>", contexto->proceso_pid, instruccion_dividida[0], instruccion_dividida[1]);

        contexto->proceso_pc++; // aumenta PC

        int tamanio = atoi(instruccion_dividida[1]); 

        t_paquete* unPaquete = crear_paquete_con_buffer(AJUSTAR_TAMANIO);
        cargar_int_a_paquete(unPaquete, contexto->proceso_pid); //para que memoria sepa el proceso
        cargar_int_a_paquete(unPaquete, tamanio);

        enviar_paquete(unPaquete, fd_memoria); 

        // semaforo wait resultado

        if(resultado == -1){ // si el resize dio out of memory
            t_paquete* unPaquete = crear_paquete_con_buffer(OUT_OF_MEMORY);

            //motivo_bloqueo = "Out of Memory";
            //cargar_string_a_paquete(unPaquete, motivo_bloqueo);

            enviarContextoAKernel(unPaquete);

            hayQueDesalojar = true;
        }

        eliminar_paquete(unPaquete);

    } else if (strcmp(instruccion_dividida[0], "COPY_STRING") == 0){ // COPY_STRING(tamaño)
        log_info(cpu_logger, "PID: <%d>, Ejecutando: <%s> - <%s>", contexto->proceso_pid, instruccion_dividida[0], instruccion_dividida[1]);

        contexto->proceso_pc++; // aumenta PC

        int tamanio = atoi(instruccion_dividida[1]);

        int direccion_logica_SI = (int)contexto->SI; // el registro contiene la direccion logica del string que quiero copiar
        char* valor = leer_valor_memoria(direccion_logica_SI, tamanio); // lee la direccion (tendria que ser un string pero leer devuelve un int como resuelvo?)
        
        if(strcmp(valor, "ERROR") != 0){
            char* resultado;
            for(int i; i < tamanio && valor[i] != '\0'; i++){
                resultado[i] = valor[i];
            }
            resultado[tamanio] = '\0';
            int direccion_logica_DI = (int)contexto->DI; // registro en el que quiero escribir
            escribir_valor_memoria(direccion_logica_DI, resultado, tamanio); // escribe en la direccion de DI el valor de la direccion SI
        }

    } else if (strcmp(instruccion_dividida[0], "IO_STDIN_READ") == 0){ // IO_STDIN_READ(interfaz, registro direccion, registro tamanio )
        log_info(cpu_logger, "PID: <%d>, Ejecutando: <%s> - <%s> <%s> <%s>", contexto->proceso_pid, instruccion_dividida[0], instruccion_dividida[1], instruccion_dividida[2], instruccion_dividida[3]);

        contexto->proceso_pc++; // aumenta PC

        int instruccion_interfaz = 1;

        uint32_t* registro_direccion = detectar_registro(instruccion_dividida[2]);
        int direccion_logica = (int)*registro_direccion;
        uint32_t* registro_tamanio = detectar_registro(instruccion_dividida[3]);
        int tamanio = (int)*registro_tamanio;

        int direccion_fisica = traducir(direccion_logica); 

        if(direccion_fisica != -1){

            t_paquete* unPaquete = crear_paquete_con_buffer(ATENDER_INSTRUCCION_CPU);

            cargar_int_a_paquete(unPaquete, instruccion_interfaz); // instruccion
            cargar_string_a_paquete(unPaquete, instruccion_dividida[1]); // interfaz 
            cargar_int_a_paquete(unPaquete, direccion_fisica); // direccion
            cargar_int_a_paquete(unPaquete, tamanio); // tamanio

            //motivo_bloqueo = "IO";
            //cargar_string_a_paquete(unPaquete, motivo_bloqueo);

            enviarContextoAKernel(unPaquete);

            hayQueDesalojar = true;
        }

    } else if (strcmp(instruccion_dividida[0], "IO_STDOUT_WRITE") == 0){ // IO_STDOUT_WRITE(interfaz, registro direccion, registro tamanio )
        log_info(cpu_logger, "PID: <%d>, Ejecutando: <%s> - <%s> <%s> <%s>", contexto->proceso_pid, instruccion_dividida[0], instruccion_dividida[1], instruccion_dividida[2], instruccion_dividida[3]);

        contexto->proceso_pc++; // aumenta PC

         int instruccion_interfaz = 2;

        uint32_t* registro_direccion = detectar_registro(instruccion_dividida[2]);
        int direccion_logica = (int)*registro_direccion;
        uint32_t* registro_tamanio = detectar_registro(instruccion_dividida[3]);
        int tamanio = (int)*registro_tamanio;

        int direccion_fisica = traducir(direccion_logica); 

        if(direccion_fisica != -1){

            t_paquete* unPaquete = crear_paquete_con_buffer(ATENDER_INSTRUCCION_CPU);

            cargar_int_a_paquete(unPaquete, instruccion_interfaz); // instruccion
            cargar_string_a_paquete(unPaquete, instruccion_dividida[1]); // interfaz 
            cargar_int_a_paquete(unPaquete, direccion_fisica); // direccion
            cargar_int_a_paquete(unPaquete, tamanio); // tamanio

            //motivo_bloqueo = "IO";
            //cargar_string_a_paquete(unPaquete, motivo_bloqueo);

            enviarContextoAKernel(unPaquete);

            hayQueDesalojar = true;
        }

    } else if(strcmp(instruccion_dividida[0], "WAIT") == 0){ //WAIT(recurso)
        log_info(cpu_logger, "PID: <%d>, Ejecutando: <%s> - <%s>", contexto->proceso_pid, instruccion_dividida[0], instruccion_dividida[1]);
        
        contexto->proceso_pc++; // aumenta PC

        t_paquete* unPaquete = crear_paquete_con_buffer(WAIT_KCPU);

        cargar_string_a_paquete(unPaquete, instruccion_dividida[1]); // recurso 

        //motivo_bloqueo = "Wait";
        //cargar_string_a_paquete(unPaquete, motivo_bloqueo);

        enviarContextoAKernel(unPaquete);

        hayQueDesalojar = true;

    } else if(strcmp(instruccion_dividida[0], "SIGNAL") == 0){ //SIGNAL(recurso)
        log_info(cpu_logger, "PID: <%d>, Ejecutando: <%s> - <%s>", contexto->proceso_pid, instruccion_dividida[0], instruccion_dividida[1]);
        
        contexto->proceso_pc++; // aumenta PC

        t_paquete* unPaquete = crear_paquete_con_buffer(SIGNAL_KCPU);

        cargar_string_a_paquete(unPaquete, instruccion_dividida[1]); // recurso 

        //motivo_bloqueo = "Signal";
        //cargar_string_a_paquete(unPaquete, motivo_bloqueo);

        enviarContextoAKernel(unPaquete);

        //hayQueDesalojar = false; no hay que desalojar, no cambia

    }
    else if(strcmp(instruccion_dividida[0], "EXIT") == 0){
        log_info(cpu_logger, "PID: <%d>, Ejecutando: <%s>", contexto->proceso_pid, instruccion_dividida[0]);

        t_paquete* unPaquete = crear_paquete_con_buffer(FINALIZAR_PROCESO);

        cargar_string_a_paquete(unPaquete, instruccion_dividida[0]);

        enviarContextoAKernel(unPaquete);

        hayQueDesalojar = true;
    }
}

uint32_t* detectar_registro(char* registro){
    if(strcmp(registro, "AX") == 0){
        return &(contexto->AX);
    }
    else if(strcmp(registro, "BX") == 0){
        return &(contexto->BX);
    }
    else if(strcmp(registro, "CX") == 0){
        return &(contexto->CX);
    }
    else if(strcmp(registro, "DX") == 0){
        return &(contexto->DX);
    }
    return NULL;
}

void enviarContextoAKernel(t_paquete* unPaquete){

    cargar_int_a_paquete(unPaquete, contexto->proceso_pid);    
    cargar_int_a_paquete(unPaquete, contexto->proceso_pc);

    cargar_uint32_a_paquete(unPaquete, contexto->AX);  // si es puntero a uint32_t:(uint32_t)(uintptr_t) 
    cargar_uint32_a_paquete(unPaquete, contexto->BX);
    cargar_uint32_a_paquete(unPaquete, contexto->CX);
    cargar_uint32_a_paquete(unPaquete, contexto->DX);
    
    cargar_int_a_paquete(unPaquete, contexto->proceso_ticket);

    calcularTiempoEjecutado();

    cargar_int_a_paquete(unPaquete, contexto->proceso_tiempo_ejecutado);

    enviar_paquete(unPaquete, fd_kernel_dispatch); 
    eliminar_paquete(unPaquete);

    //mutex
    hay_interrupcion = false;
    hayQueDesalojar = false;
    /*
    hay_interrupcion_consola = false;
    hay_interrupcion_quantum = false;
    hay_interrupcion_exit = false;
    */

}

char* leer_valor_memoria(int direccion_logica, int tamanio){
    int direccion_fisica = traducir(direccion_logica);

    if(direccion_fisica == -1){
       
        return "ERROR";

    } else {
        t_paquete* unPaquete = crear_paquete_con_buffer(SOLICITUD_LECTURA_MEMORIA_BLOQUE);
        cargar_int_a_paquete(unPaquete, contexto->proceso_pid);
        cargar_int_a_paquete(unPaquete, direccion_fisica);
        cargar_int_a_paquete(unPaquete, tamanio);
        enviar_paquete(unPaquete, fd_memoria);
        eliminar_paquete(unPaquete);

        // semaforo wait(respuesta_memoria)

        log_info(cpu_logger, "PID: <%d> - Accion: LEER - Direccion: <%d> - Valor: <%d>", contexto->proceso_pid, direccion_fisica, respuesta_marco_lectura);
        
        char* valor = respuesta_marco_lectura;
        return valor;

    }

    // TODO 
    // se fija si esta en la TLB(de donde saco la TLB?, la tengo que manejar yo?), si esta devuelvo la direccion fisica
    // si no esta llama a MMU que hace el calculo de la traduccion y la busca en memoria
    // si no la encuentra en memoria hay PF (-1)
    // le pide a memoria leer el contenido del marco
}

void escribir_valor_memoria(int direccion_logica, char* valor, int tamanio){
    int direccion_fisica = traducir(direccion_logica);

    int tamanio_total = tamanio * sizeof(char);

    if(direccion_fisica != -1){

        t_paquete* unPaquete = crear_paquete_con_buffer(SOLICITUD_ESCRITURA_MEMORIA_BLOQUE);
        cargar_int_a_paquete(unPaquete, contexto->proceso_pid);
        cargar_int_a_paquete(unPaquete, direccion_fisica);
        cargar_string_a_paquete(unPaquete, valor); 
        cargar_int_a_paquete(unPaquete, tamanio_total);
        enviar_paquete(unPaquete, fd_memoria);
        eliminar_paquete(unPaquete);

        // semaforo wait(respuesta_peticion_escritura)

        log_info(cpu_logger, "PID: <%d> - Accion: ESCRIBIR - Direccion: <%d> - Valor: <%d>", contexto->proceso_pid, direccion_fisica, respuesta_marco_escritura);
    }

}

int traducir(int direccion_logica){

    // semaforo wait envio_info de memoria
    int numero_pagina = floor(direccion_logica/tamanio_pagina);
    int direccion_fisica;
    int marco_TLB = buscar_en_TLB(numero_pagina);

    if(marco_TLB == -1){ // No la encuentra en la tlb
        log_info(cpu_logger, "PID: <%d> - TLB MISS - Pagina: <%d>", contexto->proceso_pid, numero_pagina);

        direccion_fisica = mmu(direccion_logica, numero_pagina, tamanio_pagina);

        return direccion_fisica;

    } else { // La encuentra en la tlb
        log_info(cpu_logger, "PID: <%d> - TLB HIT - Pagina: <%d>", contexto->proceso_pid, numero_pagina);

        int desplazamiento = direccion_logica - numero_pagina * tamanio_pagina;
        direccion_fisica = marco_TLB * tamanio_pagina + desplazamiento;

        return direccion_fisica;
    }
}

int mmu(int direccion_logica, int numero_pagina, int tamanio_pagina){
     
    // obtener tamanio de pagina de memoria?
    int desplazamiento = direccion_logica - numero_pagina * tamanio_pagina;

    // le pido la pag a memoria, si la tiene devuelve el marco y sino PF (-1)
    t_paquete* unPaquete = crear_paquete_con_buffer(SOLICITUD_PAGINA);
    cargar_int_a_paquete(unPaquete, contexto->proceso_pid);
    cargar_int_a_paquete(unPaquete, numero_pagina);
    enviar_paquete(unPaquete, fd_memoria);
    eliminar_paquete(unPaquete);

    // semaforo wait(peticion_marco)

    if(marco != -1){ // no hay PF
        log_info(cpu_logger, "PID: <%d> - OBTENER MARCO - Pagina: <%d> - Marco: <%d>", contexto->proceso_pid, numero_pagina, marco);

        int direccion_fisica = marco * tamanio_pagina + desplazamiento;
        
        agregar_entrada_tlb(numero_pagina, marco);

        return direccion_fisica;

    } else { // hay PF
        log_info(cpu_logger, "PID: <%d> - PAGE FAULT - Pagina: <%d>", contexto->proceso_pid, numero_pagina);

        t_paquete* unPaquete = crear_paquete_con_buffer(PAGE_FAULT);

           // motivo_bloqueo = "Page Fault";
            //cargar_string_a_paquete(unPaquete, motivo_bloqueo);

        enviarContextoAKernel(unPaquete);

        hayQueDesalojar = true;

        return -1;
    }
}

/*
int buscar_en_TLB(int numero_pagina){
    // fijarse si el numero de pagina para ese proceso esta 
    // si no esta devuelve -1 
    //int resultado_direccion = buscar(numero_pagina);
    t_link_element* entrada_encontrada = list_find(tlb, &condicion_busqueda(numero_pagina));

    if(entrada_encontrada != NULL){
        // si esta (la encontro) devuelve la direccion fisica
        return entrada_encontrada->direccion_fisica;
    } else {
        // si no esta devuelve -1 y la reemplaza
        agregar_a_la_TLB(numero_pagina);
        return -1;
    }
}

bool condicion_busqueda(int numero_pagina, void *entrada){
    t_link_element *elemento = (t_link_elemento *)entrada;
    if(elemento->pid == contexto->proceso_pid && elemento->nro_pag == numero_pagina){
        return true;
    } else {
        return false;
    }
}
*/

int buscar_en_TLB(int numero_pagina){
    int marco = -1;

    //mutex tlb
    for(int i = 0; i < tlb->tamanio; i++){
        if(tlb->entradas[i].pid == contexto->proceso_pid && tlb->entradas[i].pagina == numero_pagina && tlb->entradas[i].estado == OCUPADA){
            marco = tlb->entradas[i].marco;
            temporal_destroy(tlb->entradas[i].ultimo_uso);
            tlb->entradas[i].ultimo_uso = temporal_create();
        }
    }
    //mutex tlb

    return marco;

}


void agregar_entrada_TLB(int numero_pagina, int marco){
    int hay_libre = 0; // si hay entrada libre (no ocupada)
    
    // mutex tlb
    for(int i = 0; i < tlb->tamanio && hay_libre == 0; i++){
        if(tlb->entradas[i].estado == LIBRE) {
            tlb->entradas[i].pid = contexto->proceso_pid;
            tlb->entradas[i].pagina = numero_pagina;
            tlb->entradas[i].marco = marco;
            tlb->entradas[i].estado = OCUPADA;

            tlb->entradas[i].orden_carga = ordenCargaGlobal;
            ordenCargaGlobal++;
            temporal_destroy(tlb->entradas[i].ultimo_uso);
            tlb->entradas[i].ultimo_uso = temporal_create();
        

            hay_libre = 1;

            break;
        }
    }

    if(hay_libre == 0){ //no encontro ninguna entrada libre, hay que reemplazar alguna segun el algoritmo
      
        if(algoritmo_tlb == 2){ // FIFO
            int menorIndice = 0;
           for(int i = 1; i < tlb->tamanio; i++){ // busco la entrada con el menor orden de carga (la que esta hace mas tiempo) y la reemplazo por la nueva
                if(tlb->entradas[i].orden_carga < tlb->entradas[menorIndice].orden_carga){
                    menorIndice = i;
                }
           }
            tlb->entradas[menorIndice].pid = contexto->proceso_pid;
            tlb->entradas[menorIndice].pagina = numero_pagina;
            tlb->entradas[menorIndice].marco = marco;
            tlb->entradas[menorIndice].estado = OCUPADA;

            tlb->entradas[menorIndice].orden_carga = ordenCargaGlobal;
            ordenCargaGlobal++;
            temporal_destroy(tlb->entradas[menorIndice].ultimo_uso);
            tlb->entradas[menorIndice].ultimo_uso = temporal_create();
           


        }else if (algoritmo_tlb == 1){ //LRU
            int mayorIndice = 0;
           for(int i = 1; i < tlb->tamanio; i++){ // busco la entrada con el mayor ultimo_uso (el mas antiguo) y la reemplazo por la nueva
                if(temporal_gettime(tlb->entradas[i].ultimo_uso) > temporal_gettime(tlb->entradas[mayorIndice].ultimo_uso)){
                    mayorIndice = i;
                }
           }
            tlb->entradas[mayorIndice].pid = contexto->proceso_pid;
            tlb->entradas[mayorIndice].pagina = numero_pagina;
            tlb->entradas[mayorIndice].marco = marco;
            tlb->entradas[mayorIndice].estado = OCUPADA;

            tlb->entradas[mayorIndice].orden_carga = ordenCargaGlobal;
            ordenCargaGlobal++;
            temporal_destroy(tlb->entradas[mayorIndice].ultimo_uso);
            tlb->entradas[mayorIndice].ultimo_uso = temporal_create();

        }else{
            log_error(cpu_logger, "Algoritmo de reemplazo no valido");
            exit(EXIT_FAILURE);
        }
    }
}

/*
void quitar_entrada(int numero_pagina){
    //mutex tlb
    for(int i = 0; i < tlb->tamanio; i++){
        if(tlb->entradas[i].pid == contexto->proceso_pid && tlb->entradas[i].pagina == numero_pagina){
            tlb->entradas[i].estado = LIBRE;
            return;
        }
    }
    //mutex tlb
}

void limpiar_tlb(){
    // mutex tlb
    for(int i = 0; i < tlb-> tamanio; i++){
        tlb->entradas[i].estado = LIBRE;
    }
    // mutex tlb
}

void liberar_entradas_de_un_proceso(){
    //mutex tlb
    for (int i = 0; i < tlb->tamanio; i++){
        if(tlb->entradas[i].pid == contexto->proceso_pid){
            tlb->entradas[i].estado = LIBRE;
        }
    }
    //mutex tlb
}
*/


