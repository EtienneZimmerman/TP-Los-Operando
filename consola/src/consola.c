#include "consola.h"

#define MAX_LINE_LENGTH 1024 

int kernel_socket;
t_log *logger;
t_console_config console_config;

int main(int argc, char *argv[]){ // Argc es la cantidad de argumentos y argv son los argumentos. Esta forma es un estandar.
  //char *path_configuracion, char *path_pseudocodigo quiero que sean las entradas

  startUp(argv[1]); // Paso como parametro el archivo de configuracion
  //startUp("/home/utnso/Desktop/carpeta/tp-2023-1c-Los-Operandos-main/consola/cfg/consola.config");
  if (console_config.config == NULL) {
	    log_error(logger, "No se pudo leer el archivo de configuración.");
		  exit(EXIT_FAILURE);
	}else {
		log_info(logger,"No es null el archivo de config");
	}
	
  // Acá quiero que lea el archivo de pseudocódigo

  FILE *pseudocode_file = fopen(argv[2], "r");
  //FILE *pseudocode_file = fopen("/home/utnso/Desktop/carpeta/tp-2023-1c-Los-Operandos-main/consola/cfg/pseudo.codigo", "r");
  if (!pseudocode_file) {
    log_error(logger, "No se pudo abrir el archivo de pseudocódigo \n");
    exit(EXIT_FAILURE);
  }


  log_info(logger, "Consola iniciada");

  // parsear archivo pseudocodigo
  t_list *lista_instrucciones = parsear_pseudocodigo(pseudocode_file);


  if (lista_instrucciones == NULL)
  {
      terminar_programa();
      return 0;
  }

 

  kernel_socket = crear_conexion(console_config.kernelIP, console_config.kernelPort);

   if( enviar_instrucciones(kernel_socket, lista_instrucciones, logger)) {
        log_info(logger, "Se puedo enviar las instrucciones");
    }
    else {
        log_error(logger, "No mandé nada :c");
    }

  char mensaje[20];
  if(recv(kernel_socket, mensaje, sizeof(mensaje), 0)){
    log_info(logger, "motivo finalizacion de proceso: %s", mensaje);
  }

  // fin del programa
  terminar_programa();
  //list_destroy_and_destroy_elements(lista_instrucciones, (void*)destruir_instruccion);

  return EXIT_SUCCESS;

}

void startUp(char *path_configuracion){
	logger = log_create(LOG_PATH,"LOGGER CONSOLA", true, LOG_LEVEL_INFO); // Inicializo logger
  console_config.config = config_create(path_configuracion);
  console_config.kernelIP = config_get_string_value(console_config.config, "IP_KERNEL");
	console_config.kernelPort = config_get_string_value(console_config.config, "PUERTO_KERNEL");
}

void terminar_programa() {
		liberar_conexion(kernel_socket);
		log_destroy(logger);
		config_destroy(console_config.config);
}