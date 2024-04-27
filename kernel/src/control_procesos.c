#include "../include/control_procesos.h"

pcb* crear_pcb(char* path, char* size, char* prioridad){
	pcb* nuevo_PCB = malloc(sizeof(pcb));
	pthread_mutex_lock(&mutex_pid);
	identificador_PID++;
	nuevo_PCB->pid = identificador_PID;
	pthread_mutex_unlock(&mutex_pid);
	
    nuevo_PCB->program_counter = 0;
//    nuevo_PCB->quantum = TAM_QUANTUM;

	nuevo_PCB->registros_CPU = malloc(sizeof(registrosCPU));
	nuevo_PCB->registros_CPU->AX = 0;
	nuevo_PCB->registros_CPU->BX = 0;
	nuevo_PCB->registros_CPU->CX = 0;
	nuevo_PCB->registros_CPU->DX = 0;

	return nuevo_PCB;
}



//