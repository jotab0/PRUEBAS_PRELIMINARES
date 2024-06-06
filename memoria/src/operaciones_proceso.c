#include "../include/operaciones_proceso.h"


//--------------------------------------------------------------------------------------------

t_proceso* obtener_proceso_por_pid(int pid){
	bool _buscar_el_pid(t_proceso* proceso){
		return proceso->pid_proceso == pid;
	}
	t_proceso* un_proceso = list_find(lista_procesos, (void*)_buscar_el_pid);
	if(un_proceso == NULL){
		log_error(memoria_logger, "PID<%d> No encontrado en la lista de procesos", pid);
		exit(EXIT_FAILURE);
	}
	return un_proceso;
}
//--------------------------------------------------------------------------------------------


char*  extraer_instruccion_por_ip(t_proceso* proceso, int ip_proceso){

    if (ip_proceso < 0 ||ip_proceso >= list_size(proceso-> lista_de_instrucciones)) {
        log_error(memoria_logger, "PID<%d> - Índice de instrucción <%d> NO VALIDO", proceso->pid_proceso, ip_proceso);
        return NULL;
    }

    char* instruccion = list_get(proceso->lista_de_instrucciones, ip_proceso);
    return instruccion;
}

//---------------------------------------------------------------------------------------

void* ampliar_tamanio_proceso(int nuevo_tamanio,t_proceso* proceso){
	int tam_actual = proceso->size;

    log_info(memoria_logger,  "PID <%d> - Tamaño Actual: <%d> - Tamaño a Ampliar: <%d>" ,proceso->pid_proceso, tam_actual,nuevo_tamanio);
}

void reducir_tamanio_proceso(int nuevo_tamanio,t_proceso* proceso){
	int tam_actual = proceso->size;

	log_info(memoria_logger,  "PID <%d> - Tamaño Actual: <%d> - Tamaño a Reducir: <%d>" ,proceso->pid_proceso, tam_actual,nuevo_tamanio);

}