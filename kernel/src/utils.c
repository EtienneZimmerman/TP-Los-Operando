
#include "kernel.h"

t_kernel_config levantar_config(){
	t_kernel_config kernel_config;
	kernel_config.config = config_create(CONFIG_PATH);
	kernel_config.cpuIP = config_get_string_value(kernel_config.config, "IP_CPU");
	kernel_config.memoryIP = config_get_string_value(kernel_config.config, "IP_MEMORIA");
	kernel_config.filesystemIP = config_get_string_value(kernel_config.config, "IP_FILESYSTEM");
	kernel_config.memoryPort = config_get_string_value(kernel_config.config, "PUERTO_MEMORIA");
	kernel_config.cpuPort = config_get_string_value(kernel_config.config, "PUERTO_CPU");
	kernel_config.filesystemPort = config_get_string_value(kernel_config.config, "PUERTO_FILESYSTEM");
	kernel_config.schedulerAlgorithm = config_get_string_value(kernel_config.config, "ALGORITMO_PLANIFICACION");
	kernel_config.listenPort = config_get_string_value(kernel_config.config, "PUERTO_ESCUCHA");
	kernel_config.initialEstimate = config_get_double_value(kernel_config.config, "ESTIMACION_INICIAL");
	kernel_config.alpha = config_get_double_value(kernel_config.config, "HRRN_ALFA");
	kernel_config.multiprogrammingLevel = config_get_int_value(kernel_config.config, "GRADO_MAX_MULTIPROGRAMACION");
	kernel_config.recursos = config_get_array_value(kernel_config.config, "RECURSOS");
	kernel_config.instancias_recursos = config_get_array_value(kernel_config.config, "INSTANCIAS_RECURSOS");
	kernel_config.kernelIP = config_get_string_value(kernel_config.config, "IP_KERNEL");
	return kernel_config;
}

void iniciar_colas(){
	cola_new = list_create();
    cola_ready = list_create();
    cola_running = list_create();
    cola_blocked = list_create();
	cola_procesos = list_create();
}


void iniciar_semaforos(){

	pthread_mutex_init(&mutex_cola_listos, NULL);
	pthread_mutex_init(&mutex_cola_bloqueados, NULL);
	pthread_mutex_init(&mutex_cola_nuevos, NULL);
	pthread_mutex_init(&mutex_pcb_a_ejecutar, NULL);
	pthread_mutex_init(&mutex_cambio_estado, NULL);
	pthread_mutex_init(&procesosEnSistemaMutex, NULL);
	

	sem_init(&sem_nuevos, 0, 0);
	sem_init(&sem_ready, 0, 0);

	sem_init(&contador_multiprogramacion, 0, kernel_config.multiprogrammingLevel);
	
	pcb_a_ejecutar = NULL;
}

