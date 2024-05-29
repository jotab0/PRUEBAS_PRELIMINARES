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

void actualizar_pcb(pcb* pcb_desactualizado,pcb* pcb_nuevo){
    
    if(pcb_desactualizado->pid == pcb_nuevo->pid){
        
        pcb_desactualizado->program_counter     = pcb_nuevo->program_counter;
        
        pcb_desactualizado->registros_CPU->AX   = pcb_nuevo->registros_CPU->AX;
        pcb_desactualizado->registros_CPU->BX   = pcb_nuevo->registros_CPU->BX;
        pcb_desactualizado->registros_CPU->CX   = pcb_nuevo->registros_CPU->CX;
        pcb_desactualizado->registros_CPU->DX   = pcb_nuevo->registros_CPU->DX;

        pcb_desactualizado->tiempo_ejecutado    = pcb_nuevo->tiempo_ejecutado;
        pcb_desactualizado->motivo_bloqueo      = pcb_nuevo->motivo_bloqueo;
        pcb_desactualizado->pedido_a_interfaz   = pcb_nuevo->pedido_a_interfaz;

    }
    else{
        log_error(kernel_logger,"Se intenó actualizar un pcb entre distintos procesos");
    }

}

interfaz* obtener_interfaz_con_nombre(char* nombre_interfaz){
	
	bool _buscar_interfaz(interfaz* una_interfaz){
		
		char* nombre_encontrado = una_interfaz->nombre_interfaz;		
		return strcmp(nombre_encontrado,nombre_interfaz)==1;

	}

	interfaz* una_interfaz = NULL;
	if(list_any_satisfy(interfaces_conectadas,(void*)_buscar_interfaz)){
		return una_interfaz = list_find(interfaces_conectadas,(void*)_buscar_interfaz); // Por qué no me la reconoce?
	}
	else{
		log_error(kernel_logger,"Se solicitó interfaz que ya no se encuentra conectada");
		// En este caso retorna NULL
		return NULL;
	}
}


void destruir_pcb(pcb* un_pcb){
	free(un_pcb->registros_CPU);
	free(un_pcb->pedido_a_interfaz);
	free(un_pcb);
}


// Chequea de forma sincronizada si
// pcb se encuentra en la lista
bool pcb_esta_en_lista(pcb* un_pcb, t_list* una_lista, pthread_mutex_t* mutex){

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

// Elimina pcb de lista y devuelve true si pudo hacerlo
bool _eliminar_pcb_de_lista_sync(pcb* un_pcb, t_list* una_lista, pthread_mutex_t* mutex){

	bool coincide_pid (pcb* posible_pcb){
		return un_pcb->pid == posible_pcb->pid;
	}
	
	pthread_mutex_lock(mutex);
	if(list_any_satisfy(una_lista,(void*)coincide_pid)){
        
		list_remove_by_condition(una_lista,(void*)coincide_pid);
		pthread_mutex_unlock(mutex);
		return true;

	}

	return false;
}

pcb* buscar_pcb_en_sistema(int pid){

	pcb* un_pcb = NULL;
	switch (un_pcb -> estado)
	{
		
	case NEW:

		un_pcb = extraer_pcb_de_lista(pid,new,&mutex_lista_new);
		
		break;

	case READY:

		un_pcb = extraer_pcb_de_lista(pid,ready,&mutex_lista_ready);
		
		if(un_pcb == NULL){
			un_pcb = extraer_pcb_de_lista(pid,ready_plus,&mutex_lista_ready_plus);
		}

		break;

	case BLOCKED:

		un_pcb = extraer_pcb_de_lista(pid,blocked,&mutex_lista_blocked);
	
		break;

	case EXEC:
	
		pthread_mutex_lock(&mutex_lista_exec);
		list_remove(execute,0);
		pthread_mutex_unlock(&mutex_lista_exec);
		sem_post(&sem_pcp);

		break;

	case EXIT:

		un_pcb = extraer_pcb_de_lista(pid,lista_exit,&mutex_lista_exit);

		break;
	}

	return un_pcb;
}


// Extrae pcb de lista si coincide pid, devuelve NULL en caso contrario
pcb* extraer_pcb_de_lista(int pid, t_list* una_lista, pthread_mutex_t* mutex_lista){

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

void agregar_a_ready(pcb* un_pcb){

	switch (ALGORITMO_PCP_SELECCIONADO)
	{
	case VRR:
		if(QUANTUM > un_pcb -> tiempo_ejecutado && un_pcb -> tiempo_ejecutado != 0){
			cambiar_estado_pcb(un_pcb,READY);
			list_add_sync(ready_plus,un_pcb,&mutex_lista_ready_plus);
		}
		else{
			cambiar_estado_pcb(un_pcb,READY);
			list_add_sync(ready,un_pcb,&mutex_lista_ready);
		}
		break;
	
	default:
		cambiar_estado_pcb(un_pcb,READY);
		list_add_sync(ready,un_pcb,&mutex_lista_ready);
		break;
	}
}

void cambiar_estado_pcb(pcb* un_pcb, estado_pcb nuevo_estado){
	un_pcb->estado = nuevo_estado;
}
