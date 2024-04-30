#include "../include/control_procesos.h"

pcb* crear_pcb(char* path, char* size, char* prioridad){
	pcb* nuevo_PCB = malloc(sizeof(pcb));
	
	pthread_mutex_lock(&mutex_pid);
	identificador_PID++;
	pthread_mutex_unlock(&mutex_pid);
	
	nuevo_PCB->pid = identificador_PID;
    nuevo_PCB->program_counter = 0;
	nuevo_PCB->quantum = TAM_QUANTUM;

	nuevo_PCB->registros_CPU = malloc(sizeof(registrosCPU));
	nuevo_PCB->registros_CPU->AX = 0;
	nuevo_PCB->registros_CPU->BX = 0;
	nuevo_PCB->registros_CPU->CX = 0;
	nuevo_PCB->registros_CPU->DX = 0;

	return nuevo_PCB;
}

// PLANIFICADOR LARGO PLAZO

// PLANIFICADOR CORTO PLAZO
// Método de planificación: FIFO, RR.

void cambiar_estado(pcb* un_pcb, estado_pcb nuevo_estado){
	un_pcb->estado = nuevo_estado;
}

// PROCESOS A CPU

void atender_FIFO_RR(){

	pthread_mutex_lock(&mutex_lista_exec);
	if(list_is_empty(execute)){
		
		pcb* un_pcb = NULL;

		pthread_mutex_lock(&mutex_lista_ready);
		if(!list_is_empty(ready)){
			un_pcb = list_remove(ready, 0); // Me trae el primer elemento de la lista ready
		}
		pthread_mutex_unlock(&mutex_lista_ready);

		if(un_pcb != NULL){
			list_add(execute, un_pcb);
			cambiar_estado(un_pcb, EXEC);
			log_info(kernel_logger, " PID: %d - Estado Anterior: READY - Estado Actual: EXEC", un_pcb -> pid);
			un_pcb->ticket = generar_ticket();
			
			enviar_pcb_CPU_dispatch(un_pcb);

			if(strcmp(ALGORITMO_PLANIFICACION, "RR") == 0){
				pthread_mutex_lock(&mutex_flag_exit);
				flag_exit = false; //Seteo en falso para que si es necesario algún otro proceso lo cambie
				pthread_mutex_unlock(&mutex_flag_exit);
				ejecutar_en_hilo_detach((void*)_programar_interrupcion_por_quantum, un_pcb);
			}
			
		}else{
			log_warning(kernel_logger, "Lista de READY vacía");
		}
	}
	pthread_mutex_unlock(&mutex_lista_exec);
}

void _programar_interrupcion_por_quantum(pcb* un_pcb){
	int ticket_referencia = un_pcb->ticket;
	usleep(TAM_QUANTUM*1000);
	pthread_mutex_lock(&mutex_ticket);
	if(ticket_referencia == ticket_actual){
		pthread_mutex_lock(&mutex_flag_exit);
		if(!flag_exit){
			sem_post(&sem_enviar_interrupcion);
		}
		pthread_mutex_unlock(&mutex_flag_exit);
	}
	pthread_mutex_unlock(&mutex_ticket);
}