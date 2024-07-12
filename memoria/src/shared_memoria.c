#include "../include/shared_memoria.h"
#include <stdint.h>
#include <inttypes.h>


void retardo_respuesta(){
    usleep(RETARDO_RESPUESTA*1000);
}
//---------------------------------------------------------------------------------------------------------
bool puede_escribir_leer_en_la_direccion(uint32_t direc_fisica, int pid, int tam){
    t_proceso* proceso = obtener_proceso_por_pid(pid);
    int num_marco = direc_fisica / TAM_PAGINA;
    int offset = direc_fisica % TAM_PAGINA;

    bool marco_encontrado = false;
    t_marco* marco;

    for(int i=0; i< list_size(proceso->tabla_paginas); i++){
        t_tabla_de_pagina * entrada = list_get(proceso->tabla_paginas, i);
        if(entrada->num_marco == num_marco){
            marco_encontrado = true;
            break;
        }
    }
    if(!marco_encontrado){
        return false;
    }


    pthread_mutex_lock(&mutex_lista_marcos);
    pthread_mutex_lock(&(proceso->mutex_tabla_paginas));

    for(int j=0; j < list_size(proceso->tabla_paginas); j++){
        t_tabla_de_pagina * entrada = list_get(proceso->tabla_paginas, j);
        if(entrada->num_marco == num_marco){
    
            marco = list_get(lista_marcos,num_marco);
            if(marco->proceso->pid_proceso == proceso->pid_proceso){
                pthread_mutex_unlock(&mutex_lista_marcos);
                pthread_mutex_unlock(&(proceso->mutex_tabla_paginas));
                return true;
            }else{
                pthread_mutex_unlock(&mutex_lista_marcos);
                pthread_mutex_unlock(&(proceso->mutex_tabla_paginas));
                return false;
            }
        }
    
    }

    pthread_mutex_unlock(&mutex_lista_marcos);
    pthread_mutex_unlock(&(proceso->mutex_tabla_paginas));

    int num_pagina_inicial = -1;
    pthread_mutex_lock(&(proceso->mutex_tabla_paginas));

    for(int i=0; i< list_size(proceso->tabla_paginas); i++){
        t_tabla_de_pagina * entrada = list_get(proceso->tabla_paginas, i);
        if(entrada->num_marco== num_marco){
            num_pagina_inicial = entrada->num_pagina;
            break;
        }
    
    }

    if(num_pagina_inicial == -1){
        pthread_mutex_unlock(&(proceso->mutex_tabla_paginas));
        return false;
    }

    int memoria_usada =0;
    for(int i= 0; i < num_pagina_inicial; i++){
        t_tabla_de_pagina * unused_entrada = list_get(proceso->tabla_paginas, i);
        (void)unused_entrada;
        memoria_usada+= TAM_PAGINA;
    }
    memoria_usada +=offset;
    int tam_disponible = (proceso->size) - memoria_usada;


    if(tam > tam_disponible){
        pthread_mutex_unlock(&(proceso->mutex_tabla_paginas));
        return false;
    }


    pthread_mutex_unlock(&(proceso->mutex_tabla_paginas));
    return true;

}

//---------------------------------------------------------------------------------------------------------
// ACCESO A ESPACIO USUARIO 

// Lectura // 

bool direccion_valida(uint32_t direc_fisica){
    uintptr_t inicio = (uintptr_t)espacio_usuario;
    uintptr_t fin = inicio + TAM_MEMORIA;
    return (direc_fisica >= inicio && direc_fisica < fin); 
}





//---------------------------------------------------------------------------------------------------------
//-------------------------------------- NUEVO ------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

char* leer_espacio_usuario(int32_t direccion_fisica, int tamanio){
   char* puntero = espacio_usuario + direccion_fisica;
   char* datos_leidos = malloc(tamanio * sizeof(char*));
   pthread_mutex_lock(&mutex_espacio_usuario);
   memcpy(datos_leidos, puntero, tamanio * sizeof(char*));     
   pthread_mutex_unlock(&mutex_espacio_usuario);
  
   return datos_leidos;
 
}

char* escribir_espacio_usuario(char* datos_para_escribir, int32_t direccion_fisica, int tamanio){
    char* puntero = espacio_usuario + direccion_fisica;
    pthread_mutex_lock(&mutex_espacio_usuario);
    memcpy(puntero,datos_para_escribir, tamanio);    
    pthread_mutex_unlock(&mutex_espacio_usuario);
    
    return"OK";
}



//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

char* leer_en_mas_de_una_pagina(int32_t direc_fisica, int tamanio, int pid) {
    int bytes_por_leer = tamanio;
    int desplazamiento = direc_fisica % TAM_PAGINA;
    char* datos_leidos = malloc(tamanio);
    int bytes_leidos = 0;

	int marco_num = direc_fisica / TAM_PAGINA;
    t_marco* marco_actual = buscar_marco_segun_numero(marco_num);
    char* puntero = espacio_usuario + (marco_actual->base + desplazamiento);

    while (bytes_por_leer > 0) {
        int tam_disponible = TAM_PAGINA - desplazamiento;
        int tam_a_leer = (bytes_por_leer < tam_disponible) ? bytes_por_leer : tam_disponible;
        pthread_mutex_lock(&mutex_espacio_usuario);
        memcpy(datos_leidos + bytes_leidos, puntero, tam_a_leer);
        pthread_mutex_unlock(&mutex_espacio_usuario);

        bytes_leidos += tam_a_leer;
        bytes_por_leer -= tam_a_leer;
		marco_num = marco_num +1;

        if (bytes_por_leer > 0) {
			marco_actual = buscar_marco_segun_numero(marco_num);
            direc_fisica = marco_actual->base;
            desplazamiento = 0;
			puntero = espacio_usuario + direc_fisica;
        }
    }

    return datos_leidos;
}

char* escribir_en_mas_de_una_pagina(char* datos_para_escribir, int32_t direc_fisica, int tamanio, int pid) {
    int bytes_por_escribir = tamanio;
    int desplazamiento = direc_fisica % TAM_PAGINA;
    int bytes_escritos = 0;

	int marco_num = direc_fisica / TAM_PAGINA;
    t_marco* marco_actual = buscar_marco_segun_numero(marco_num);
    char* puntero = espacio_usuario + (marco_actual->base + desplazamiento);

    while (bytes_por_escribir > 0) {
        int tam_disponible = TAM_PAGINA - desplazamiento;
        int tam_a_escribir = (bytes_por_escribir < tam_disponible) ? bytes_por_escribir : tam_disponible;
        pthread_mutex_lock(&mutex_espacio_usuario);
        memcpy(puntero, datos_para_escribir + bytes_escritos, tam_a_escribir);
        pthread_mutex_unlock(&mutex_espacio_usuario);

        bytes_escritos += tam_a_escribir;
        bytes_por_escribir -= tam_a_escribir;
		marco_num++;

        if (bytes_por_escribir > 0) {
            marco_actual = buscar_marco_segun_numero(marco_num);
            direc_fisica = marco_actual->base;
            desplazamiento = 0;
			puntero = espacio_usuario + direc_fisica;
        }
    }

    return "OK";
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

char*  leer_datos_a_partir_de_direc_fisica(int tamanio, int32_t direc_fisica, int pid){
	int desplazamiento = direc_fisica % TAM_PAGINA;
	int tam_disponible = TAM_PAGINA - desplazamiento;

	if(tam_disponible >= tamanio){
		return leer_espacio_usuario(direc_fisica, tamanio);
	}else{
        return leer_en_mas_de_una_pagina(direc_fisica, tamanio ,pid);
		}
}


char*  escribir_datos_a_partir_de_direc_fisica(char* datos_a_escribir, int tamanio, int32_t direc_fisica,int pid){
	int desplazamiento = direc_fisica % TAM_PAGINA;
    int tam_disponible = TAM_PAGINA - desplazamiento;

    if (tam_disponible >= tamanio) {
        return escribir_espacio_usuario(datos_a_escribir, direc_fisica, tamanio);
    } else {
        return escribir_en_mas_de_una_pagina(datos_a_escribir, direc_fisica, tamanio, pid);
    }
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

char* resolver_solicitud_leer_bloque(t_buffer* unBuffer){
	int pid = extraer_int_del_buffer(unBuffer);
	int32_t direc_fisica = extraer_int32_del_buffer(unBuffer);
	int tamanio = extraer_int_del_buffer(unBuffer);

	if(!puede_escribir_leer_en_la_direccion(direc_fisica, pid,tamanio)){
		//log_error(memoria_logger, "ERROR: No tiene permitido leer en la direccion: <%d>", direc_fisica);
		return "ERROR";
	}

	char* datos_leidos = leer_datos_a_partir_de_direc_fisica(tamanio, direc_fisica, pid);
	datos_leidos[tamanio]= '\0';
	log_info(memoria_logger, "PID: <%d> - Accion <LEER> - Direccion Fisica: <%d" PRId32 ">- Tamaño: <%d>", pid, direc_fisica, (tamanio * sizeof(char)));
    return datos_leidos;
}


char* resolver_solicitud_escribir_bloque(t_buffer* unBuffer){
	int pid = extraer_int_del_buffer(unBuffer);
	int32_t direc_fisica = extraer_int32_del_buffer(unBuffer);
	int tamanio = extraer_int_del_buffer(unBuffer);

	char* datos_a_escribir = malloc(tamanio);
	memcpy(datos_a_escribir, unBuffer, tamanio);


	if(!puede_escribir_leer_en_la_direccion(direc_fisica, pid,tamanio)){
		free(datos_a_escribir);
		log_error(memoria_logger, "ERROR: No tiene permitido leer en la direccion: <%d>", direc_fisica);
		return "ERROR";
	}

	char* resultado = escribir_datos_a_partir_de_direc_fisica(datos_a_escribir, tamanio, direc_fisica, pid);
	
	log_info(memoria_logger, "PID: <%d> - Accion <ESCRIBIR> - Direccion Fisica: <%d" PRId32 ">- Tamaño: <%d>", pid, direc_fisica, (tamanio * sizeof(char)));
	free(datos_a_escribir);
	return resultado;
}


//--------------------------------------------------------------------------------------
//----------------------------------- VIEJO --------------------------------------------
//--------------------------------------------------------------------------------------


/*

char* traducir_direccion_fisica(uint32_t direc_fisica, t_proceso* proceso) {
    int nro_marco = direc_fisica / TAM_PAGINA;
    t_marco* marco= buscar_marco_segun_numero(nro_marco);
    int pagina = marco->num_pagina;
    int unused_desplazamiento = direc_fisica % TAM_PAGINA;
    (void)unused_desplazamiento;

    // Busca la entrada de la tabla de páginas correspondiente
    t_tabla_de_pagina* entrada_pagina = NULL;
    if (pagina < list_size(proceso->tabla_paginas)) {

        pthread_mutex_lock(&(proceso->mutex_tabla_paginas));
        entrada_pagina = list_get(proceso->tabla_paginas, pagina);
        pthread_mutex_unlock(&(proceso->mutex_tabla_paginas));
    }

    if (!entrada_pagina) {
        log_info(memoria_logger, "Error: Página no encontrada en la tabla de páginas\n");
        return NULL;
    }

   
    marco = buscar_marco_segun_numero(entrada_pagina->num_marco);

    if (!marco) {
        log_info(memoria_logger, "Error: Marco no encontrado para la página\n");
        return NULL;
    }

    // Calcula la dirección física base del marco
    char* direccion_fisica_base = (char*)espacio_usuario + (marco->nro_marco * TAM_PAGINA);
    return (char*)marco->base;
}





// Estructuras y funciones preexistentes


void escribir_valor_en_espacio_usuario(const char* datos, int tamanio, uint32_t direc_fisica, t_proceso* proceso) {
    int bytes_restantes = tamanio;
    uint32_t offset = direc_fisica;

    while (bytes_restantes > 0) {
        int pagina_actual = offset / TAM_PAGINA;
        int desplazamiento_en_pagina = offset % TAM_PAGINA;
        int espacio_en_pagina = TAM_PAGINA - desplazamiento_en_pagina;

        int bytes_a_escribir = (bytes_restantes > espacio_en_pagina) ? espacio_en_pagina : bytes_restantes;

        int* puntero_a_espacio_usuario = traducir_direccion_fisica(offset, proceso);
        if (!puntero_a_espacio_usuario) {
            log_error(memoria_logger, "Error: Dirección física no válida\n");
            return;
        }
        
        pthread_mutex_lock(&mutex_espacio_usuario);
        memcpy(puntero_a_espacio_usuario, datos, bytes_a_escribir);
        pthread_mutex_unlock(&mutex_espacio_usuario);

        offset += bytes_a_escribir;
        datos += bytes_a_escribir;
        bytes_restantes -= bytes_a_escribir;
    }
}

char* resolver_solicitud_escribir_bloque(t_buffer *unBuffer) {
    int pid = extraer_int_del_buffer(unBuffer);
    uint32_t direc_fisica = extraer_uint32_del_buffer(unBuffer);
    int tamanio = extraer_int_del_buffer(unBuffer);

    char* datos_a_escribir = malloc(tamanio);
    memcpy(datos_a_escribir, unBuffer, tamanio); 
    
    t_proceso* proceso = obtener_proceso_por_pid(pid);
    if (!proceso) {
        free(datos_a_escribir);
        log_error(memoria_logger, "Error: Proceso no encontrado\n");
        return "ERROR";
    }

    if(!puede_escribir_leer_en_la_direccion(direc_fisica, pid, tamanio)){
       free(datos_a_escribir);
       log_error(memoria_logger, "Error: No tiene permitido escribir en la direccion <%d>", direc_fisica);
       return "ERROR";
    }

    escribir_valor_en_espacio_usuario(datos_a_escribir, tamanio, direc_fisica, proceso);

    log_info(memoria_logger, "PID: <%d> - Accion: <ESCRIBIR> - Direccion Fisica: <%d> - Tamaño: <%d>", pid, direc_fisica, tamanio);
    free(datos_a_escribir);
    
    return "OK";
}

char* leer_valor_del_espacio_usuario(int tamanio, uint32_t direc_fisica) {
    int bytes_leidos = 0;  
    char* dato_leido = malloc(tamanio);

    if(!direccion_valida(direc_fisica)){
        return "ERROR";
    }

    while (bytes_leidos < tamanio) {
        int pagina_actual = (direc_fisica + bytes_leidos) / TAM_PAGINA;
        int desplazamiento_en_pagina = (direc_fisica + bytes_leidos) % TAM_PAGINA;
        int espacio_en_pagina = TAM_PAGINA - desplazamiento_en_pagina;

        char* puntero_a_datos = (char*)espacio_usuario + direc_fisica + bytes_leidos;

        int bytes_a_leer = (tamanio - bytes_leidos > espacio_en_pagina) ? espacio_en_pagina : (tamanio - bytes_leidos);
        
        pthread_mutex_lock(&mutex_espacio_usuario);
        memcpy(dato_leido + bytes_leidos, puntero_a_datos, bytes_a_leer);
        pthread_mutex_unlock(&mutex_espacio_usuario);
      
        bytes_leidos += bytes_a_leer;
    }

    return dato_leido;
}

char* resolver_solicitud_leer_bloque(t_buffer *unBuffer) {
    int pid = extraer_int_del_buffer(unBuffer);
    uint32_t direc_fisica = extraer_uint32_del_buffer(unBuffer);
    int tamanio = extraer_int_del_buffer(unBuffer);

    if(!puede_escribir_leer_en_la_direccion(direc_fisica, pid, tamanio)){
       log_error(memoria_logger, "Error: No tiene permitido leer en la direccion <%d>", direc_fisica);
       return "ERROR";
    }

    char* datos_leidos = leer_valor_del_espacio_usuario(tamanio, direc_fisica);
    datos_leidos[tamanio] = '\0';

    log_info(memoria_logger, "PID: <%d> - Accion: <LEER> - Direccion FIsica: <%d> - Tamaño: <%d>", pid, direc_fisica, tamanio);

    return datos_leidos;

}


*/