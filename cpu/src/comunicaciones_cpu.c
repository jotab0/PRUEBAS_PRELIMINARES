#include "../include/comunicaciones_cpu.h"

void iterator(char *value)
{
	log_info(cpu_logger, "%s", value);
}

void esperar_kernel_cpu_interrupt()
{
	int estado_while = 1;
	while (estado_while)
	{
		log_trace(cpu_logger, "CPU INTERRUPT: ESPERANDO MENSAJES DE KERNEL...");
		int cod_op = recibir_operacion(fd_kernel_interrupt);
		switch (cod_op)
		{
		/*
		case MENSAJE:
			recibir_mensaje_tp0(fd_kernel_interrupt, cpu_logger);
			break;
		case PAQUETE:
			break;
		*/
		case INTERRUPCION:
			t_buffer *unBuffer = recibir_buffer(fd_kernel_interrupt);
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
			log_warning(cpu_logger, "Operacion desconocida de KERNEL (Interrupt)");
			break;
		}
	}
}

void esperar_kernel_cpu_dispatch()
{
	int estado_while = 1;
	while (estado_while)
	{
		log_trace(cpu_logger, "CPU DISPATCH: ESPERANDO MENSAJES DE KERNEL...");
		int cod_op = recibir_operacion(fd_kernel_dispatch);
		switch (cod_op)
		{
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
			log_warning(cpu_logger, "Operacion desconocida de KERNEL (Distaptch)");
			break;
		}
	}
}

void esperar_memoria_cpu()
{
	int estado_while = 1;
	while (estado_while)
	{
		log_trace(cpu_logger, "CPU: ESPERANDO MENSAJES DE MEMORIA");
		int cod_op = recibir_operacion(fd_memoria);
		switch (cod_op)
		{
		case SOLICITUD_INSTRUCCION:
			t_buffer *buffer1 = recibir_buffer(fd_memoria);
			recibir_instruccion(buffer1);
			destruir_buffer(buffer1);
			break;
		case SOLICITUD_CONSULTA_PAG:
			t_buffer *buffer2 = recibir_buffer(fd_memoria);
			marco = extraer_int_del_buffer(buffer2);
			destruir_buffer(buffer2);
			break;
		case SOLICITUD_INFO_MEMORIA:
			t_buffer *buffer3 = recibir_buffer(fd_memoria);
			tamanio_pagina = extraer_int_del_buffer(buffer3);
			destruir_buffer(buffer3);
			break;
		/*case SOLICITUD_ESCRITURA_MEMORIA_BLOQUE:
			t_buffer* buffer4 = recibir_buffer(fd_memoria);
			valor_marco = extraer_string_del_buffer(buffer4);
			destruir_buffer(buffer4);
			break;*/
		case RTA_AJUSTAR_TAMANIO:
			t_buffer *buffer5 = recibir_buffer(fd_memoria);
			resultado = extraer_int_del_buffer(buffer5);
			destruir_buffer(buffer5);
			break;
		case SOLICITUD_LECTURA_MEMORIA_BLOQUE:
			t_buffer *buffer6 = recibir_buffer(fd_memoria);
			char *literal;
			literal = extraer_string_del_buffer(buffer6);
			atender_lectura(literal);
			destruir_buffer(buffer6);
			break;
		case SOLICITUD_ESCRITURA_MEMORIA_BLOQUE:
			t_buffer *buffer7 = recibir_buffer(fd_memoria);
			char *literal2;
			literal2 = extraer_string_del_buffer(buffer7);
			atender_escritura(literal2);
			destruir_buffer(buffer7);
			break;
		case -1:
			log_error(cpu_logger, "MEMORIA se desconecto. Terminando servidor");
			estado_while = 0;
			break;
		default:
			log_warning(cpu_logger, "Operacion desconocida de MEMORIA");
			break;
		}
	}
}

// atender memoria

void atender_lectura(char* literal){
	if (respuesta_marco_lectura != NULL){
		free(respuesta_marco_lectura);
	}
	respuesta_marco_lectura = (char *)malloc((strlen(literal) + 1) * sizeof(char));
	if(respuesta_marco_lectura == NULL){
		fprintf(stderr, "Error de asignacion de memoria\n");
		exit(EXIT_FAILURE);
	}
	strcpy(respuesta_marco_lectura, literal);
}

void atender_escritura(char* literal2){
	if (respuesta_marco_escritura != NULL){
		free(respuesta_marco_escritura);
	}
	respuesta_marco_escritura = (char *)malloc((strlen(literal2) + 1) * sizeof(char));
	if(respuesta_marco_escritura == NULL){
		fprintf(stderr, "Error de asignacion de memoria\n");
		exit(EXIT_FAILURE);
	}
	strcpy(respuesta_marco_escritura, literal2);
}

void recibir_instruccion(t_buffer *unBuffer)
{
	char *delimitador = " ";
	char *instruccion = extraer_string_del_buffer(unBuffer);
	log_warning(cpu_logger, "Instruccion recibida: [%s]", instruccion);
	// string split divide una cadena en subcadenas (para dividir el cod_op de los operadores?)
	// semaforo mutex
	instruccion_dividida = string_split(instruccion, delimitador);
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

void mostrar_pcb()
{
	log_warning(cpu_logger, "[PID: %d] [PC: %d] [TIEMPO EJECUTADO: %u] [REGISTROS: %u|%u|%u|%u|%u|%u]",
				contexto->proceso_pid,
				contexto->proceso_pc,
				contexto->proceso_tiempo_ejecutado,
				contexto->AX,
				contexto->BX,
				contexto->CX,
				contexto->DX,
				contexto->SI,
				contexto->DI);
}

void recibir_pcb_del_kernel(t_buffer *unBuffer)
{
	pthread_mutex_lock(&mutex_manejo_contexto);
	iniciar_estructuras_para_recibir_pcb(unBuffer);
	pthread_mutex_unlock(&mutex_manejo_contexto);

	mostrar_pcb();

	// log_info(cpu_logger, "PID del proceso antes del WHILE, %d", contexto->proceso_pid);
}

void iniciar_estructuras_para_recibir_pcb(t_buffer *unBuffer)
{
	contexto = malloc(sizeof(t_contexto));

	contexto->proceso_pid = extraer_int_del_buffer(unBuffer);
	contexto->proceso_pc = extraer_int_del_buffer(unBuffer);
	contexto->proceso_tiempo_ejecutado = extraer_int_del_buffer(unBuffer);
	// contexto->proceso_ticket=extraer_int_del_buffer(unBuffer);

	contexto->AX = extraer_uint32_del_buffer(unBuffer);
	contexto->BX = extraer_uint32_del_buffer(unBuffer);
	contexto->CX = extraer_uint32_del_buffer(unBuffer);
	contexto->DX = extraer_uint32_del_buffer(unBuffer);
	contexto->SI = extraer_uint32_del_buffer(unBuffer);
	contexto->DI = extraer_uint32_del_buffer(unBuffer);

	free(unBuffer);
}

void atender_interrupcion(t_buffer *unBuffer)
{

	// verifico que el pid del proceso interrumpido sea el mismo que el del proceso actual, si lo es lo interrumpo y sino no hago nada
	int pid_interrumpido = extraer_int_del_buffer(unBuffer);
	motivo_interrupcion = extraer_int_del_buffer(unBuffer);

	if (pid_interrumpido == contexto->proceso_pid)
	{

		// hay_interrupcion = true;
	}
	if (motivo_interrupcion == 1)
	{ // QUANTUM_INTERRUPT
		// mutex
		hay_interrupcion_quantum = true;
	}
	else if (motivo_interrupcion == 2)
	{ // EXIT_PROCESS
		// mutex
		hay_interrupcion_exit = true;
	}
}

// que cada tipo de interrupcion sea un case y tenga una flag distinta?

/*
void atender_interrupcion_IO(t_buffer* unBuffer){
	hay_interrupcion_IO = true;
}
*/

char **string_split(char *instruccion, char *delimitador)
{
	// contar numero delimitadores
	int num_tokens = contar_delimitadores(instruccion, delimitador);

	// asignar memoria para el array de punteros a char
	char **tokens = malloc((num_tokens) * sizeof(char *));
	if (!tokens)
	{
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	// hacer copia de la cadena original
	char *instruccion_copy = strdup(instruccion);
	if (!instruccion_copy)
	{
		perror("strdup");
		exit(EXIT_FAILURE);
	}

	// dividir cadena y guardar los tokens
	char *token = strtok(instruccion_copy, delimitador);
	int indice = 0;
	while (token != NULL)
	{
		tokens[indice] = strdup(token); // copia cada token
		if (!tokens[indice])
		{
			perror("strdup");
			exit(EXIT_FAILURE);
		}
		indice++;
		token = strtok(NULL, delimitador);
	}
	free(instruccion_copy);
	return tokens;
}

int contar_delimitadores(char *instruccion, char *delimitador)
{
	int num_tokens = 0;
	char *instruccion_copy = strdup(instruccion); // crear una copia de la cadena original
	if (!instruccion_copy)
	{
		perror("strdup");
		exit(EXIT_FAILURE);
	}

	char *token = strtok(instruccion_copy, delimitador);
	while (token != NULL)
	{
		num_tokens++;
		token = strtok(NULL, delimitador);
	}
	free(instruccion_copy);
	return num_tokens;
}
