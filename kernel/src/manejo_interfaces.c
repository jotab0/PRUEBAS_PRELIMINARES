#include "../include/manejo_interfaces.h"

///////// INTERFACES

// Recibe pcb actualizado y gestiona el pedido a la interfaz
void manejar_pedido_a_interfaz(pcb *pcb_recibido)
{

	// Se evalúa si es posible, sino lo manda a exit

	pthread_mutex_lock(&mutex_lista_interfaces); 

	if (!_evaluar_diponibilidad_pedido(pcb_recibido))
	{
		log_info(kernel_logger, "Terminando proceso con PID: %d. Solicitud de instrucción inválida", pcb_recibido->pid);
		pthread_mutex_unlock(&mutex_lista_interfaces);
		planificar_proceso_exit_en_hilo(pcb_recibido);
	}
	else
	{
		// IMPORTANTE: Una vez que se entró acá, la interfaz está bloqueada (Se bloquea al evaluar su disponibilidad)
		interfaz *interfaz_solicitada = NULL;
		interfaz_solicitada = _traer_interfaz_solicitada(pcb_recibido);

		pthread_mutex_unlock(&mutex_lista_interfaces);
		
		pthread_mutex_lock(&interfaz_solicitada->mutex_interfaz);
		//int estado_solicitud = solicitar_instruccion_a_interfaz(pcb_recibido, interfaz_solicitada);
		list_add(interfaz_solicitada->lista_procesos_en_cola,pcb_recibido);
		pthread_mutex_unlock(&interfaz_solicitada->mutex_interfaz);

		sem_post(&interfaz_solicitada->sem_request_interfaz);
		
	}	
}

bool _evaluar_diponibilidad_pedido(pcb *un_pcb) // 	CONSULTAR: Si está bien como bloqueo interfaz
{

	bool _buscar_interfaz(interfaz * una_interfaz)
	{
		return strcmp(una_interfaz->nombre_interfaz, un_pcb->pedido_a_interfaz->nombre_interfaz) == 0;
	}

	bool _buscar_instruccion(instruccion_interfaz *instruccion_encontrada)
	{
		return un_pcb->pedido_a_interfaz->instruccion_a_interfaz == *instruccion_encontrada;
	}

	interfaz *una_interfaz = NULL;

	
	if (list_any_satisfy(interfaces_conectadas, (void *)_buscar_interfaz))
	{
		una_interfaz = list_find(interfaces_conectadas, (void *)_buscar_interfaz);
		return list_any_satisfy(una_interfaz->instrucciones_disponibles, (void *)_buscar_instruccion);
	}
	return false;	
}

interfaz* _traer_interfaz_solicitada(pcb *un_pcb)
{

	bool _buscar_interfaz(interfaz * una_interfaz)
	{
		return strcmp(un_pcb->pedido_a_interfaz->nombre_interfaz, una_interfaz->nombre_interfaz) == 0;
	}

	interfaz *una_interfaz = NULL;
	// Directamente se hace list_finde porque ya evalué si existía antes y estoy entre mutex
	return una_interfaz = list_find(interfaces_conectadas, (void *)_buscar_interfaz);
}

void control_request_de_interfaz(interfaz* una_interfaz){
	while(1){
		
		// Se le envía signal solamente cuando algún proceso hace la request del recurso
		sem_wait(&una_interfaz->sem_request_interfaz);

		// Se le envía señal cada vez que se libera interfaz
		sem_wait(&una_interfaz->sem_interfaz);

		pcb* un_pcb = NULL;

		pthread_mutex_lock(&una_interfaz->mutex_interfaz);
		un_pcb = list_remove(una_interfaz->lista_procesos_en_cola,0);
		pthread_mutex_unlock(&una_interfaz->mutex_interfaz);

		resultado_operacion resultado_de_operacion = solicitar_instruccion_a_interfaz(un_pcb,una_interfaz);
		switch(resultado_de_operacion){

			case OK:
			
				if(_eliminar_pcb_de_lista_sync(un_pcb,blocked,&mutex_lista_blocked)){
					list_clean(un_pcb->pedido_a_interfaz->datos_auxiliares_interfaz);
					un_pcb->pedido_a_interfaz->instruccion_a_interfaz = INSTRUCCION_IO_NO_DEFINIDA;
					un_pcb->pedido_a_interfaz->nombre_interfaz = NULL;

					agregar_a_ready(un_pcb);
					sem_post(&sem_pcp);
				}
				
			break;

			case ERROR:
				
				// Lo hago de esta manera para la lógica de liberación de recursos, porque ya no está en la lista de la interafaz
				list_clean(un_pcb->pedido_a_interfaz->datos_auxiliares_interfaz);
				un_pcb->pedido_a_interfaz->instruccion_a_interfaz = INSTRUCCION_IO_NO_DEFINIDA;
				un_pcb->pedido_a_interfaz->nombre_interfaz = NULL;
				
				planificar_proceso_exit_en_hilo(un_pcb);

			break;
		}
		
	}

}