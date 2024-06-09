#include "../include/espacio_usuario.h"


//-------------------------------------------------------------------------------------------------------
// INICIALIZAR ESPACIO DE MEMORIA -> Paginacion simple 

    //  1) Espacio Usuario = Malloc del tam de la memoria
  ///   2) Tablas -> tengo que crear el diccionario 
////    3) Creo marcos -> TAM_MEMORIA/TAM_PAGINA 

void iniciar_espacio_usuario(){

    espacio_usuario = malloc(TAM_MEMORIA);
    if (espacio_usuario == NULL) {
        log_error(memoria_logger, "Error: Fallo en la asignación de memoria para espacio_usuario");
        exit(1);
    }

    tablas = dictionary_create();
    if (tablas == NULL) {
        log_error(memoria_logger,"Error: Fallo al crear el diccionario para las tablas de páginas");
        exit(1);
    }

    lista_marcos = list_create();
    if (lista_marcos == NULL) {
        log_error(memoria_logger, "Error: Fallo al crear la lista de marcos");
        exit(1);
    }

    cantidad_marcos = TAM_MEMORIA/TAM_PAGINA;
	for(int i=0;i< cantidad_marcos;i++){
		t_marco* un_marco_nuevo  = crear_frame(TAM_PAGINA*i, true, i);

		list_add(lista_marcos,un_marco_nuevo);
	}



    log_info(memoria_logger, "Memoria inicializada con Paginacion Simple.\n");

    // Inicializar Semaforos 

   

    return 0;
}

//-------------------------------------------------------------------------------------------------------
// FUNCIONES NECESARIAS 

t_marco* obtener_ultimo_marco(t_proceso proceso){
    int cantidad_marcos = list_size(proceso->tabla_paginas);
    if(cantidad_marcos ==NULL){
                asignar_marco_disponible_a_proceso_vacio(proceso);
                cantidad_marcos = list_size(proceso->tabla_paginas);
    }
    t_tabla_de_pagina una_fila = list_get(proceso->tabla_paginas,cantidad_marcos);
    t_marco* ultimo_marco = buscar_marco_segun_numero(una_fila->num_marco);
    return ultimo_marco;
}


//---------------------------------------------------------------



void encontrar_todos_los_marcos_del_simulador_y_liberarlos(int cantidad,t_proceso* proceso){
   
    bool _marco_del_proceso(t_marco* un_marco){
		return un_marco->proceso == proceso;
	}

    while(cantidad >0){
        t_marco* marco = list_find(lista_marcos, (void*)_marco_del_proceso);
        poner_en_disponible_frame(marco);
        cantidad--;
    }
    
}


//---------------------------------------------------------------

void inicializar_proceso_simulador(t_proceso* proceso_simulacion){
    proceso_simulacion->pid_proceso = NULL;
    proceso_simulacion->pathInstrucciones = NULL;
    proceso_simulacion->lista_de_instrucciones = list_create();
    proceso_simulacion->tabla_paginas =list_create();
    //inicializar mutex_tabla_paginas;
}

//---------------------------------------------------------------

bool tengo_espacio_suficiente(int tamanio_necesario,t_proceso* proceso){
    t_marco* ultimo_marco = obtener_ultimo_marco(proceso);

    if(ultimo_marco->queda_lugar_disponible){
        int tam_disponible = TAM_PAGINA - (ultimo_marco->cantidad_usado);
        tamanio_necesario -= tam_disponible;
    }

    if(tamanio_necesario > 0){
        int cantidad_marcos_necesarios = cantidad_paginas_necesarias(tamanio_necesario);
        t_proceso* proceso_simulacion;
        int cantidad_a_liberar = 0;
        inicializar_proceso_simulador(proceso_simulacion);

        while(cantidad_marcos_necesarios > 0){
            if(obtener_frame_disponible_simulacion(t_proceso* proceso)){
                cantidad_a_liberar ++;
                cantidad_marcos_necesarios --;
            }else{
                encontrar_todos_los_marcos_del_simulador_y_liberarlos(cantidad_a_liberar,proceso);
                cantidad_marcos_necesarios = 0;
                return false;
            }
        }
        encontrar_todos_los_marcos_del_simulador_y_liberarlos(cantidad_a_liberar, proceso);

    }

   return true;
}


void agrandar_proceso(int tamanio_a_aumentar, t_proceso* proceso){
   
    while(tamanio_a_aumentar >0){
        t_marco* ultimo_marco = obtener_ultimo_marco(proceso);

        if(!(ultimo_marco->queda_lugar_disponible)){
            t_marco* marco_nuevo =  obtener_frame_disponible();
            int num_pagina = (ultimo_marco->num_pagina) +1;
            inicializar_marco_para_un_proceso(marco_nuevo,proceso,num_pagina);
            agregar_marco_pagina_a_tabla(proceso, num_pagina,  marco_nuevo->nro_marco);

        }else{
           int espacio_disponible = TAM_PAGINA - (ultimo_marco->cantidad_usado);

            if((espacio_disponible - tamanio_a_aumentar) =< 0){
                ultimo_marco->cantidad_usado = TAM_PAGINA;
                ultimo_marco->queda_lugar_disponible =false;
                tamanio_a_aumentar = abs(espacio_disponible - tamanio_a_aumentar);
            }else{
                ultimo_marco->cantidad_usado = espacio_disponible - tamanio_a_aumentar;
                ultimo_marco->queda_lugar_disponible =true;
                tamanio_a_aumentar = 0;
            }

        }

        
    }
}

//-----------------------------------------------------------------

int ampliar_tamanio_proceso(int nuevo_tamanio,t_proceso* proceso){
	int tam_inicial = proceso->size;
    int tamanio_a_aumentar = nuevo_tamanio - tam_inicial;
    log_info(memoria_logger,  "PID <%d> - Tamaño Actual: <%d> - Tamaño a Ampliar: <%d>" ,proceso->pid_proceso, tam_inicial,tamanio_a_aumentar);

    if(tengo_espacio_suficiente(tamanio_a_aumentar, proceso)){
        agrandar_proceso(tamanio_a_aumentar, proceso);
        return 1;

    }else{
        log_error(memoria_logger, "ERROR: Out Of Memory");
        return -1;
    }

}


int reducir_tamanio_proceso(int nuevo_tamanio,t_proceso* proceso){
	int tam_inicial = proceso->size;
    int tamanio_a_reducir = tam_inicial - nuevo_tamanio; 

    if(tamanio_a_reducir < 0){
        tamanio_a_reducir = tam_inicial;
    }

    while(tamanio_a_reducir >0){
        int cantidad_marcos = list_size(proceso->tabla_paginas);
        t_tabla_de_pagina una_fila = list_get(proceso->tabla_paginas, cantidad_marcos);
        t_marco* un_marco =  buscar_marco_segun_numero( una_fila->num_marco);
        un_marco->cantidad_usado = (un_marco->cantidad_usado)- tamanio_a_reducir;
    
        if(un_marco->cantidad_usado =< 0){
           proceso_size = (proceso->size) - (tamanio_a_reducir + (un_marco->cantidad_usado))
           tamanio_a_reducir = abs(un_marco->cantidad_usado);
           poner_en_disponible_frame(un_marco);
           list_remove(proceso->tabla_paginas,una_fila);
           free(una_fila);
        }
        else{
            proceso->size = (proceso->size) - tamanio_a_reducir;
            tamanio_a_reducir = 0;
        }
    }
    
	log_info(memoria_logger,  "PID <%d> - Tamaño Actual: <%d> - Tamaño a Reducir: <%d>" ,proceso->pid_proceso, tam_inicial,tamanio_a_reducir);
    
    return 1;
}

//-------------------------------------------------------------------------------------------------------
// FINALIZAR MEMORIA 

void finalizar_memoria(){
    log_destroy(memoria_logger);
	log_destroy(memoria_log_obligatorio);
	config_destroy(memoria_config);
}

void liberar_espacio_usuario(){
    free(espacio_usuario);
}