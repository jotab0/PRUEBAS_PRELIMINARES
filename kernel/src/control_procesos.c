#include "../include/control_procesos.h"
// FUNCIONALIDADES PCB
pcb* crear_pcb(char* path, char* size, char* prioridad){
	pcb* nuevo_PCB = malloc(sizeof(pcb));
	
	nuevo_PCB->pid = asignar_pid();
    nuevo_PCB->program_counter = 0;
	nuevo_PCB->quantum = QUANTUM;
	nuevo_PCB->tiempo_ejecutado = 0;
	nuevo_PCB->ticket = generar_ticket();
	nuevo_PCB->registros_CPU = malloc(sizeof(registrosCPU));
	nuevo_PCB->registros_CPU->AX = 0;
	nuevo_PCB->registros_CPU->BX = 0;
	nuevo_PCB->registros_CPU->CX = 0;
	nuevo_PCB->registros_CPU->DX = 0;

	return nuevo_PCB;
}

void cambiar_estado_pcb(pcb* un_pcb, estado_pcb nuevo_estado){
	un_pcb->estado = nuevo_estado;
}


// PLANIFICADOR LARGO PLAZO

// PLANIFICADOR CORTO PLAZO
// Método de planificación: FIFO, RR, VRR.

// En que casos debería planificar?
// 	- Llegada a READY y lista EXEC vacía
// 	- Salida por I/O				(Osea salida de exec)
// 	- Salida por fin de proceso		(Osea salida de exec)
// 	- Salida por fin de quantum		(Osea salida de exec)

void planificar_corto_plazo(){ // ESTO PROBABLEMENTE SE EJECUTE CONSTANTEMENTE

	pthread_mutex_lock(&mutex_lista_exec); //Todo lo que afecte a las listas compartidas deberá ir entre sus mutexes
	if(list_is_empty(execute)){
		
		pcb* un_pcb = NULL;
		pthread_mutex_lock(&mutex_lista_ready);
		pthread_mutex_lock(&mutex_lista_ready_plus);
		if (!list_is_empty(ready_plus)){
			un_pcb = list_remove(ready_plus, 0);
		}
		else if(!list_is_empty(ready)){
			un_pcb = list_remove(ready, 0); // Me trae el primer elemento de la lista ready
		}
		pthread_mutex_unlock(&mutex_lista_ready);
		pthread_mutex_unlock(&mutex_lista_ready_plus);

		poner_en_ejecucion(un_pcb);
	}
	pthread_mutex_unlock(&mutex_lista_exec);
}

void poner_en_ejecucion(pcb* un_pcb){ // ATENCIÓN!!! ESTA FUNCIÓN DEBE SER LLAMADA ENTRE MUTEXES SIEMPRE
	
	if(un_pcb != NULL){
			list_add(execute, un_pcb);
			cambiar_estado_pcb(un_pcb, EXEC);
			log_info(kernel_logger, " PID: %d - SET: EXEC", un_pcb -> pid);
			un_pcb->ticket = generar_ticket();
			
			enviar_pcb_CPU_dispatch(un_pcb);

			if(strcmp(ALGORITMO_PLANIFICACION, "RR") == 0){
				ejecutar_en_hilo_detach((void*)_programar_interrupcion_por_quantum_RR, un_pcb);
			}
			else if(strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0){
				ejecutar_en_hilo_detach((void*)_programar_interrupcion_por_quantum_VRR, un_pcb);
			}
	}
	else{
			log_warning(kernel_logger, "Lista de READY vacía");
	}
}

void _programar_interrupcion_por_quantum_RR(pcb* un_pcb){ // Que pasa si el proceso salió antes porque abortó? Se envía el interrupt igual, como resuelvo?
	int ticket_referencia = un_pcb->ticket;
	usleep(un_pcb -> quantum);
	pthread_mutex_lock(&mutex_ticket);
	if(ticket_referencia == ticket_actual){ // Esto está bien auncque se cambie en cpu? // Esto es posible porque el ticket varía globalmente
		sem_post(&sem_enviar_interrupcion);	
	}
	pthread_mutex_unlock(&mutex_ticket);
}

void _programar_interrupcion_por_quantum_VRR(pcb* un_pcb){
	int ticket_referencia = un_pcb->ticket;
	int tiempo_restante = un_pcb->quantum - un_pcb->tiempo_ejecutado; // Consultar si está ok
	usleep(tiempo_restante);
	pthread_mutex_lock(&mutex_ticket);
	if(ticket_referencia == ticket_actual){ 
		sem_post(&sem_enviar_interrupcion);	
	}
	pthread_mutex_unlock(&mutex_ticket);
}

// En teoría lo anterior está ok porque cuando un proceso vuelva porque se interrumpió su quantum
// la CPU debería haberme devuelto el proceso con su contexto de ejecución
