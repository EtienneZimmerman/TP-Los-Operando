#include "fileSystem.h"


void atender_kernel(void* socket_kernel)
{
	int conexion_kernel = (int)socket_kernel;
 while(1) {	
	
	t_paquete* paquete = recibir_paquete(conexion_kernel, logger);
	t_pcb* pcb = deserializar_buffer_pcb(paquete->buffer, logger);
	t_instruccion* instruccion = obtener_instruccion(pcb);
	switch(instruccion->identificador){
		case F_OPEN: 
			bool abrioElArchivo = abrir_archivo(instruccion->parametros[0]);
			if(abrioElArchivo){
				send(conexion_kernel, "abierto", strlen("abierto") + 1, 0);
				log_info(logger, "Abrir Archivo: %s", instruccion->parametros[0]);
			}
			else{
				send(conexion_kernel, "no abierto", strlen("no abierto"), 0);
				uint32_t offset = 0;
				t_paquete* paquete = recibir_paquete(conexion_kernel, logger);
				t_instruccion* instruccion = crear_instruccion_para_el_buffer(paquete->buffer, &offset);
				crear_archivo(instruccion->parametros[0]);
				send(conexion_kernel, "creado", strlen("creado"), 0);
			}

		break;
		case F_READ: 
			log_info(logger , "Leer Archivo: %s - Puntero: %s - Memoria: %d - Tamaño: %s", instruccion->parametros[0], instruccion->parametros[1], pcb->direccion_fisica, instruccion->parametros[2]);
			leer_archivo(instruccion->parametros[0], atoi(instruccion->parametros[1]), atoi(instruccion->parametros[2]), pcb->direccion_fisica, pcb->pid);
    		send(conexion_kernel, "SI", strlen("SI"), 0);
		break;
		case F_WRITE: 			
			escribir(pcb, conexion_kernel);
			
		break;
		case F_TRUNCATE:
			truncar_archivo(instruccion->parametros[0], atoi(instruccion->parametros[1]));
			send(conexion_kernel, "truncado", strlen("truncado"), 0);
		break;	
	}

		destruir_pcb(pcb);
	
	}

	close(socket_kernel);
}

char pedir_a_memoria(t_instruccion* instruccion, int conexionConMemoria){
	t_buffer *buffer = malloc(sizeof(t_buffer));
	buffer = crear_buffer_para_t_instruccion(instruccion);
	t_paquete* paquete = crear_paquete(buffer, instruccion->identificador);
	enviar_paquete(conexionConMemoria, paquete, logger);

	// recibimos valor de memoria
	recv(conexionConMemoria, &(paquete->codigo_operacion), sizeof(uint8_t), 0);

	// deserializamos paquete
	char valor;
	void* stream = paquete->buffer->stream;
	memcpy(&(valor), stream, sizeof(char));

	return valor;
}

void mandar_a_memoria(char* valor, t_direc_fisica* direccion, int socketMemoria, t_instruccion *instruccion){

	t_buffer *buffer = malloc(sizeof(t_buffer));
	t_paquete* paquete = crear_paquete(buffer, instruccion->identificador);
	agregar_a_paquete(paquete, (void*)direccion, sizeof(t_direc_fisica));
	agregar_a_paquete(paquete, valor, strlen(valor));
	enviar_paquete(socketMemoria, paquete, logger);
	eliminar_paquete(paquete);

}