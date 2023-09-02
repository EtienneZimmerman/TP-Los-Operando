#include "kernel.h"

t_list* lista_instrucciones;

t_config *config;
t_log *logger;

t_list *cola_new;
t_list *cola_ready;
t_list *cola_running;
t_list *cola_blocked;
t_list *cola_exit;

int server_socket;
int cpu_socket;
int filesystem_socket;
int memoria_socket;

t_kernel_config kernel_config;

pthread_t threadCPU;
pthread_t threadMemoria;
pthread_t threadFileSystem; 

pthread_mutex_t procesosEnSistemaMutex; 

sem_t contador_multiprogramacion;
sem_t sem_nuevos;
sem_t sem_ready;

t_pcb* pcb_a_ejecutar;
t_list* cola_procesos; 


int main(void) {

	startUp();
	
	if (kernel_config.config == NULL) {
	    log_error(logger, "No se pudo leer el archivo de configuración.");
		return EXIT_FAILURE;	// Hacer funcion terminate_kernel(EXIT)
	}else {
		log_info(logger,"No es null el archivo de config");
	}
	
	hilo_planificador_largo_plazo();
	hilo_planificador_corto_plazo();

	server_socket = iniciar_servidor(kernel_config.listenPort, kernel_config.kernelIP, logger);

	memoria_socket = crear_conexion(kernel_config.memoryIP,kernel_config.memoryPort);
	log_info(logger, "conecte con memoria");
	send(memoria_socket, "hola, soy kernel", strlen("hola, soy kernel") + 1, 0);
	
	filesystem_socket = crear_conexion(kernel_config.filesystemIP,kernel_config.filesystemPort);
	log_info(logger, "conecte con filesystem");

	// escucho consola y recibo las instruccioneS 
	pthread_t hilo_de_escucha_consola;
	while(1){
		int socket_consola = esperar_cliente(server_socket);
		pthread_create(&hilo_de_escucha_consola, NULL, (void*)atender_consola, (void*)socket_consola);
		pthread_detach(hilo_de_escucha_consola);
	}

	
	log_destroy(logger);
	config_destroy(config);

	return EXIT_SUCCESS;
}


void startUp(void){

	// Inicializo logger
	logger = log_create(LOG_PATH,"kernel_logger", true, LOG_LEVEL_DEBUG);	
	
	// Inicializo archivo de configuracion.
	kernel_config =	levantar_config();
	// Creo todas las colas
	iniciar_colas();
	iniciar_semaforos();
	iniciar_recursos();
}

void terminate_kernel(int x)
{
	list_destroy(cola_new);
	list_destroy(cola_ready);
	list_destroy(cola_running);
	list_destroy(cola_blocked);
	list_destroy(cola_exit);

	//pthread_mutex_destroy(&execution_mutex);

	config_destroy(config);
	if (cpu_socket)
		close(cpu_socket);
	if (memoria_socket)
		close(memoria_socket);
	if (filesystem_socket)
		close(filesystem_socket);

	log_destroy(logger);

	switch (x)
	{
	case 1:
		exit(EXIT_FAILURE);
	case SIGINT:
		exit(EXIT_SUCCESS);
	}
}

