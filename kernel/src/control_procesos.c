#include "../include/control_procesos.h"
// FUNCIONALIDADES PCB
pcb* crear_pcb(char* path, int size){
	
	pcb* nuevo_PCB = malloc(sizeof(pcb));
	
	nuevo_PCB->pid = asignar_pid();
    
	nuevo_PCB->program_counter = 0;
	
	nuevo_PCB->quantum = QUANTUM;
	nuevo_PCB->tiempo_ejecutado = 0;
	// nuevo_PCB->ticket = generar_ticket(); // Esto debería generarlo cuando lo pongo en exec?

	nuevo_PCB->ticket = -1; // Inicializa en -1 porque el valor del primer ticket global es 0
	
	nuevo_PCB->size = size;
	nuevo_PCB->path = path; //Que pasa si cambia o sucesde algo con lo que apunta
	
	nuevo_PCB->registros_CPU = malloc(sizeof(registrosCPU));
	nuevo_PCB->registros_CPU->AX = 0;
	nuevo_PCB->registros_CPU->BX = 0;
	nuevo_PCB->registros_CPU->CX = 0;
	nuevo_PCB->registros_CPU->DX = 0;

	nuevo_PCB->estado = NEW;
	// *****CONSULTAR:
	nuevo_PCB->motivo_bloqueo = BLOQUEO_NO_DEFINIDO; 
	nuevo_PCB->pedido_a_interfaz->nombre_interfaz=NULL;
	nuevo_PCB->pedido_a_interfaz->instruccion_a_interfaz=INSTRUCCION_IO_NO_DEFINIDA;
	
	return nuevo_PCB;
}

void cambiar_estado_pcb(pcb* un_pcb, estado_pcb nuevo_estado){
	un_pcb->estado = nuevo_estado;
}


// PLANIFICADOR LARGO PLAZO

void planificador_largo_plazo() { 

    while (1) {
        
		_check_interrupt_plp();

		// Chequeo condiciones para crear proceso
		// Acá podría haber un sem_wait para controlar grado de multiprogramación
		/*
		pthread_mutex_lock(&mutex_lista_new);
        if (list_is_empty(new)) {
            pthread_mutex_unlock(&mutex_lista_new);
            sleep(1); 
			// Ejecuto el while nuevamente 
            continue; 
        }*/
		// Espera a que ingrese un nuevo proceso por consola
		//*****CONSTULTAR SI EL ÚNICO QUE AGREGA A NEW ES LA CONSOLA
		sem_wait(&sem_lista_new);

		pcb* un_pcb = NULL;

		pthread_mutex_lock(&mutex_lista_new);
        un_pcb = list_remove(new, 0);
        pthread_mutex_unlock(&mutex_lista_new);
		
		if (un_pcb != NULL){
			
			// Envía la orden de iniciar estructura a memoria y espera con semáforo a que memoria la cree 
			iniciar_estructura_en_memoria(un_pcb);
			
			if(flag_respuesta_creacion_proceso == 0){
				
				// Vuelvo a agregar el pcb a la lista new en caso de que falló creación de proceso
				pthread_mutex_lock(&mutex_lista_new);
					list_add(new,un_pcb);
				pthread_mutex_lock(&mutex_lista_new);
				
				// Reinicio la bendera
				flag_respuesta_creacion_proceso = 1; // Asumo que no necesito mutex porque plp es el único que accede a este flag y son ejecuciones secuenciales
				
				// Ejecuto while nuevamente
				continue;
			}

			pthread_mutex_lock(&mutex_lista_ready);
			list_add(ready, un_pcb);
			pthread_mutex_unlock(&mutex_lista_ready);
			
			cambiar_estado_pcb(un_pcb, READY);
			log_info(kernel_logger, "Proceso %d movido a READY", un_pcb->pid);
			
			pthread_mutex_lock(&mutex_procesos_en_core); // (Lo dejo por las dudas)
			procesos_en_core++;
			pthread_mutex_unlock(&mutex_procesos_en_core);

			// Esto le avisa a pcp que se agrego algo a ready, entonces puede planificar
			sem_post(&sem_listas_ready);
			// Acá se frena si ya no hay lugar de multiprogramación, no hay más espera activa si no hay lugar
			// Debe haber sem_post en: Exit
			sem_wait(&sem_multiprogramacion);
		}
		else{
			log_error(kernel_logger, "ERROR: Se intentó cargar un proceso vacío");
		}
    }
}


// PLANIFICADOR CORTO PLAZO
// Método de planificación: FIFO, RR, VRR.

// En que casos debería planificar?
// 	- Llegada a READY y lista EXEC vacía
// 	- Salida por I/O				(Osea salida de exec)
// 	- Salida por fin de proceso		(Osea salida de exec)
// 	- Salida por fin de quantum		(Osea salida de exec)
//	- => CUANDO EXEC ESTÁ VACÍA

void planificador_corto_plazo(){ // Controla todo el tiempo la lista ready y ready_plus en caso de VRR
 
	while(1){
		
		_check_interrupt_pcp();
		// Espero a que se agregue algo a ready
		// Pensar que pasa si tengo proceso en blocked que entra y sale de readyplus, se me va a terminar bloqueando
		// Claro entonces en el que maneje la lista blocked va a mandar un sem_post de que agrego a readyplus !!!! IMPORTANTE !!!!
		sem_wait(&sem_listas_ready);
		switch(ALGORITMO_PCP_SELECCIONADO){
			case FIFO:

				pthread_mutex_lock(&mutex_lista_ready); 
				pthread_mutex_lock(&mutex_lista_ready_plus);

				if (!list_is_empty(ready)){
					
					planificar_corto_plazo();
				
				}

				pthread_mutex_unlock(&mutex_lista_ready);
				pthread_mutex_unlock(&mutex_lista_ready_plus);

			break;

			case RR:
								
				pthread_mutex_lock(&mutex_lista_ready);
				pthread_mutex_lock(&mutex_lista_ready_plus);

				if (!list_is_empty(ready)){

					planificar_corto_plazo();
					
				}

				pthread_mutex_unlock(&mutex_lista_ready);
				pthread_mutex_unlock(&mutex_lista_ready_plus);

			break;

			case VRR:

				pthread_mutex_lock(&mutex_lista_ready);
				pthread_mutex_lock(&mutex_lista_ready_plus);

				if (!list_is_empty(ready) || !list_is_empty(ready_plus)){

					planificar_corto_plazo();

				}

				pthread_mutex_unlock(&mutex_lista_ready); // nota
				pthread_mutex_unlock(&mutex_lista_ready_plus);

			break;

			default:
				log_error(kernel_logger_extra,"ERROR: Este algoritmo de planificación no es reconocido.");
				// Debería romer la ejecución?
		}
		// Esto es para reducir un poco la carga de la CPU
		sleep(1); 
	}

}

void planificar_corto_plazo(){ // ESTO PROBABLEMENTE SE EJECUTE CONSTANTEMENTE

	pthread_mutex_lock(&mutex_lista_exec); //Todo lo que afecte a las listas compartidas deberá ir entre sus mutexes
	if(list_is_empty(execute)){
		
		pcb* un_pcb = NULL;
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
	usleep(tiempo_restante); // El tiempo debe ser calculado en base a microsegundos
	pthread_mutex_lock(&mutex_ticket);
	if(ticket_referencia == ticket_actual){ 

		sem_post(&sem_enviar_interrupcion);	

	}
	pthread_mutex_unlock(&mutex_ticket);
}

// En teoría lo anterior está ok porque cuando un proceso vuelva porque se interrumpió su quantum
// la CPU debería haberme devuelto el proceso con su contexto de ejecución

void _check_interrupt_pcp(){
	switch (flag_interrupt_pcp) {
        case 0:
            sem_wait(&sem_interrupt_pcp); // Espera hasta que el semáforo sea señalizado
            break;
        default:
            // Aquí puedes manejar otros valores de flag_interrupt_pcp si es necesario
            break;
    }
}

void _check_interrupt_plp(){
	switch (flag_interrupt_plp) {
        case 0:
            sem_wait(&sem_interrupt_pcp); // Espera hasta que el semáforo sea señalizado
            break;
        default:
            // Aquí puedes manejar otros valores de flag_interrupt_pcp si es necesario
            break;
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

Puedo usar semáforos entre módulos?

3) que pasasa si hay varios planificadores a corto plazo corriendo al mismo tiempo? Acelera la ejecución del programa? PROBAR

*/

// FALTA:
// PLANIFICACIÓN BLOCKED A READY
// PLANIFICACIÓN BLOCKED A EXIT

void bloquear_proceso(pcb* un_pcb){
	// A TENER EN CUENTA:
	// Cuando un proceso se bloquea?
	// CPU me va a pedir que bloquee un proceso (olor a semáforo)
	// Proceso pidió recurso que no dispone

	// Cuando CPU me pide que lo bloquee tengo que sacarlo de exec si cumple con las siguientes condiciones
	// 		- La interfaz existe y se encuentra conectada (Tengo que tener lista de interfaces con su nombre como índice y
	//		  luego socket correspondiente)
	//		- La interfaz admite la operacion solicitada (En la lista anterior tambiém instrucciones que puede manejar?)
	//				-> Base de datos con nombres, sockets (dinámico) y instrucciones que pueden aceptar?

	switch (un_pcb->motivo_bloqueo)
	{
	case PEDIDO_A_INTERFAZ:
		manejar_pedido_a_interfaz(un_pcb);
		break;
	case RECURSO_FALTANTE:

		break;
	default:
		break;
	}
}

void manejar_pedido_a_interfaz (pcb* un_pcb){
	
}

void planificar_lista_exit(){
	// A TENER EN CUENTA:
	// Cuando un proceso sale a exit?
	// Cuando termina su ejecución (Me avisa CPU?)
	// Cuando falla (Me avisa CPU?)
	// Cuando lo pido por consola
	// !!!! IMPORTANTE !!!! Cuando proceso salga por exit, grado de multiprogramación debe aumentar
}