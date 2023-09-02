#include "kernel.h"

pthread_mutex_t mutex_cola_listos;
pthread_mutex_t mutex_cola_bloqueados;
pthread_mutex_t mutex_cola_nuevos;
pthread_mutex_t mutex_cambio_estado;

void agregar_a_listos(t_pcb* pcb)
{   
    cambiar_estado(pcb, LISTO);
    pthread_mutex_lock(&mutex_cola_listos);
    list_add(cola_ready, pcb);
    pcb->tiempo_llegada = temporal_create();
    pthread_mutex_unlock(&mutex_cola_listos);
    agregar_a_cola_procesos(pcb);
    char pid[20];
    strcpy(pid, "");
    for(int i=0; i < list_size(cola_procesos); i++){
        t_pcb* pcb_aux = list_get(cola_procesos, i);
        strcat(pid, string_itoa(pcb_aux->pid));
        if(i == list_size(cola_procesos)){
            break;
        }
        strcat(pid, ", ");
    }
    log_info(logger, "Cola ready %s [%s]", kernel_config.schedulerAlgorithm, pid);
    sem_post(&sem_ready);
    //log_info(logger, "Cola Ready %d:  %???", kernel_config.schedulerAlgorithm, ???)
}

t_pcb* sacar_de_listos()
{
    t_pcb* pcb;
    pthread_mutex_lock(&mutex_cola_listos);
    pcb = list_remove(cola_ready, 0);
    temporal_stop(pcb->tiempo_llegada);
    pthread_mutex_unlock(&mutex_cola_listos);
    sacar_de_cola_procesos(pcb);

    return pcb;
}

void agregar_a_bloqueados(t_pcb* pcb)
{
    cambiar_estado(pcb, BLOQUEADO);
    pthread_mutex_lock(&mutex_cola_bloqueados);
    list_add(cola_blocked, pcb);
    pthread_mutex_unlock(&mutex_cola_bloqueados);
}

t_pcb* sacar_de_bloqueados()
{
    t_pcb* pcb;
    pthread_mutex_lock(&mutex_cola_bloqueados);
    pcb = list_remove(cola_blocked, 0);
    pthread_mutex_unlock(&mutex_cola_bloqueados);
    return pcb;
} 
void agregar_a_nuevos(t_pcb* pcb)
{
    cambiar_estado(pcb, NUEVO);
    pthread_mutex_lock(&mutex_cola_nuevos);
    list_add(cola_new, pcb);
    pthread_mutex_unlock(&mutex_cola_nuevos);
    agregar_a_cola_procesos(pcb);
    log_info(logger, "Se crea proceso %d en NEW", pcb->pid);
    sem_post(&sem_nuevos);
}

t_pcb* sacar_de_nuevos()
{
    t_pcb* pcb;
    pthread_mutex_lock(&mutex_cola_nuevos);
    pcb = list_remove(cola_new, 0);
    pthread_mutex_unlock(&mutex_cola_nuevos);
    sacar_de_cola_procesos(pcb);
    return pcb;
} 

void cambiar_estado(t_pcb* pcb, t_estado nuevo_estado)
{
    if(pcb->estado != nuevo_estado){
        log_info(logger, "PID: %d - Estado anterior: %s - Estado actual: %s", pcb->pid, pasar_a_string(pcb->estado), pasar_a_string(nuevo_estado));
    }
    pthread_mutex_lock(&mutex_cambio_estado);
    pcb->estado = nuevo_estado;
    pthread_mutex_unlock(&mutex_cambio_estado);
}

char* pasar_a_string(t_estado estado)
{
    switch(estado){
        case EJECUTANDO:
            return "EJECUTANDO";
        break;
        case LISTO:
            return "LISTO";
        break;
        case NUEVO:
            return "NUEVO";
        break;
        case BLOQUEADO:
            return "BLOQUEADO";
        break;
        case TERMINADO:
            return "TERMINANDO";
        break;
	    case SEG_FAULT:
            return "SEGMENTATION FAULT";
        break;
    }
}
void sacar_de_cola_procesos(t_pcb* proceso)
{
    pthread_mutex_lock(&procesosEnSistemaMutex);
    list_remove_element(cola_procesos, proceso);
    pthread_mutex_unlock(&procesosEnSistemaMutex);
    return;
}


void agregar_a_cola_procesos(t_pcb* proceso) 
{
    pthread_mutex_lock(&procesosEnSistemaMutex);
    list_add(cola_procesos, proceso); 
    pthread_mutex_unlock(&procesosEnSistemaMutex);
}