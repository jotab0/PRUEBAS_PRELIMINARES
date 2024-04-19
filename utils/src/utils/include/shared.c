#include "shared.h"

int crear_conexion(char *ip, char* puerto){
	struct addrinfo hints; 
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int err = getaddrinfo(ip, puerto, &hints, &server_info);

	if (err != 0){
		printf("Error al crear conexion");
		exit(-2);
	}

	int socket_cliente = socket(server_info->ai_family,
		server_info->ai_socktype,
		server_info->ai_protocol);

	connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen); //conecta socket

	freeaddrinfo(server_info);

	return socket_cliente;
}

int iniciar_servidor(char* puerto,t_log* logger,char* mensaje) //Crea socket servidor, lo bindea y lo deja escuchando
{
	int socket_servidor;

	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, puerto, &hints, &servinfo); //De acá saca el puerto a donde se bindea el socket

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(
		servinfo->ai_family,
        servinfo->ai_socktype,
        servinfo->ai_protocol);
	// Asociamos el socket a un puerto
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);
	// Escuchamos las conexiones entrantes
	listen(socket_servidor,SOMAXCONN);

	freeaddrinfo(servinfo);
	log_trace(logger, "SERVER: %s",mensaje);

	return socket_servidor;
}

int esperar_cliente(int socket_servidor,t_log* logger,char* mensaje) 
{	
	// Aceptamos un nuevo cliente
	int socket_cliente;
	
	log_info(logger, "Esperando a %s", mensaje);

	socket_cliente = accept(socket_servidor,NULL,NULL); //qieda esperando hasta que se conecte cliente

	log_info(logger, "!! Se conecto %s !!", mensaje);

	return socket_cliente;
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

////////////////////// COMUNICACIÓN  //////////////////////

// BUFFERS

t_buffer* crear_buffer() { // CREA BUFFER PARA QUE LLEVE DISTINTOS MENSAJES
	t_buffer* buffer = malloc(sizeof(t_buffer));
    buffer->size = 0;
	buffer->stream = NULL;
	return buffer;
}

void destruir_buffer(t_buffer* buffer) { // DESTRUYE BUFFER PARA QUE NO HAYA !!!!MEMORY LEAKS!!!!
    if (buffer->stream != NULL) {
        free(buffer->stream);
        buffer->stream = NULL;
    }
    free(buffer);
}

void cargar_mensaje_a_buffer(t_buffer* buffer, void* mensaje, int tam_mensaje) { // AGREGA EL CODIGO DE MSJE + MENSAJE A UN BUFFER EXISTENTE (Puede ya tener algo cargado o no)
	if(buffer -> size == 0){
		buffer -> stream = malloc(sizeof(int) + tam_mensaje); // VA SIEMRPE PRIMERO SIZEOF(INT) PORQUE ES EL TAMAÑO QUE VA A TENER SIEMPRE EL CÓDIGO DE COMUNICACIÓN
		memcpy(buffer -> stream,&tam_mensaje,sizeof(int));
		memcpy(buffer -> stream + sizeof(int),mensaje,tam_mensaje);
	}
	else {
   		buffer->stream = realloc(buffer->stream, buffer->size + sizeof(int) + tam_mensaje); // realloc() permite aumentar en tiempo de ejecución el tamaño de memoria dinámica ya pedido
    	memcpy(buffer->stream + buffer->size, &tam_mensaje, sizeof(int));
    	memcpy(buffer->stream + buffer->size + sizeof(int), mensaje, tam_mensaje);
	}
    buffer->size += sizeof(int);
    buffer->size += tam_mensaje;
}

void cargar_int_a_buffer(t_buffer* buffer, int valor) {  
    cargar_mensaje_a_buffer(buffer, &valor, sizeof(int));
}

void cargar_uint32_a_buffer(t_buffer* buffer, uint32_t valor) {
    cargar_mensaje_a_buffer(buffer, &valor, sizeof(uint32_t));
}

void cargar_string_a_buffer(t_buffer* buffer, char* string) {
    cargar_mensaje_a_buffer(buffer, string, strlen(string)+1);
}

void* extraer_mensaje_de_buffer(t_buffer* buffer){ //EXTRAE BUFFER DE A PARTES
	
	if(buffer -> size == 0){
		printf("ERROR: Se intento extraer_mensaje_de_buffer de buffer vacío");
		exit(EXIT_FAILURE);
	}

	if(buffer -> size < 0){
		printf("ERROR: Se intento extraer_mensaje_de_buffer pero el tamaño del mensaje es negativo");
		exit(EXIT_FAILURE);
	}

	int tam_mensaje;
	memcpy(&tam_mensaje,buffer->stream,sizeof(int));
	void* mensaje = malloc(tam_mensaje);
	memcpy(mensaje,buffer->stream + sizeof(int),tam_mensaje);

	int tam_nuevo = buffer->size - sizeof(int) - tam_mensaje;
	
	if (tam_nuevo == 0){
		buffer->size = 0;
		free(buffer->stream);
		buffer->stream = NULL;
		return mensaje;
	}

	if (tam_nuevo < 0){
		printf("ERROR: Buffer de tamaño negativo");
		exit(EXIT_FAILURE);
	}

	void* nuevo_stream = malloc(tam_nuevo);
	memcpy(nuevo_stream,buffer->stream+sizeof(int)+tam_mensaje,tam_nuevo);
	free(buffer->stream);
	buffer->size = tam_nuevo;
	buffer->stream = nuevo_stream;

	return mensaje;
}

int extraer_int_del_buffer(t_buffer* buffer){
	int* un_entero = extraer_mensaje_de_buffer(buffer);
	int valor_del_int = *un_entero;
	free(un_entero);
	return valor_del_int; 
}

char* extraer_string_del_buffer(t_buffer* buffer){
	char* string = extraer_mensaje_de_buffer(buffer);
	return string;
}

uint32_t extraer_uint32_del_buffer(t_buffer* buffer){
	uint32_t* un_entero = extraer_mensaje_de_buffer(buffer);
	uint32_t valor_del_uint32_t = *un_entero;
	free(un_entero);
	return valor_del_uint32_t; 
}


// PAQUETES

t_paquete* crear_paquete_con_buffer(op_code codigo_operacion, t_buffer* buffer){ // EL PAQUETE ADEMAS DE LLEVAR INFORMACION (COMO EL BUFFER) LLEVA EL CODIGO DE OPERACION
	t_paquete* nuevo_paquete = malloc(sizeof(t_paquete));
	nuevo_paquete -> codigo_operacion = codigo_operacion;
	nuevo_paquete -> buffer = buffer;
	return nuevo_paquete;
}

void destruir_paquete(t_paquete* paquete){
	destruir_buffer(paquete->buffer);
	free(paquete);
}

void* serializar_paquete(t_paquete* paquete){
	int tam_puntero = paquete->buffer->size + 2*sizeof(int); // POR LOS 2 sizeof(int) QUE AGREGO MÁS ABAJO
	void* puntero = malloc(sizeof(tam_puntero));
	int desplazamiento = 0;

	memcpy(puntero+desplazamiento,&(paquete->codigo_operacion),sizeof(int)); // COPIO EL CODIGO DE OPERACION
	desplazamiento += sizeof(int);
	memcpy(puntero+desplazamiento,&(paquete->buffer->size),sizeof(int)); // COPIO EL TAMAÑO DEL BUFFER
	desplazamiento += sizeof(int);
	memcpy(puntero+desplazamiento,&(paquete->buffer->stream),paquete->buffer->size); // COPIO EL STREAM

	return puntero; 
}

void enviar_paquete(t_paquete* paquete, int fd){ // RECIBE PAQUETE Y FILE DESCRIPTOR DE CONEXION
	void* a_enviar = serializar_paquete(paquete);

	int bytes = paquete->buffer->size + 2*sizeof(int); // LE AGREGA *2 int POR LOS QUE LE AGREGO AL SERIALZIAR
	send(fd, a_enviar, bytes, 0);

	free(a_enviar);
}

// LUEGO CADA UNO SE FIJA COMO LO EXTRAE, YA QUE DEPENDERÁ DEL CODIGO DE OPERACIÓN


// BUFFER 				|-> STREAM + SIZE
// PAQUETE 				|-> CODIGO DE OPERACION + BUFFER (STREAM + SIZE)

// ======== Serializar:

// PAQUETE SERIALZIADO 	|-> CODIGO DE OPERACION + SIZE + STREAM

// Como el size viene antes que el stream:

void* recibir_buffer(int socket_cliente)
{
	void* buffer;
	int* size = 0;
	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size); 									// RESERVO MEMORIA DEL TAMAÑO DEL SIZE PARA BUFFER
	recv(socket_cliente, buffer, *size, MSG_WAITALL);			// RECIVO EL STREAM

	return buffer; // SOLAMENTE TIENE EL STREAM
}