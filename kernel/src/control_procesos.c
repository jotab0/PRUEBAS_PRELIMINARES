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

		_poner_en_ejecucion(un_pcb);
	}
	pthread_mutex_unlock(&mutex_lista_exec);
}

void _poner_en_ejecucion(pcb* un_pcb){ // ATENCIÓN!!! ESTA FUNCIÓN DEBE SER LLAMADA ENTRE MUTEXES SIEMPRE
	
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
	if(ticket_referencia == ticket_actual){ // Esto es posible porque el ticket varía globalmente
											// Evita que se interrumpa un proceso que no debería ser interrumpido
		sem_post(&sem_enviar_interrupcion);	// FALTA AGREGAR: FUNCION EN kernel_cpu_interrupt QUE ENVÍE LA INTERRUPT
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


void planificador_corto_plazo(){
	// Aquí se debería evaluar si la lista ready NO está vacía para luego pasar a ejecutar la función planificar_corto_plazo()
	// Entonces mientras la lista ready no esté vacía se evaluará constantemente si la lista exec no está vacía
	
	switch(ALGORITMO_PCP_SELECCIONADO){
		case FIFO:
			if (!list_is_empty(ready)){
					planificar_corto_plazo();
			}
		break;

		case RR:
			if (!list_is_empty(ready)){
				planificar_corto_plazo();
			}
		break;

		case VRR:
			if (!list_is_empty(ready) || !list_is_empty(ready_plus)){
				planificar_corto_plazo();
			}
		break;

		default:
			log_error(kernel_logger_extra,"ERROR: Este algoritmo de planificación no es reconocido.");
			// Debería romer la ejecución?
	}
}

// DUDAS:
/*
1)
Al recibir el Contexto de Ejecución del proceso en ejecución, en caso de que el motivo de desalojo
implique replanificar se seleccionará el siguiente proceso a ejecutar según indique el algoritmo. 
Durante este período la CPU se quedará esperando el nuevo contexto.

Hace falta que el motivo sea replanificar si cada vez que se vacía la lista EXEC se replanifica de manera automática?
Yo creo que de esto en realidad se debería encargar la función que vuelva a poner el proceso en ready
Quizás usar un mutex de lista ready y ready plus para que se llame a la función planificar_corto_plazo dentro de esa función

2)Como hago para saber cuando se está ejecutando un proceso o no?

*/
	