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

    int cantidad_marcos = TAM_MEMORIA/TAM_PAGINA;
	for(int i=0;i< cantidad_marcos;i++){
		t_marco* un_marco_nuevo  = crear_frame(TAM_PAGINA*i, true, i);

		list_add(lista_marcos,un_marco_nuevo);
	}



    log_info(memoria_logger, "Memoria inicializada con Paginacion Simple.\n");

    // Inicializar Semaforos 

    pthread_mutex_init(&mutex_lista_marcos, NULL);
    pthread_mutex_init(&mutex_espacio_usuario, NULL);
    pthread_mutex_init(&mutex_carga_global, NULL);

    sem_init(&sem_swap, 0, 0);
    sem_init(&sem_pageFault, 0, 0);


    return 0;
}

//-------------------------------------------------------------------------------------------------------
// FUNCIONES NECESARIAS 
// FRAMES

t_marco* crear_frame(int tamBase, bool disponible, int i){
    t_marco *marco = malloc(sizeof(t_marco));
	marco->nro_marco = i;
	marco->base = tamBase;
	marco->disponible = disponible;
	marco->info_nuevo = NULL;
	marco->info_anterior = NULL;
	marco->orden_carga = 0;
	marco->ultimo_uso = NULL;

	return marco;
}



//-------------------------------------------------------------------------------------------------------
// FINALIZAR MEMORIA 

void finalizar_memoria(){
    log_destroy(memoria_logger);
	log_destroy(memoria_log_obligatorio);
	config_destroy(memoria_config);
}
