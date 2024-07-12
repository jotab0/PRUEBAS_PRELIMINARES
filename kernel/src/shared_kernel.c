#include "../include/shared_kernel.h"

int asignar_pid(){
    int valor_pid;

    pthread_mutex_lock(&mutex_pid); // PORQUE NECESITO ASEGURAR QUE SE EJECUTE ESTA FUNCIÓN DE A UNO POR VEZ (Zona Crítica)
    identificador_PID++;
    valor_pid = identificador_PID;
    pthread_mutex_unlock(&mutex_pid);

    return valor_pid;
}

int generar_ticket(){
	int valor_ticket;
	pthread_mutex_lock(&mutex_ticket);
	ticket_actual++;
	valor_ticket = ticket_actual;
	pthread_mutex_unlock(&mutex_ticket);
	return valor_ticket;
}

void list_add_sync(t_list* lista, void* un_elemento, pthread_mutex_t* mutex){

    pthread_mutex_lock(mutex);
    list_add(lista,un_elemento);
    pthread_mutex_unlock(mutex);

}

// Agrega pcb a nueva lista y cambia estado de manera sincronzada
void list_add_pcb_sync(t_list* lista, pcb* un_pcb, pthread_mutex_t* mutex, estado_pcb nuevo_estado){

    pthread_mutex_lock(mutex);
    list_add(lista,un_pcb);
	cambiar_estado_pcb(un_pcb,nuevo_estado);
    pthread_mutex_unlock(mutex);

}

void actualizar_pcb(pcb* pcb_desactualizado,pcb* pcb_nuevo){
    
    if(pcb_desactualizado->pid == pcb_nuevo->pid){
        
        pcb_desactualizado->program_counter     = pcb_nuevo->program_counter;
        
        pcb_desactualizado->registros_CPU->AX   = pcb_nuevo->registros_CPU->AX;
        pcb_desactualizado->registros_CPU->BX   = pcb_nuevo->registros_CPU->BX;
        pcb_desactualizado->registros_CPU->CX   = pcb_nuevo->registros_CPU->CX;
        pcb_desactualizado->registros_CPU->DX   = pcb_nuevo->registros_CPU->DX;
		pcb_desactualizado->registros_CPU->SI   = pcb_nuevo->registros_CPU->SI;
		pcb_desactualizado->registros_CPU->DI   = pcb_nuevo->registros_CPU->DI;

        pcb_desactualizado->tiempo_ejecutado    = pcb_nuevo->tiempo_ejecutado;

    }
    else{
        log_error(kernel_logger,"Se intenó actualizar un pcb entre distintos procesos");
    }

}

void destruir_pcb(pcb* un_pcb){
	free(un_pcb->registros_CPU);
	// Debería liberar memoria usada en las listas
	list_destroy(un_pcb->recursos_en_uso);
	list_destroy(un_pcb->pedido_a_interfaz->datos_auxiliares_interfaz); 
	if(un_pcb->pedido_a_interfaz->nombre_interfaz != NULL){
		free(un_pcb->pedido_a_interfaz->nombre_interfaz);
	}
	free(un_pcb->pedido_a_interfaz);
	free(un_pcb);
}

// Elimina pcb de lista y devuelve true si pudo hacerlo
pcb* _eliminar_pcb_de_lista_sync(pcb* un_pcb, t_list* una_lista, pthread_mutex_t* mutex){

	pcb* pcb_lista = NULL;

	bool coincide_pid (pcb* posible_pcb){
		return un_pcb->pid == posible_pcb->pid;
	}
	
	pthread_mutex_lock(mutex);
	if(list_any_satisfy(una_lista,(void*)coincide_pid)){
        
		pcb_lista = list_remove_by_condition(una_lista,(void*)coincide_pid);
		pthread_mutex_unlock(mutex);
		return pcb_lista;

	}
	pthread_mutex_unlock(mutex);

	return pcb_lista;
}

void agregar_a_ready(pcb* un_pcb){

	switch (ALGORITMO_PCP_SELECCIONADO)
	{
	case VRR:

		if(QUANTUM > un_pcb -> tiempo_ejecutado && un_pcb -> tiempo_ejecutado != 0){
			list_add_pcb_sync(ready_plus,un_pcb,&mutex_lista_ready_plus,READY);
		}
		else{
			list_add_pcb_sync(ready,un_pcb,&mutex_lista_ready,READY);
		}
		break;
	
	default:
		list_add_pcb_sync(ready,un_pcb,&mutex_lista_ready,READY);
		break;
	}
}

void cambiar_estado_pcb(pcb* un_pcb, estado_pcb nuevo_estado){
	un_pcb->estado = nuevo_estado;
}

void obtener_contexto_pcb(t_buffer* un_buffer, pcb* un_pcb){

	int pid_recibido = extraer_int_del_buffer(un_buffer);

	if(un_pcb->pid == pid_recibido){

		un_pcb -> program_counter = extraer_int_del_buffer(un_buffer);

		un_pcb -> registros_CPU -> AX = extraer_uint32_del_buffer(un_buffer);
		un_pcb -> registros_CPU -> BX = extraer_uint32_del_buffer(un_buffer);
		un_pcb -> registros_CPU -> CX = extraer_uint32_del_buffer(un_buffer);
		un_pcb -> registros_CPU -> DX = extraer_uint32_del_buffer(un_buffer);
		un_pcb -> registros_CPU -> SI = extraer_uint32_del_buffer(un_buffer);
		un_pcb -> registros_CPU -> DI = extraer_uint32_del_buffer(un_buffer);
		
		un_pcb -> tiempo_ejecutado = extraer_int_del_buffer(un_buffer);

	}else{
		log_error(kernel_logger_extra,"MÓDULO - shared_kernel: ERROR - Se intentó actualizar procesos con distinto PID");
		log_error(kernel_logger_extra,"MÓDULO - shared_kernel: PID recibido: %d",pid_recibido);
		log_error(kernel_logger_extra,"MÓDULO - shared_kernel: PID en EXECUTE: %d",un_pcb->pid);
		exit(EXIT_FAILURE);
	}	
}

pcb* buscar_pcb_en_sistema_(int pid){

		log_info(kernel_logger, "Buscando proceso en sistema con PID: %d", pid);
		pcb* un_pcb = NULL;
		
		un_pcb = _buscar_pcb_en_lista(pid,ready,&mutex_lista_ready);
		if(un_pcb == NULL){
			un_pcb = _buscar_pcb_en_lista(pid,ready_plus,&mutex_lista_ready_plus);
		}
		if (un_pcb == NULL){
			un_pcb = _buscar_pcb_en_lista(pid,blocked,&mutex_lista_blocked);
		}
		if (un_pcb == NULL){
			un_pcb = _buscar_pcb_en_lista(pid,new,&mutex_lista_new);
		}
		if (un_pcb == NULL){
			un_pcb = _buscar_pcb_en_lista(pid,execute,&mutex_lista_exec);
		}

	return un_pcb;
}

pcb* _buscar_pcb_en_lista(int pid, t_list* una_lista, pthread_mutex_t* mutex_lista){

	bool coincide_pid (pcb* posible_pcb){
		return pid == posible_pcb->pid;
	}

	pcb* un_pcb = NULL;

	pthread_mutex_lock(mutex_lista);
	if(list_any_satisfy(una_lista,(void*)coincide_pid)){
        
		un_pcb = list_find(una_lista,(void*)coincide_pid);
		pthread_mutex_unlock(mutex_lista);

		return un_pcb;

	}else{
		pthread_mutex_unlock(mutex_lista);

		return un_pcb;
	}
}

// FUNCIONES PARA LISTAS

void agregar_int_a_lista(t_list* una_lista, int un_valor){
	
	int *un_puntero = malloc(sizeof(int));
	*un_puntero = un_valor;
	list_add(una_lista,un_puntero);

}

void agregar_string_a_lista(t_list* una_lista, const char* un_string){
	
	char *un_puntero = malloc(sizeof(strlen(un_string) + 1));
	strcpy(un_puntero,un_string);
	list_add(una_lista,un_puntero);

}

// FUNCIONES GENÉRICAS

void decrementar_procesos_en_core(){
	pthread_mutex_lock(&mutex_procesos_en_core);
	procesos_en_core --;
	pthread_mutex_unlock(&mutex_procesos_en_core);
}

void mostrar_contexto(pcb* un_pcb){

	log_info(kernel_logger,"MOSTRANDO CONTEXTO DE PROCESO CON PID: %d", un_pcb->pid);

	log_info(kernel_logger,"PC: %d", un_pcb->program_counter);

	log_info(kernel_logger,"AX: %d", un_pcb->registros_CPU->AX);
	log_info(kernel_logger,"BX: %d", un_pcb->registros_CPU->BX);
	log_info(kernel_logger,"CX: %d", un_pcb->registros_CPU->CX);
	log_info(kernel_logger,"DX: %d", un_pcb->registros_CPU->DX);
	log_info(kernel_logger,"SI: %d", un_pcb->registros_CPU->SI);
	log_info(kernel_logger,"DI: %d", un_pcb->registros_CPU->DI);

	log_info(kernel_logger,"Tiempo ejecutado: %d", un_pcb->tiempo_ejecutado);

}











































// FUNCIONES QUE QUEDARON Y NO SE UTILIZAN

// Chequea de forma sincronizada si
// pcb se encuentra en la lista
bool _pcb_esta_en_lista(pcb* un_pcb, t_list* una_lista, pthread_mutex_t* mutex){

	bool coincide_pid (pcb* posible_pcb){
		return un_pcb->pid == posible_pcb->pid;
	}
	
    pthread_mutex_lock(mutex);
    if(list_any_satisfy(una_lista,(void*)coincide_pid)){
        pthread_mutex_unlock(mutex);
        return true;
    }
    else{
		pthread_mutex_unlock(mutex);
        return false;
    }
}

//BUSCA PCB Y LO QUITA DE LA LISTA
pcb* _extraer_pcb_de_lista_sistema(pcb* un_pcb){

	switch (un_pcb -> estado)
	{
		
	case NEW:

		un_pcb = _extraer_pcb_de_lista(un_pcb->pid,new,&mutex_lista_new);
		
		break;

	case READY:

		un_pcb = _extraer_pcb_de_lista(un_pcb->pid,ready,&mutex_lista_ready);
		
		if(un_pcb == NULL){
			un_pcb = _extraer_pcb_de_lista(un_pcb->pid,ready_plus,&mutex_lista_ready_plus);
		}

		break;

	case BLOCKED:

		un_pcb = _extraer_pcb_de_lista(un_pcb->pid,blocked,&mutex_lista_blocked);
	
		break;

	case EXEC:
	
		pthread_mutex_lock(&mutex_lista_exec);
		list_remove(execute,0);
		pthread_mutex_unlock(&mutex_lista_exec);

		break;

	case EXIT:

		un_pcb = _extraer_pcb_de_lista(un_pcb->pid,lista_exit,&mutex_lista_exit);

		break;
	}

	return un_pcb;
}

// Extrae pcb de lista si coincide pid, devuelve NULL en caso contrario
pcb* _extraer_pcb_de_lista(int pid, t_list* una_lista, pthread_mutex_t* mutex_lista){

	bool coincide_pid (pcb* posible_pcb){
		return pid == posible_pcb->pid;
	}

	pcb* un_pcb = NULL;

	pthread_mutex_lock(mutex_lista);
	if(list_any_satisfy(una_lista,(void*)coincide_pid)){
        
		un_pcb = list_remove_by_condition(una_lista,(void*)coincide_pid);
		pthread_mutex_unlock(mutex_lista);

		return un_pcb;

	}else{
		pthread_mutex_unlock(mutex_lista);

		return un_pcb;
	}
}

