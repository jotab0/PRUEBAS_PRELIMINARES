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
        
        pcb_desactualizado->program_counter = pcb_nuevo->program_counter;
        
        pcb_desactualizado->registros_CPU->AX = pcb_nuevo->registros_CPU->AX;
        pcb_desactualizado->registros_CPU->BX = pcb_nuevo->registros_CPU->BX;
        pcb_desactualizado->registros_CPU->CX = pcb_nuevo->registros_CPU->CX;
        pcb_desactualizado->registros_CPU->DX = pcb_nuevo->registros_CPU->DX;

        pcb_desactualizado->tiempo_ejecutado =  pcb_nuevo->tiempo_ejecutado;
        pcb_desactualizado->pedido_a_interfaz = pcb_nuevo->pedido_a_interfaz;

    }
    else{
        log_error(kernel_logger,"Se intenó actualizar un pcb entre distintos procesos");
    }

}
