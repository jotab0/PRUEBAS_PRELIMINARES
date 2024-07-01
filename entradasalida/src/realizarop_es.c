#include "../include/realizarop_es.h"


t_dictionary *listaFcbs;

void atender_es_kernel(){

    while(1){
		t_buffer* unBuffer;
		int cod_op = recibir_operacion(fd_kernel);

    switch (cod_op) {

		case IO_GEN_SLEEP:
            unBuffer = recibir_un_paquete(fd_kernel);
            atender_peticion_de_gen(unBuffer);
			break; 
        
        case IO_STDIN_READ:
            unBuffer = recibir_un_paquete(fd_kernel);
            atender_peticion_de_stdin(unBuffer);
            break;

        case IO_STDOUT_WRITE:
            unBuffer = recibir_un_paquete(fd_kernel);
            atender_peticion_de_stdout(unBuffer);
            break;        

        case IO_FS_CREATE:
            unBuffer = recibir_un_paquete(fd_kernel);
            //atender_peticion_de_fs_create(unBuffer);
			break; 

        case IO_FS_READ:
            unBuffer = recibir_un_paquete(fd_kernel);
            //atender_peticion_de_fs_read(unBuffer);
			break; 

        case IO_FS_WRITE:
            unBuffer = recibir_un_paquete(fd_kernel);
            //atender_peticion_de_fs_write(unBuffer);
			break; 

        case IO_FS_TRUNCATE:
            unBuffer = recibir_un_paquete(fd_kernel);
            //atender_peticion_de_fs_truncate(unBuffer);
			break; 

        case IO_FS_DELETE:
            unBuffer = recibir_un_paquete(fd_kernel);
            //atender_peticion_de_fs_delete(unBuffer);
			break; 

        default:
			log_warning(es_logger, "Operacion desconocida");
			free(unBuffer);
			break;
		}
    }
}

//------GEN-------------------------------------------------------

void atender_peticion_de_gen(t_buffer* un_buffer){

    char* nombre_interfaz = extraer_string_del_buffer(un_buffer);
    int pid = extraer_int_del_buffer(un_buffer);
    int un_tiempo = extraer_int_del_buffer(un_buffer);


    usleep(un_tiempo*1000);

    respuesta_de_operacion_gen_a_kernel(nombre_interfaz, pid);

    free(un_buffer);
}

//--------------STDIN--------------------------------------------------

void atender_peticion_de_stdin(t_buffer* un_buffer){

    char* nombre_interfaz = extraer_string_del_buffer(un_buffer);
    int pid = extraer_int_del_buffer(un_buffer);
    int32_t una_direccion = extraer_int_del_buffer(un_buffer);
    int tamanio_direcc = extraer_int_del_buffer(un_buffer);
    
    char contenido_ingresado[256];

    log_info(es_logger, "Ingrese el texto:  <%s>", contenido_ingresado);
    fgets(contenido_ingresado, sizeof(contenido_ingresado), stdin);

    // Remover el salto de línea si es que existe
    size_t tam_contenido_ingresado = strlen(contenido_ingresado);
    if (tam_contenido_ingresado > 0 && contenido_ingresado[tam_contenido_ingresado-1] == '\n') {
        contenido_ingresado[tam_contenido_ingresado-1] = '\0';
    }

    tam_contenido_ingresado = strlen(contenido_ingresado); // Volver a calcular el tamaño después de sacar el salto de línea

    // Compara el tamaño de la entrada con el tamaño de la dirección física
    if (tam_contenido_ingresado > tamanio_direcc) {
        // Truncar el texto
        contenido_ingresado[tamanio_direcc] = '\0';
        log_info(es_logger, "Texto truncado a %d caracteres.\n", tamanio_direcc);
    } else if (tam_contenido_ingresado < tamanio_direcc) {
        // Lanzar un mensaje de error
        log_info(es_logger, "Error: El tamaño del texto ingresado (%zu) es menor al tamaño de la dirección física (%d).\n", tam_contenido_ingresado, tamanio_direcc);
        destruir_buffer(un_buffer);
        return;
    }

    enviar_contenido_a_memoria_stdin(pid, una_direccion, tamanio_direcc, contenido_ingresado);

    respuesta_de_operacion_stdin_a_kernel(nombre_interfaz, pid);
}

//----STDOUT------------------------------------------------------------
void atender_peticion_de_stdout(t_buffer* un_buffer) {

    char* nombre_interfaz = extraer_string_del_buffer(un_buffer);
    int pid = extraer_int_del_buffer(un_buffer);
    int32_t una_direccion = extraer_int_del_buffer(un_buffer);
    int tamanio_direcc = extraer_int_del_buffer(un_buffer);

    enviar_contenido_a_memoria_stdout(pid, una_direccion, tamanio_direcc);

    char* datos_leidos = recibir_datos_a_escribir_de_memoria(un_buffer);

    mostrar_datos_leidos(datos_leidos, tamanio_direcc);

    respuesta_de_operacion_stdout_a_kernel(nombre_interfaz, pid);
}



//-------------DIALFS---------------------------------------------------

/* void atender_peticion_de_fs_create(t_buffer* un_buffer){
	char* nombre_archivo = extraer_string_del_buffer(un_buffer);
    int pid = extraer_int_del_buffer(un_buffer);
	//char* operacion = recibir_string_del_buffer(un_buffer);

    crear_archivo(nombre_archivo);
	
}


void verificar_existencia_archivo(char *nombreArchivo) // para abrir archivo
{   
    log_abrir_archivo(nombreArchivo);
    //Si existe devolver OK
    if (dictionary_has_key(listaFcbs, nombreArchivo))
    {
        enviar_confirmacion_existencia_archivo();
        log_existe_archivo(nombreArchivo);
    }
    // Si no existe
    else
    {
        enviar_confirmacion_no_existencia_archivo();
        log_no_existe_archivo(nombreArchivo);
    }
    return;
}

t_fcb *crear_archivo(char *nombreArchivo)
{   
    bool archivoCreado;
    // Crear un archivo FCB(metadata) correspondiente al nuevo archivo, con tamaño 0 y sin bloques asociados.
    t_fcb* nuevoFcb = crear_nuevo_fcb(nombreArchivo);
    archivoCreado = crear_archivo_nuevo_fcb(nuevoFcb);
    if (archivoCreado)
    {   
        enviar_confirmacion_archivo_creado();
        log_crear_archivo(nombreArchivo);
    }
    return nuevoFcb;
}

void atender_peticion_de_fs_read(t_buffer* un_buffer){

	char* nombre_archivo = extraer_string_del_buffer(un_buffer);
	int pid = extraer_int_del_buffer(un_buffer);
	int una_direccion = extraer_int_del_buffer(un_buffer);
    int tamanio = extraer_int_del_buffer(un_buffer);
	int puntero = extraer_int_del_buffer(un_buffer);

	
}

void atender_peticion_de_fs_write(t_buffer* un_buffer){

	char* nombre_archivo = extraer_string_del_buffer(un_buffer);
	int pid = extraer_int_del_buffer(un_buffer);
	int una_direccion = extraer_int_del_buffer(un_buffer);
    int tamanio = extraer_int_del_buffer(un_buffer);
	int puntero = extraer_int_del_buffer(un_buffer);

}

void atender_peticion_de_fs_truncate(t_buffer* un_buffer){
	char* nombre_archivo = extraer_string_del_buffer(un_buffer);
	int pid = extraer_int_del_buffer(un_buffer);
	int tamanio_nuevo = extraer_int_del_buffer(un_buffer);

	
}

void atender_peticion_de_fs_delete(t_buffer* un_buffer){
	char* nombre_archivo = extraer_string_del_buffer(un_buffer);
    int pid = extraer_int_del_buffer(un_buffer);

} */



//-----RESPUESTAS DE OPERACIONES PARA KERNEL ------------------------------------

void  respuesta_de_operacion_gen_a_kernel(char* nombre_interfaz, int pid){
    t_paquete* un_paquete = crear_paquete_con_buffer(RESPUESTA_ES_GEN_ESK);

    cargar_string_a_paquete(un_paquete, nombre_interfaz);
    cargar_int_a_paquete(un_paquete, pid);
    //cargar_int_a_paquete(un_paquete, resultado);
    enviar_paquete(un_paquete, fd_kernel);
    eliminar_paquete(un_paquete);
}

 void respuesta_de_operacion_stdin_a_kernel(char* nombre_interfaz, int pid){
    t_paquete* un_paquete = crear_paquete_con_buffer(RESPUESTA_ES_STDIN_ESK);

    cargar_string_a_paquete(un_paquete, nombre_interfaz);
    //cargar_int_a_paquete(un_paquete, resultado);
    enviar_paquete(un_paquete, fd_kernel);
    eliminar_paquete(un_paquete);
 }

 void respuesta_de_operacion_stdout_a_kernel(char* nombre_interfaz, int pid){
    t_paquete* un_paquete = crear_paquete_con_buffer(RESPUESTA_ES_STDOUT_ESK);

    cargar_string_a_paquete(un_paquete, nombre_interfaz);
    //cargar_int_a_paquete(un_paquete, resultado);
    enviar_paquete(un_paquete, fd_kernel);
    eliminar_paquete(un_paquete);
 }

//----- ENVIAR A MEMORIA -----------------------------------

void enviar_contenido_a_memoria_stdin(int pid, int una_direccion,int tam_contenido_ingresado, char* contenido_ingresado){
    t_paquete* un_paquete = crear_paquete_con_buffer(ENVIO_RECURSOS_STDIN_ESM);
	cargar_int_a_paquete(un_paquete,pid);
	cargar_int_a_paquete(un_paquete,una_direccion);
    cargar_int_a_paquete(un_paquete, tam_contenido_ingresado);
	cargar_string_a_paquete(un_paquete, contenido_ingresado);
	enviar_paquete(un_paquete, fd_memoria);
	eliminar_paquete(un_paquete);
}

void enviar_contenido_a_memoria_stdout(int pid, int una_direccion, int tamanio_direcc){
    t_paquete* un_paquete = crear_paquete_con_buffer(ENVIO_RECURSOS_STDOUT_ESM);
	cargar_int_a_paquete(un_paquete,pid);
	cargar_int_a_paquete(un_paquete,una_direccion);
    cargar_int_a_paquete(un_paquete, tamanio_direcc);
	enviar_paquete(un_paquete, fd_memoria);
	eliminar_paquete(un_paquete);
}

//----- RECIBIR DE MEMORIA -----------------------------------

char* recibir_datos_a_escribir_de_memoria(t_buffer* un_buffer) {
    return extraer_string_del_buffer(un_buffer);
}



//---------------------------------------------------------------------


void mostrar_datos_leidos(char* datos_leidos, int tamanio_direcc) {
    // Calcular el tamaño total necesario para el mensaje concatenado
    size_t tamanio_total = 0;
    size_t pos = 0;
    
    while (pos < tamanio_direcc) {
        size_t mensaje_len = strlen(datos_leidos + pos);
        tamanio_total += mensaje_len;
        pos += mensaje_len + 1; // +1 para saltar el carácter nulo
    }
    
    // Crear un buffer para el mensaje concatenado, incluyendo espacios y el carácter nulo final
    char* mensaje_concatenado = malloc(tamanio_total + 1 + (tamanio_total - 1)); // Espacios entre mensajes y carácter nulo
    if (mensaje_concatenado == NULL) {
        log_error(es_logger, "Error al mostrar datos leidos de memoria.\n");
        return;
    }

    // Concatena los mensajes
    mensaje_concatenado[0] = '\0'; 
    pos = 0;
    
    while (pos < tamanio_direcc) {
        size_t mensaje_len = strlen(datos_leidos + pos);
        if (pos != 0) {
            strcat(mensaje_concatenado, " "); // Añadir un espacio entre mensajes
        }
        strcat(mensaje_concatenado, datos_leidos + pos);
        pos += mensaje_len + 1; 
    }

    log_info(es_logger, "%s\n", mensaje_concatenado);

    free(mensaje_concatenado);
}