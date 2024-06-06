# include "../include/paginacion.h"


t_marco* devolver_marco_buscado(t_pagina* una_pagina){
    t_marco* un_marco;


    return un_marco;

}

//-------------------------------------------------------------------------------------------------------
// TABLA DE PAGINAS

void inicializar_tabla_de_paginas(t_proceso* nuevo_proceso){
    int cant_paginas =(int)ceil(nuevo_proceso->size/TAM_PAGINA);
    for(int i=0, i<cant_paginas,i++){
        t_pagina* pagina_nueva = malloc(sizeof(t_pagina));
        pagina_nueva->nro_pagina = i;
        pagina_nueva->nro_marco = NULL;
        list_add(nuevo_proceso->tabla_paginas,pagina_nueva);

    }
}

//-------------------------------------------------------------------------------------------------------
// FRAMES

t_marco* crear_frame(int tamBase, bool disponible, int numero_marco){
    t_marco *marco = malloc(sizeof(t_marco));
	marco->nro_marco = numero_marco;
	marco->base = tamBase;
	marco->disponible = disponible;
    marco->proceso = NULL;
    marco->num_pagina = NULL;	

	return marco;
}

void liberar_frame(t_marco* un_marco){
    un_marco->disponible = true;

}

// TENGO Q USAR SEMAFOROS??

t*marco buscar_marco_segun_numero(int num_marco){
    t_marco* un_marco;
    un_marco = list_get(lista_marcos, num_marco);
    return un_marco;
}


t_marco* obtener_frame_disponible(){
	t_marco* un_marco = NULL;

	bool _marco_libre(t_marco* un_marco){
		return un_marco->libre;
	}
//	pthread_mutex_lock(&mutex_lista_marcos);
	un_marco = list_find(lista_marcos, (void*)_marco_libre);
	
	if(un_marco == NULL){
		//ERROR OUT OF MEMORY
        log_error(memoria_logger,"Error: out of memory");
        fprintf(stderr, "Error: Out Of Memory\n");
        exit(EXIT_FAILURE);
	}
		
	un_marco->libre = false;
//	pthread_mutex_unlock(&mutex_lista_marcos);

	return un_marco;
}

