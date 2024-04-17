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