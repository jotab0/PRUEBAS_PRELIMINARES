#include "../include/control_procesos.h"
// FUNCIONALIDADES PCB
pcb* crear_pcb(char* path, int size){
	
	pcb* nuevo_PCB = malloc(sizeof(pcb));
	
	nuevo_PCB->pid = asignar_pid();
    
	nuevo_PCB->program_counter = 0;
	
	nuevo_PCB->quantum = QUANTUM * 1000; // Multiplico por mil para pasarlo de milisegundos a microsegundos
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
// Agrega a ready cuando proceso llega a new (Solamente por consola)
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


				sem_post(&sem_lista_new);
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
			sem_post(&sem_pcp);
			// Acá se frena si ya no hay lugar de multiprogramación, no hay más espera activa si no hay lugar
			// Debe haber sem_post en: Exit
			sem_wait(&sem_multiprogramacion);
		}
		else{
			log_error(kernel_logger, "ERROR: Se intentó cargar un proceso vacío");
		}
    }
}

/*





















*/

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
		sem_wait(&sem_pcp);
		switch(ALGORITMO_PCP_SELECCIONADO){
		// Este switch me parece que no lo necesito, hace siempre lo mismo en cada case y no lo voy a expandir
			case FIFO:

				planificar_corto_plazo();

			break;

			case RR:

				planificar_corto_plazo();

			break;

			case VRR:

				planificar_corto_plazo();

			break;

			default:
				log_error(kernel_logger_extra,"ERROR: Este algoritmo de planificación no es reconocido.");
				// Debería romer la ejecución?
		}
 
	}

}

void planificar_corto_plazo(){ // ESTO PROBABLEMENTE SE EJECUTE CONSTANTEMENTE

	pthread_mutex_lock(&mutex_lista_ready); 
	pthread_mutex_lock(&mutex_lista_ready_plus);
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
	pthread_mutex_unlock(&mutex_lista_ready); 
	pthread_mutex_unlock(&mutex_lista_ready_plus);
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
	usleep(un_pcb -> quantum); // Multiplicar porque lo que me dan está en milisegundos
	pthread_mutex_lock(&mutex_ticket);
	if(ticket_referencia == ticket_actual){ // Esto es posible porque el ticket varía globalmente
											// Evita que se interrumpa un proceso que no debería ser interrumpido
		un_pcb = _gestionar_interrupcion();	// FALTA AGREGAR: FUNCION EN kernel_cpu_interrupt QUE ENVÍE LA INTERRUPT
		// Debería llegar mensaje de si se quitó proceso en ejecución?
		agregar_a_ready(un_pcb);
		sem_post(&sem_pcp);
	
	}
	pthread_mutex_unlock(&mutex_ticket);
}

void _programar_interrupcion_por_quantum_VRR(pcb* un_pcb){
	int ticket_referencia = un_pcb->ticket;
	int tiempo_restante = un_pcb->quantum - un_pcb->tiempo_ejecutado; // Consultar si está ok 
	usleep(tiempo_restante); // El tiempo debe ser calculado en base a microsegundos
	pthread_mutex_lock(&mutex_ticket);
	if(ticket_referencia == ticket_actual){ 

		un_pcb = _gestionar_interrupcion();

		agregar_a_ready(un_pcb);
		sem_post(&sem_pcp);

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

/*





















*/

void manejar_bloqueo_de_proceso(pcb* un_pcb){  // PASA DE EXECUTE A BLOCKED UN PROCESO QUE LLEGA DESDE LA CPU Y TIENE MOTIVO DE BLOQUEO
	
	// Cuando CPU me pide que lo bloquee tengo que sacarlo de exec si cumple con las siguientes condiciones
	// 		- La interfaz existe y se encuentra conectada (Tengo que tener lista de interfaces con su nombre como índice y
	//		  luego socket correspondiente)
	//		- La interfaz admite la operacion solicitada (En la lista anterior tambiém instrucciones que puede manejar?)
	//				-> Base de datos con nombres, sockets (dinámico) y instrucciones que pueden aceptar?
	
	// =========================================================================================================================
	
	// Decidí poner el mutez de la lista exec acá porque cada vez que quiera atender un bloqueo, no quiero que se me modifique
	// la lista exec, ya que podría llevar a errores, es por precaución. En un futuro podría cambiarse si puedo garantizar seguridad.
	pthread_mutex_lock(&mutex_lista_exec);

	switch (un_pcb->motivo_bloqueo)
	{
	case PEDIDO_A_INTERFAZ:

		ejecutar_en_hilo_detach((void*)manejar_pedido_a_interfaz,un_pcb);
		
		break;

	case RECURSO_FALTANTE:

		break;

	default:
		break;
	}
}

void manejar_pedido_a_interfaz (pcb* pcb_recibido){
	
	// Se evalúa si es posible, sino lo manda a exit

	pthread_mutex_lock(&mutex_lista_interfaces);

	if(_evaluar_diponibilidad_pedido(pcb_recibido)==0){
		log_info(kernel_logger, "Terminando proceso con PID: %d. Solicitud de instrucción inválida", pcb_recibido->pid);
		pthread_mutex_unlock(&mutex_lista_interfaces);
		pthread_mutex_unlock(&mutex_lista_exec);
		// No se hace más nada porque proceso se va a exit
	}else{
		// IMPORTANTE: Una vez que se entró acá, la interfaz está bloqueada (Se bloquea al evaluar su disponibilidad)
		interfaz* interfaz_solicitada = NULL;
		pcb* un_pcb = NULL;
		
		// Obtengo pcb ejecutando
		un_pcb = list_remove(execute, 0); 
		// Libero lista exec y le mando señal a planificador corto plazo
		pthread_mutex_unlock(&mutex_lista_exec);
		sem_post(&sem_pcp);
		// Actualizo pcb con lo que me devolvió la cpu y lo muevo a lista blocked
		actualizar_pcb(un_pcb,pcb_recibido);
		cambiar_estado_pcb(un_pcb,BLOCKED);
		list_add_sync(blocked,un_pcb,&mutex_lista_blocked);

		interfaz_solicitada = _traer_interfaz_solicitada(un_pcb);
		pthread_mutex_unlock(&mutex_lista_interfaces);

		int estado_solicitud = solicitar_instruccion_a_interfaz(un_pcb,interfaz_solicitada);
		sem_wait(&interfaz_solicitada->sem_interfaz);

		if(estado_solicitud == ERROR){
			planificar_proceso_exit(un_pcb);
			log_error(kernel_logger,"ERROR: La interfaz solicitada no pudo realizar la operacion");
		}
		else{
			if(_eliminar_pcb_de_lista_sync(un_pcb,blocked,&mutex_lista_blocked)){
				switch (ALGORITMO_PCP_SELECCIONADO)
				{
				case FIFO:
					cambiar_estado_pcb(un_pcb,READY);
					list_add_sync(ready,un_pcb,&mutex_lista_ready);
					break;
				
				case RR:
					cambiar_estado_pcb(un_pcb,READY);
					list_add_sync(ready,un_pcb,&mutex_lista_ready);
					break;
				
				case VRR: 	// !!!!!!!CORREGIR ESTO!!!!!!!
					if (un_pcb->tiempo_ejecutado>=QUANTUM){
						un_pcb->tiempo_ejecutado=(QUANTUM*99)/100;
						list_add_sync(ready_plus,un_pcb,&mutex_lista_ready_plus);
					}else{
						list_add_sync(ready_plus,un_pcb,&mutex_lista_ready_plus);
					}
					cambiar_estado_pcb(un_pcb,READY);
					break;
				}
				// Aviso a planificador corto plazo
				sem_post(&sem_pcp);
			}
		}
	}	
}

bool _evaluar_diponibilidad_pedido (pcb* un_pcb){

	
	bool _buscar_interfaz(interfaz* una_interfaz){
		
		char* nombre_encontrado = una_interfaz->nombre_interfaz;
		char* nombre_buscado = un_pcb->pedido_a_interfaz->nombre_interfaz;
		
		return strcmp(nombre_encontrado,nombre_buscado)==1;
	}

	bool _buscar_instruccion(int instruccion_encontrada){
		
		int instruccion_buscada = un_pcb->pedido_a_interfaz->instruccion_a_interfaz;
		return instruccion_buscada == instruccion_encontrada;

	}

	interfaz* una_interfaz = NULL;

	// Evalúo si existe interfaz
	if(list_any_satisfy(interfaces_conectadas,(void*)_buscar_interfaz)){
		una_interfaz = list_find(interfaces_conectadas,(void*)_buscar_interfaz); 
	}
	else{
		planificar_proceso_exit(un_pcb);
		return false;
	}

	// Evalúo si la interfaz cuenta con la instrucción que estoy solicitando
	if(list_any_satisfy(una_interfaz->instrucciones_disponibles,(void*)_buscar_instruccion)){
		// Si se que la voy a usar la bloqueo
		// Acá se empiezan a encolar procesos que quieran acceder a misma interfaz
		pthread_mutex_lock(&una_interfaz->mutex_interfaz);
		return true;
	}
	else{
		planificar_proceso_exit(un_pcb);
		return false;
	}

}

 interfaz* _traer_interfaz_solicitada(pcb* un_pcb){

	bool _buscar_interfaz(interfaz* una_interfaz){
		
		char* nombre_encontrado = una_interfaz->nombre_interfaz;
		char* nombre_buscado = un_pcb->pedido_a_interfaz->nombre_interfaz;
		
		return strcmp(nombre_encontrado,nombre_buscado)==1;
	}

	interfaz* una_interfaz = NULL;
	return una_interfaz = list_find(interfaces_conectadas,(void*)_buscar_interfaz); 
 }


/*





















*/
void planificar_proceso_exit(pcb* un_pcb){
	// A TENER EN CUENTA:
	// Cuando un proceso sale a exit?
	// Cuando termina su ejecución (Me avisa CPU?)
	// Cuando falla (Me avisa CPU?)
	// Cuando lo pido por consola
	// !!!! IMPORTANTE !!!! Cuando proceso salga por exit, grado de multiprogramación debe aumentar

	switch (un_pcb->estado)
	{
	
	case NEW:
	
		if(_eliminar_pcb_de_lista_sync(un_pcb,new,&mutex_lista_new)){
				
				destruir_pcb(un_pcb);
				
		}

		break;

	case READY:

		switch (ALGORITMO_PCP_SELECCIONADO)
		{
		case VRR:
			if(_eliminar_pcb_de_lista_sync(un_pcb,ready,&mutex_lista_ready)){
				liberar_memoria(un_pcb);
				destruir_pcb(un_pcb);
				sem_post(&sem_multiprogramacion);
				break;
			}
			
			else if(_eliminar_pcb_de_lista_sync(un_pcb,ready_plus,&mutex_lista_ready_plus)){
				liberar_memoria(un_pcb);
				destruir_pcb(un_pcb);
				sem_post(&sem_multiprogramacion);
				break;	
			}

			break;

		default:

			if(_eliminar_pcb_de_lista_sync(un_pcb,ready,&mutex_lista_ready)){
				liberar_memoria(un_pcb);
				destruir_pcb(un_pcb);
				sem_post(&sem_multiprogramacion);
			}
		break;
		}
	
	break;

	case BLOCKED:
	
		if(_eliminar_pcb_de_lista_sync(un_pcb,blocked,&mutex_lista_blocked)){
				liberar_memoria(un_pcb);
				destruir_pcb(un_pcb);
				sem_post(&sem_multiprogramacion);
		}

	break;

	case EXEC: 	// Este caso es para cuando consola me pide que haga exit de un proceso
		
		un_pcb = _gestionar_interrupcion(); 
		liberar_memoria(un_pcb);
		destruir_pcb(un_pcb);
		sem_post(&sem_multiprogramacion);
		sem_post(&sem_pcp);

	break;
	
	case EXIT: 	// Este caso lo voy a dejar para cuando CPU me pida hacer exit de un proceso
				// El sem_post para el pcp lo hago en el momento que me llega el mensaje de CPU
				// Porque antes de llamar a esta función saco el pcb de exec, lo paso a exit y lo mando a esta función

		if(_eliminar_pcb_de_lista_sync(un_pcb,lista_exit,&mutex_lista_exit)){
			liberar_memoria(un_pcb);
			destruir_pcb(un_pcb);
			sem_post(&sem_multiprogramacion);
		}
		
	break;
	
	default:
		break;
	}
}




