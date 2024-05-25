#include "../include/comunicaciones_cpu.h"

void iterator(char* value){
	log_info(cpu_logger,"%s",value);
}


void esperar_kernel_cpu_interrupt(){
    int estado_while = 1;
    while (estado_while) {
		log_trace(cpu_logger,"CPU INTERRUPT: ESPERANDO MENSAJES DE KERNEL...");
        int cod_op = recibir_operacion(fd_kernel_interrupt);
		switch (cod_op) {
		/*
		case MENSAJE:
			recibir_mensaje_tp0(fd_kernel_interrupt, cpu_logger);
			break;
		case PAQUETE:
			break;
		*/
		case INTERRUPCION:
			t_buffer* unBuffer = recibir_buffer(fd_kernel_interrupt);
			atender_interrupcion(unBuffer);
			destruir_buffer(unBuffer);
			break;
		/*case INTERRUPCION_IO:
			t_buffer* unBuffer = recibir_buffer(fd_kernel_interrupr);
			atender_interrupcion_IO(unBuffer);
			destruir_buffer(unBuffer);
			break;
		*/
		case -1:
			log_error(cpu_logger, "KERNEL se desconecto de cpu interrupt. Terminando servidor");
			estado_while = 0;
            break;
		default:
			log_warning(cpu_logger,"Operacion desconocida de KERNEL (Interrupt)");
			break;
		}
	}
}

void esperar_kernel_cpu_dispatch(){
    int estado_while = 1;
    while (estado_while) {
		log_trace(cpu_logger,"CPU DISPATCH: ESPERANDO MENSAJES DE KERNEL...");
        int cod_op = recibir_operacion(fd_kernel_dispatch);
		switch (cod_op) {
		/*
		case MENSAJE:
		 	recibir_mensaje_tp0(fd_kernel_dispatch,cpu_logger);
			break;
		case PAQUETE:
			break;
		*/
		case -1:
			log_error(cpu_logger, "KERNEL se desconecto de cpu dispatch. Terminando servidor");
			estado_while = 0;
            break;
		default:
			log_warning(cpu_logger,"Operacion desconocida de KERNEL (Distaptch)");
			break;
		}
	}
}

void esperar_memoria_cpu(){
    int estado_while = 1;
    while (estado_while) {
		log_trace(cpu_logger,"CPU: ESPERANDO MENSAJES DE MEMORIA");
        int cod_op = recibir_operacion(fd_memoria);
		switch (cod_op) {
		case SOLICITUD_INSTRUCCION:
			t_buffer* unBuffer = recibir_buffer(fd_memoria);
			recibir_instruccion(unBuffer);
			destruir_buffer(unBuffer);
			break;
		case -1:
			log_error(cpu_logger, "MEMORIA se desconecto. Terminando servidor");
			estado_while= 0;
            break;
		default:
			log_warning(cpu_logger,"Operacion desconocida de MEMORIA");
			break;
		}
	}
}

// atender memoria

void recibir_instruccion(t_buffer* unBuffer){
	char* delimitador = " ";
	char* instruccion = extraer_string_del_buffer(unBuffer);
	log_warning(cpu_logger, "Instruccion recibida: [%s]", instruccion);
	// string split divide una cadena en subcadenas (para dividir el cod_op de los operadores?)
	//semaforo mutex
	char** instruccion_dividida = string_split(instruccion, delimitador);
	free(instruccion);
	// semaforo signal pata indicarle a el ciclo de instruccion que ya recibio la instruccion
}

// atender kernel
/*void solicitar_instruccion_de_memoria(){
	log_info(cpu_log_obligatorio, "PID: <%d> - Program Counter: <%d>", contexto->proceso_pid, contexto->proceso_pc);
	t_paquete* un_paquete = crear_paquete_con_buffer(SOLICITUD_INSTRUCCION);
	cargar_int_a_paquete(un_paquete, contexto->proceso_pid);
	cargar_int_a_paquete(un_paquete, contexto->proceso_pc);
	enviar_paquete(un_paquete, fd_memoria);
	eliminar_paquete(un_paquete);
}
*/

void mostrar_pcb(){
	log_warning(cpu_logger, "[PID: %d] [PC: %d] [TIEMPO EJECUTADO: %u] [REGISTROS: %u|%u|%u|%u]",
	contexto->proceso_pid,
	contexto->proceso_pc,
	contexto->proceso_tiempo_ejecutado,
	contexto->AX,
	contexto->BX,
	contexto->CX,
	contexto->DX);
}

void recibir_pcb_del_kernel(t_buffer* unBuffer){
	pthread_mutex_lock(&mutex_manejo_contexto);
	iniciar_estructuras_para_recibir_pcb(unBuffer);
	pthread_mutex_unlock(&mutex_manejo_contexto);

	mostrar_pcb();

	//log_info(cpu_logger, "PID del proceso antes del WHILE, %d", contexto->proceso_pid);
}

void iniciar_estructuras_para_recibir_pcb(t_buffer* unBuffer){
	contexto = malloc(sizeof(t_contexto));

	contexto->proceso_pid = extraer_int_del_buffer(unBuffer);
	contexto->proceso_pc=extraer_int_del_buffer(unBuffer);
	contexto->proceso_tiempo_ejecutado=extraer_int_del_buffer(unBuffer);
	contexto->proceso_ticket=extraer_int_del_buffer(unBuffer);
	
	contexto->AX = extraer_uint32_del_buffer(unBuffer);
	contexto->BX = extraer_uint32_del_buffer(unBuffer);
	contexto->CX = extraer_uint32_del_buffer(unBuffer);
	contexto->DX = extraer_uint32_del_buffer(unBuffer);

	free(unBuffer);
}

void atender_interrupcion(t_buffer* unBuffer){
	// mutex
	hay_interrupcion = true;
	// si llegan varias interrupciones ?
	// que cada tipo de interrupcion sea un case y tenga una flag distinta?
}

/*
void atender_interrupcion_IO(t_buffer* unBuffer){
	hay_interrupcion_IO = true;
}
*/


char** string_split(char* instruccion, char* delimitador){
	// contar numero delimitadores
	int num_tokens = contar_delimitadores(instruccion, delimitador);

	//asignar memoria para el array de punteros a char
	char** tokens = malloc((num_tokens) * sizeof(char*));
	if(!tokens){
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	//hacer copia de la cadena original
	char* instruccion_copy = strdup(instruccion);
	if(!instruccion_copy){
		perror("strdup");
		exit(EXIT_FAILURE);
	}

	// dividir cadena y guardar los tokens
	char* token = strtok(instruccion_copy, delimitador);
	int indice = 0;
	while (token != NULL){
		tokens[indice] = strdup(token); //copia cada token
		if (!tokens[indice]){
			perror("strdup");
			exit(EXIT_FAILURE);
		}
		indice++;
		token = strtok(NULL, delimitador);
	}
	free(instruccion_copy);
	return tokens;
}


int contar_delimitadores(char* instruccion,char* delimitador){
	int num_tokens = 0;
	char* instruccion_copy = strdup(instruccion); // crear una copia de la cadena original
	if(!instruccion_copy){
		perror("strdup");
		exit(EXIT_FAILURE);
	}

	char* token = strtok(instruccion_copy, delimitador);
	while(token != NULL){
		num_tokens++;
		token = strtok(NULL, delimitador);
	}
	free(instruccion_copy);
	return num_tokens;
}


