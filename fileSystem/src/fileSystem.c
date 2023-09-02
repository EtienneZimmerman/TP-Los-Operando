#include "fileSystem.h"

#define MAX_LINE_LENGTH 1024 


t_log* logger;
t_config *config;
int conexionConMemoria;
char *ipMemoria;
char *puertoMemoria;
char *puertoEscucha;
char *path_superbloque;
char *path_bitmap;
char *path_bloques;
char *path_fcb;
int retardo_acceso_bloque;
int tamanio_bloque;
int cant_bloques; 
char* ipFs;

t_list* lista_fcbs;
t_superbloque* superbloque;
t_bitarray* bitmap;

int main(void) {

	iniciar_logger();
	iniciar_config();
	levantar_estructuras();
	log_info(logger, "Se levantaron correctamente las estructuras");
	conexionConMemoria= crear_conexion(ipMemoria, puertoMemoria);
	send(conexionConMemoria, "hola, soy filesystem", strlen("hola, soy filesystem") + 1, 0);
	log_info(logger, "conecte con memoria");


	//espero al kernel 
	int socket_servidor = iniciar_servidor(puertoEscucha, ipFs, logger);
	pthread_t hilo_de_escucha_filesystem;
	log_info(logger, "Servidor listo para recibir al cliente");
	while(1){
		int socket_kernel = esperar_cliente(socket_servidor);
		pthread_create(&hilo_de_escucha_filesystem, NULL, (void*)atender_kernel, (void*)socket_kernel);
		pthread_detach(hilo_de_escucha_filesystem);
	}


	terminar_programa(conexionConMemoria, logger, config);
	
	
	return EXIT_SUCCESS;
}

