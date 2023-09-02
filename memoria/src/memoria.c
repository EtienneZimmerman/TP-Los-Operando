#include "memoria.h"


t_log* logger;
t_list* tablas_segmentos;
t_segmento segmento_cero;
t_segmento segmento_vacio;
int memoria_libre;
void* memoria_fisica;
t_list* lista_huecos;
t_algoritmo algoritmo;

char*  puerto;
int  tamanio_memoria;
int  tamanio_segmento_0;
int  cantidad_segmentos;
int  retardo_memoria;
int  retardo_compactacion;
char* algoritmo_asignacion;
char* ip_memoria;
t_list* procesos_en_memoria;
pthread_t hilo_de_escucha_kernel, hilo_de_escucha_cpu, hilo_de_escucha_fs;

int conexion_kernel;


int main(void) {
	
	logger = log_create(LOG_PATH, "LOGGER MEMORIA", 1, LOG_LEVEL_DEBUG);
    levantar_config(CONFIG_PATH);

    inicializarMemoria();

    int server_socket = iniciar_servidor(puerto, ip_memoria, logger);

    pthread_t hilo_escucha;	
	pthread_create(&hilo_escucha, NULL, escucha_general, (void*) &server_socket);
	pthread_join(hilo_escucha, NULL);
   
}

void escucha_general(void* conexion)
{
	escuchar(*(int*)conexion);
}
