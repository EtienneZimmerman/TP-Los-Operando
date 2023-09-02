#include "kernel.h"

pthread_t hilo_corto_plazo;
pthread_mutex_t mutex_pcb_a_ejecutar;

void hilo_planificador_corto_plazo()
{

    pthread_create(&hilo_corto_plazo, NULL, (void*)inicializar_planificador_corto, NULL);
	pthread_detach(hilo_corto_plazo);
}

void inicializar_planificador_corto()
{
   while(1) { 
    sem_wait(&sem_ready);
    if(! strcmp(kernel_config.schedulerAlgorithm, "FIFO")){
        ejecutar_con_fifo();
    }else if(! strcmp(kernel_config.schedulerAlgorithm, "HRRN")){

        ejecutar_con_hrrn();
    }else{
        log_error(logger, "algoritmo incorrecto !");
    }

    }
}

void ejecutar_con_fifo()
{
    t_pcb* pcb = sacar_de_listos();
    ejecutar(pcb);
}

void ejecutar(t_pcb* pcb)
{
    cpu_socket = crear_conexion(kernel_config.cpuIP,kernel_config.cpuPort);
	log_info(logger, "conecte con cpu");
    poner_en_ejecucion(pcb);
    enviar_pcb(pcb_a_ejecutar, cpu_socket, logger);
    recibir_pcb_de_cpu(cpu_socket);
    close(cpu_socket);
}

void poner_en_ejecucion(t_pcb* pcb)
{
    cambiar_estado(pcb, EJECUTANDO);
    pthread_mutex_lock(&mutex_pcb_a_ejecutar);
    pcb_a_ejecutar = pcb;
    pthread_mutex_unlock(&mutex_pcb_a_ejecutar);
    agregar_a_cola_procesos(pcb);
}


void ejecutar_con_hrrn()
{
    pthread_mutex_lock(&mutex_cola_listos);
    parar_tiempos();
    list_sort(cola_ready, (void*)mayor_response_ratio);
    reiniciar_tiempos();
    pthread_mutex_unlock(&mutex_cola_listos);
    t_pcb* pcb = sacar_de_listos();
    ejecutar(pcb);
}

void parar_tiempos()
{
    for(int i = 0; i < list_size(cola_ready); i++){
        t_pcb* pcb = list_get(cola_ready, i);
        temporal_stop(pcb->tiempo_llegada);
    }
}

void reiniciar_tiempos()
{
    for(int i = 0; i < list_size(cola_ready); i++){
        t_pcb* pcb = list_get(cola_ready, i);
        temporal_resume(pcb->tiempo_llegada);
    }
}

bool mayor_response_ratio(t_pcb* pcb1, t_pcb* pcb2)
{
    return response_ratio(pcb1) > response_ratio(pcb2);
}

double response_ratio(t_pcb* pcb)
{
    return (1 + temporal_gettime(pcb->tiempo_llegada)) / pcb->estimado_rafaga;
}

void actualizar_estimado_rafaga(t_pcb* pcb)
{
    // s = a * rafaga_ant + (1 - a) * estimado_ant
    double alfa = kernel_config.alpha;
    double estimado_nuevo = alfa * pcb->tiempo_ejecucion + (1- alfa) * pcb->estimado_rafaga;
    pcb->estimado_rafaga = estimado_nuevo;
}