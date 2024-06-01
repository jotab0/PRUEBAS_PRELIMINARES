#include "../include/control_procesos.h"
// FUNCIONALIDADES PCB
pcb *crear_pcb(char *path, int size)
{

	pcb *nuevo_PCB = malloc(sizeof(pcb));

	nuevo_PCB->pid = asignar_pid();

	nuevo_PCB->program_counter = 0;

	nuevo_PCB->quantum = QUANTUM; // Multiplico por mil para pasarlo de milisegundos a microsegundos
	nuevo_PCB->tiempo_ejecutado = 0;
	// nuevo_PCB->ticket = generar_ticket(); // Esto debería generarlo cuando lo pongo en exec?

	nuevo_PCB->ticket = -1; // Inicializa en -1 porque el valor del primer ticket global es 0

	nuevo_PCB->size = size;
	nuevo_PCB->path = path; // Que pasa si cambia o sucesde algo con lo que apunta

	nuevo_PCB->registros_CPU = malloc(sizeof(registrosCPU));
	nuevo_PCB->registros_CPU->AX = 0;
	nuevo_PCB->registros_CPU->BX = 0;
	nuevo_PCB->registros_CPU->CX = 0;
	nuevo_PCB->registros_CPU->DX = 0;

	nuevo_PCB->estado = NEW;
	// *****CONSULTAR:
	nuevo_PCB->motivo_bloqueo = BLOQUEO_NO_DEFINIDO;
	nuevo_PCB->pedido_a_interfaz->nombre_interfaz = NULL;
	nuevo_PCB->pedido_a_interfaz->instruccion_a_interfaz = INSTRUCCION_IO_NO_DEFINIDA;

	return nuevo_PCB;
}

// Semáforos que necesita para funcionar:
// - sem_lista_new

// PLANIFICADOR LARGO PLAZO
// Agrega a ready cuando proceso llega a new (Solamente por consola)
void planificador_largo_plazo()
{

	while (1)
	{

		_check_interrupt_plp();

		sem_wait(&sem_lista_new);

		pcb *un_pcb = NULL;

		pthread_mutex_lock(&mutex_lista_new);
		un_pcb = list_remove(new, 0);
		pthread_mutex_unlock(&mutex_lista_new);

		if (un_pcb != NULL)
		{

			// Envía la orden de iniciar estructura a memoria y espera con semáforo a que memoria la cree
			iniciar_estructura_en_memoria(un_pcb);

			if (flag_respuesta_creacion_proceso == 0) 
			{

				// Vuelvo a agregar el pcb a la lista new en caso de que falló creación de proceso
				pthread_mutex_lock(&mutex_lista_new);
				list_add(new, un_pcb);
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

			log_info(kernel_logger, " PID: %d - SET: READY", un_pcb->pid);

			pthread_mutex_lock(&mutex_procesos_en_core); // (Lo dejo por las dudas)
			procesos_en_core++;
			pthread_mutex_unlock(&mutex_procesos_en_core);

			// Esto le avisa a pcp que se agrego algo a ready, entonces puede planificar
			sem_post(&sem_pcp);
			// Acá se frena si ya no hay lugar de multiprogramación, no hay más espera activa si no hay lugar
			// Debe haber sem_post cada vez que se manda un proceso a exit
			sem_wait(&sem_multiprogramacion);
		}
		else
		{
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
//  - Salida por interrupción desde consola
//	- => CUANDO EXEC ESTÁ VACÍA

void planificador_corto_plazo()
{ // Controla todo el tiempo la lista ready y ready_plus en caso de VRR

	while (1)
	{

		_check_interrupt_pcp();
		// Espero a que se agregue algo a ready
		sem_wait(&sem_pcp);
		sem_wait(&sem_cpu_libre);

		switch (ALGORITMO_PCP_SELECCIONADO)
		{

			case FIFO:

			case RR:

			case VRR:

				planificar_corto_plazo();

				break;

			default:
				log_error(kernel_logger_extra, "ERROR: Este algoritmo de planificación no es reconocido.");
				// Debería romer la ejecución?
		}
	}
}

void planificar_corto_plazo()
{

	// BLOQUEO LISTA EXEC HASTA QUE TERMINE EL PCP
	pthread_mutex_lock(&mutex_lista_exec);

	if (list_is_empty(execute))
	{ // ESTO NO SÉ SI LO NECESITO CON EL SEMÁFORO sem_cpu_libre, CONSULTAR

		pcb *un_pcb = NULL;

		pthread_mutex_lock(&mutex_lista_ready);
		pthread_mutex_lock(&mutex_lista_ready_plus);

		if (!list_is_empty(ready_plus))
		{
			un_pcb = list_remove(ready_plus, 0);
		}
		else if (!list_is_empty(ready))
		{
			un_pcb = list_remove(ready, 0); // Me trae el primer elemento de la lista ready
		}
		pthread_mutex_unlock(&mutex_lista_ready);
		pthread_mutex_unlock(&mutex_lista_ready_plus);

		_poner_en_ejecucion(un_pcb);
	}

	pthread_mutex_unlock(&mutex_lista_exec);
}

void _poner_en_ejecucion(pcb *un_pcb)
{

	if (un_pcb != NULL)
	{

		list_add(execute, un_pcb);

		log_info(kernel_logger, " PID: %d - SET: EXEC", un_pcb->pid);
		un_pcb->ticket = generar_ticket();

		enviar_pcb_CPU_dispatch(un_pcb);
		cambiar_estado_pcb(un_pcb, EXEC);

		if (strcmp(ALGORITMO_PLANIFICACION, "RR") == 0)
		{
			ejecutar_en_hilo_detach((void *)_programar_interrupcion_por_quantum_RR, un_pcb);
		}
		else if (strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0)
		{
			ejecutar_en_hilo_detach((void *)_programar_interrupcion_por_quantum_VRR, un_pcb);
		}
	}
	else
	{
		log_warning(kernel_logger, "Lista de READY vacía");
	}
}

void _programar_interrupcion_por_quantum_RR(pcb *un_pcb)
{

	int ticket_referencia = un_pcb->ticket;
	usleep(un_pcb->quantum);

	pthread_mutex_lock(&mutex_ticket);
	if (ticket_referencia == ticket_actual)
	{
		_gestionar_interrupcion(un_pcb, QUANTUM_INTERRUPT);
	}
	pthread_mutex_unlock(&mutex_ticket);
}

void _programar_interrupcion_por_quantum_VRR(pcb *un_pcb)
{
	int ticket_referencia = un_pcb->ticket;
	int tiempo_restante = un_pcb->quantum - un_pcb->tiempo_ejecutado; // Consultar si está ok
	usleep(tiempo_restante);										  // El tiempo debe ser calculado en base a microsegundos
	pthread_mutex_lock(&mutex_ticket);
	if (ticket_referencia == ticket_actual)
	{
		_gestionar_interrupcion(un_pcb, QUANTUM_INTERRUPT);
	}
	pthread_mutex_unlock(&mutex_ticket);
}

// En teoría lo anterior está ok porque cuando un proceso vuelva porque se interrumpió su quantum
// la CPU debería haberme devuelto el proceso con su contexto de ejecución

void _check_interrupt_pcp()
{
	switch (flag_interrupt_pcp)
	{
	case 0:
		sem_wait(&sem_interrupt_pcp); // Espera hasta que el semáforo sea señalizado
		break;
	default:

		break;
	}
}

void _check_interrupt_plp()
{
	switch (flag_interrupt_plp)
	{
	case 0:
		sem_wait(&sem_interrupt_plp); // Espera hasta que el semáforo sea señalizado
		break;
	default:

		break;
	}
}

/*





















*/

// MANEJA PROCESOS QUE SE BLOQUEAN EN CPU
void manejar_bloqueo_de_proceso(pcb *un_pcb)
{

	// Cuando CPU me pide que lo bloquee tengo que sacarlo de exec si cumple con las siguientes condiciones
	// 		- La interfaz existe y se encuentra conectada (Tengo que tener lista de interfaces con su nombre como índice y
	//		  luego socket correspondiente)
	//		- La interfaz admite la operacion solicitada (En la lista anterior tambiém instrucciones que puede manejar?)
	//				-> Base de datos con nombres, sockets (dinámico) y instrucciones que pueden aceptar?

	// =========================================================================================================================

	switch (un_pcb->motivo_bloqueo)
	{
	case PEDIDO_A_INTERFAZ:

		ejecutar_en_hilo_detach((void *)manejar_pedido_a_interfaz, un_pcb);

		break;

	case RECURSO_FALTANTE:

		break;

	default:
		break;
	}
}

// Recibe pcb actualizado y gestiona el pedido a la interfaz
void manejar_pedido_a_interfaz(pcb *pcb_recibido)
{

	// Se evalúa si es posible, sino lo manda a exit

	pthread_mutex_lock(&mutex_lista_interfaces); // CONSULTAR: Si está bien el mutex

	if (!_evaluar_diponibilidad_pedido(pcb_recibido))
	{
		log_info(kernel_logger, "Terminando proceso con PID: %d. Solicitud de instrucción inválida", pcb_recibido->pid);
		pthread_mutex_unlock(&mutex_lista_interfaces);
		// No se hace más nada porque proceso se va a exit
	}
	else
	{
		// IMPORTANTE: Una vez que se entró acá, la interfaz está bloqueada (Se bloquea al evaluar su disponibilidad)
		interfaz *interfaz_solicitada = NULL;
		interfaz_solicitada = _traer_interfaz_solicitada(pcb_recibido);

		pthread_mutex_unlock(&mutex_lista_interfaces);

		int estado_solicitud = solicitar_instruccion_a_interfaz(pcb_recibido, interfaz_solicitada);

		if (estado_solicitud == ERROR)
		{
			planificar_proceso_exit_en_hilo(pcb_recibido);
			log_error(kernel_logger, "ERROR: La interfaz solicitada no pudo realizar la operacion");

		}else{
			agregar_a_ready(pcb_recibido);
			sem_post(&sem_pcp);
		}
	}
}

bool _evaluar_diponibilidad_pedido(pcb *un_pcb) // 	CONSULTAR: Si está bien como bloqueo interfaz
{

	bool _buscar_interfaz(interfaz * una_interfaz)
	{

		char *nombre_encontrado = una_interfaz->nombre_interfaz;
		char *nombre_buscado = un_pcb->pedido_a_interfaz->nombre_interfaz;

		return strcmp(nombre_encontrado, nombre_buscado) == 1;
	}

	bool _buscar_instruccion(int instruccion_encontrada)
	{

		int instruccion_buscada = un_pcb->pedido_a_interfaz->instruccion_a_interfaz;
		return instruccion_buscada == instruccion_encontrada;
	}

	interfaz *una_interfaz = NULL;

	// Evalúo si existe interfaz
	if (list_any_satisfy(interfaces_conectadas, (void *)_buscar_interfaz))
	{
		una_interfaz = list_find(interfaces_conectadas, (void *)_buscar_interfaz);
	}
	else
	{
		planificar_proceso_exit_en_hilo(un_pcb);
		return false;
	}

	// Evalúo si la interfaz cuenta con la instrucción que estoy solicitando
	if (list_any_satisfy(una_interfaz->instrucciones_disponibles, (void *)_buscar_instruccion))
	{
		// Si se que la voy a usar la bloqueo
		// Acá se empiezan a encolar procesos que quieran acceder a misma interfaz
		pthread_mutex_lock(&una_interfaz->mutex_interfaz);
		return true;
	}
	else
	{
		planificar_proceso_exit_en_hilo(un_pcb);
		return false;
	}
}

interfaz* _traer_interfaz_solicitada(pcb *un_pcb)
{

	bool _buscar_interfaz(interfaz * una_interfaz)
	{

		char *nombre_encontrado = una_interfaz->nombre_interfaz;
		char *nombre_buscado = un_pcb->pedido_a_interfaz->nombre_interfaz;

		return strcmp(nombre_encontrado, nombre_buscado) == 1;
	}

	interfaz *una_interfaz = NULL;
	return una_interfaz = list_find(interfaces_conectadas, (void *)_buscar_interfaz);
}

/*





















*/

void planificar_proceso_exit_en_hilo(pcb* un_pcb){
	ejecutar_en_hilo_detach((void *)planificar_proceso_exit, un_pcb);
}

void planificar_proceso_exit(pcb *un_pcb)
{
	// A TENER EN CUENTA:
	// Cuando un proceso sale a exit?
	// Cuando termina su ejecución (Me avisa CPU?)
	// Cuando falla (Me avisa CPU?)
	// Cuando lo pido por consola
	// !!!! IMPORTANTE !!!! Cuando proceso salga por exit, grado de multiprogramación debe aumentar

	switch (un_pcb->estado)
	{

	case NEW:

		if (_eliminar_pcb_de_lista_sync(un_pcb, new, &mutex_lista_new))
		{
			destruir_pcb(un_pcb);
		}

		break;

	case READY:

		switch (ALGORITMO_PCP_SELECCIONADO)
		{
		case VRR:
			if (_eliminar_pcb_de_lista_sync(un_pcb, ready, &mutex_lista_ready))
			{
				liberar_recursos_pcb(un_pcb);
				destruir_pcb(un_pcb);
				sem_post(&sem_multiprogramacion);
				break;
			}

			else if (_eliminar_pcb_de_lista_sync(un_pcb, ready_plus, &mutex_lista_ready_plus))
			{
				liberar_recursos_pcb(un_pcb);
				destruir_pcb(un_pcb);
				sem_post(&sem_multiprogramacion);
				break;
			}

			break;

		default:

			if (_eliminar_pcb_de_lista_sync(un_pcb, ready, &mutex_lista_ready))
			{
				liberar_recursos_pcb(un_pcb);
				destruir_pcb(un_pcb);
				sem_post(&sem_multiprogramacion);
			}
			break;
		}

		break;

	case BLOCKED:

		if (_eliminar_pcb_de_lista_sync(un_pcb, blocked, &mutex_lista_blocked))
		{
			liberar_recursos_pcb(un_pcb);
			destruir_pcb(un_pcb);
			sem_post(&sem_multiprogramacion);
		}

		break;

	case EXEC: // Este caso es para cuando consola me pide que haga exit de un proceso

		_gestionar_interrupcion(un_pcb, EXIT_PROCESS);
		sem_post(&sem_multiprogramacion);

		break;

	case EXIT: // Este caso lo voy a dejar para cuando CPU me pida hacer exit de un proceso
			   // El sem_post para el pcp lo hago en el momento que me llega el mensaje de CPU
			   // Porque antes de llamar a esta función saco el pcb de exec, lo paso a exit y lo mando a esta función

		if (_eliminar_pcb_de_lista_sync(un_pcb, lista_exit, &mutex_lista_exit))
		{
			liberar_recursos_pcb(un_pcb);
			destruir_pcb(un_pcb);
			sem_post(&sem_multiprogramacion);
		}

		break;

	default:
		break;
	}
}
