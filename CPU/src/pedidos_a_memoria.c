#include "CPU.h"

char* MOV_IN_memoria(pid_t pid, uint32_t direccion_f, uint32_t tam_registro) 
{ 
	// paso parametros a string
	char* pid_str = string_itoa((int)pid);
    char* direccion_f_str = string_itoa(direccion_f);

	// los sumo a la lista
	t_list* parametros = list_create();
	list_add(parametros, pid_str);
	list_add(parametros, direccion_f_str);
	list_add(parametros, string_itoa((int)tam_registro));

	// creo la instruccion y hago el pedido a memoria
    t_instruccion* instruccion_pedido = crear_instruccion(MOV_IN, parametros);
    pedir_memoria(instruccion_pedido, conexion_con_memoria, logger);
	t_paquete *paquete = recibir_paquete(conexion_con_memoria, logger);
	int offset = 0;
    t_instruccion* respuesta = crear_instruccion_para_el_buffer(paquete->buffer, &offset);
	
	return respuesta->parametros[0];
}

void MOV_OUT_memoria(pid_t pid, uint32_t direccion_f, char* valor_leido) 
{
	// paso parametros a string
	char* pid_str = string_itoa(pid);
    char* direccion_f_str = string_itoa(direccion_f);

	// los sumo a la lista
	t_list* parametros = list_create();
	list_add(parametros, pid_str);
	list_add(parametros, direccion_f_str);
	list_add(parametros, valor_leido);
    t_instruccion* instruccion_pedido = crear_instruccion(MOV_OUT, parametros);
    pedir_memoria(instruccion_pedido, conexion_con_memoria, logger);

	// creo la instruccion y hago el pedido a memoria
	t_paquete *paquete = recibir_paquete(conexion_con_memoria, logger);
    if(paquete->codigo_operacion == MOV_OUT) log_info(logger, "MOV_OUT exitoso");
    destruir_paquete(paquete);
	return;
}

