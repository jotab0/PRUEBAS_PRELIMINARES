#include "../include/realizarop_es.h"



configuracion_t cargar_configuracion(char* archivo_configuracion) {
    configuracion_t config;
    FILE *file = fopen(archivo_configuracion, "rt");
    if (file == NULL) {
        perror("Error al abrir el archivo de configuración");
        exit(EXIT_FAILURE);
    }

    fscanf(file, "TIPO_INTERFAZ=%s\n", config.tipo_interfaz);
    fscanf(file, "TIEMPO_UNIDAD_TRABAJO=%d\n", &config.tiempo_unidad_trabajo);
    fscanf(file, "IP_KERNEL=%s\n", config.ip_kernel);
    fscanf(file, "PUERTO_KERNEL=%d\n", &config.puerto_kernel);

    fclose(file);
    return config;
}

void enviar_handshake(int* fd_kernel, char* nombre_interfaz, char* tipo_interfaz) {
    t_paquete* paquete = crear_paquete(HANDSHAKE_K_ES);
    cargar_string_a_paquete(paquete, nombre_interfaz);
    cargar_string_a_paquete(paquete, tipo_interfaz);

    enviar_paquete(paquete, fd_kernel);

    eliminar_paquete(paquete);
}

void ejecutar_instruccion_pedida(){

    switch (instruccion_interfaz) {
		case IO_GEN_SLEEP:
            sleep(tiempo_unidad_trabajo);
			respuesta_de_operacion_realizada_a_kernel(int* fd_kernel, char* nombre_interfaz, int resultado_operacion);
			break; 

}
}

void  respuesta_de_operacion_realizada_a_kernel(int* fd_kernel, char* nombre_interfaz, int resultado_operacion){
    t_paquete* un_paquete = crear_paquete_con_buffer(RESPUESTA_INSTRUCCION_KES);
    cargar_string_a_paquete(un_paquete, nombre_interfaz);
    cargar_int_a_paquete(un_paquete, resultado_operacion);
    enviar_paquete(un_paquete, fd_kernel);
    eliminar_paquete(un_paquete);
}