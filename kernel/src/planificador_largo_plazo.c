#include "kernel.h"

pthread_t hilo_largo_plazo;


void hilo_planificador_largo_plazo(){
    pthread_create(&hilo_largo_plazo, NULL, (void*)inicializar_planificador_largo, NULL);
	pthread_detach(hilo_largo_plazo);
}

void inicializar_planificador_largo(){

   while(1) { 
    sem_wait(&contador_multiprogramacion);
    sem_wait(&sem_nuevos);
    t_pcb* pcb = sacar_de_nuevos();
    agregar_a_listos(pcb);
    log_info(logger, "ingreso proceso a listos");

    }
}

void terminar_proceso(t_pcb* pcb_actualizado, char* motivo){
    cambiar_estado(pcb_actualizado, TERMINADO);
    log_info(logger, "finaliza el proceso PID: %d - motivo: %s", pcb_actualizado->pid, motivo);
    sem_post(&contador_multiprogramacion);
    solicitud_memoria(pcb_actualizado->pid, ELIMINAR_PROCESO);
    sacar_de_cola_procesos(pcb_actualizado);
    send(pcb_actualizado->conexion, motivo, strlen(motivo), 0);
    destruir_pcb(pcb_actualizado);
}