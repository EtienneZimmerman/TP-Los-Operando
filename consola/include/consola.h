#ifndef CONSOLA_H_
#define CONSOLA_H_

#include "utils-shared.h"
#define LOG_PATH "./cfg/consola.log"

extern int kernel_socket;
extern t_log *logger;
typedef struct t_console_config
{
	t_config *config;
	char *kernelIP;
    char *kernelPort;
} t_console_config;

extern t_console_config console_config;

void startUp(char *path_configuracion);
t_log* iniciar_logger(void);
t_config* iniciar_config(void);
void terminar_programa();

//************* INSTRUCCIONES  *************//

t_identificador buscar_identificador(char *identificador);
void agregar_tamanio_instruccion(t_list *parametros, t_instruccion *instruccion);
t_instruccion *cargar_instruccion(t_identificador identificador, t_list *parametros);
t_instruccion *guardar_linea(char *line);
t_list *parsear_pseudocodigo(FILE *pseudo_file);
void enviar_instrucciones_y_esperar( t_list *lista_instrucciones);
 
#endif /* CONSOLA_H_ */