#include "CPU.h"
t_log* logger;
t_config *config;


int pc = 0;
int tam_max_segmento;
int retardo_instruccion;

int conexion_con_memoria ;
char *ipMemoria;
char *puertoMemoria;
char *valorMemoria;
char *puerto_escucha;
char *ipCPU;

t_temporal* temporizador;

int main(){
	logger = iniciar_logger();

	config = iniciar_config();

	conexion_con_memoria = crear_conexion(ipMemoria, puertoMemoria);
	log_info(logger, "conecte con memoria");
	send(conexion_con_memoria, "hola, soy cpu", strlen("hola, soy cpu") + 1, 0);
	
	int server_fd = iniciar_servidor(puerto_escucha, ipCPU, logger);

	// espero a kernel

	pthread_t hilo_de_escucha;
	log_debug(logger, "Escuchando kernel");
	while(1){
		int socket_kernel = esperar_cliente(server_fd);
		pthread_create(&hilo_de_escucha, NULL, (void*)atender_kernel, (void*)socket_kernel);
		pthread_detach(hilo_de_escucha);
	} //escucha infinita 


}

t_log* iniciar_logger(void) {
	t_log *nuevo_logger;
	nuevo_logger = log_create(LOG_PATH,"LOGGER CPU", true, LOG_LEVEL_DEBUG);

	return nuevo_logger;
}

t_config* iniciar_config(void) {
	t_config *nuevo_config;
	nuevo_config = config_create(CONFIG_PATH);

	if(nuevo_config == NULL){
		log_error(logger,"error al leer el archivo cpu.config");
	}else {
			ipCPU = config_get_string_value(nuevo_config,"IP_CPU");
		    ipMemoria = config_get_string_value(nuevo_config,"IP_MEMORIA");
			puertoMemoria = config_get_string_value(nuevo_config,"PUERTO_MEMORIA");
			valorMemoria = config_get_string_value(nuevo_config,"CLAVE_MEMORIA");
			tam_max_segmento = config_get_int_value(nuevo_config,"TAM_MAX_SEGMENTO");
			retardo_instruccion = config_get_int_value(nuevo_config,"RETARDO_INSTRUCCION");
			puerto_escucha = config_get_string_value(nuevo_config, "PUERTO_ESCUCHA");
	}

	return nuevo_config;
}

void atender_kernel(void* conexion)
{
	int conexion_kernel = (int)conexion;
	log_info(logger, "escuchando al kernel");
	t_pcb* pcb = recibir_pcb(conexion_kernel, logger);
	log_info(logger, "recibi pcb");

	temporizador = temporal_create(); 
	comenzar_ciclo_de_instruccion(pcb, conexion_kernel); 
	close(conexion_kernel);
	destruir_pcb(pcb);
}
