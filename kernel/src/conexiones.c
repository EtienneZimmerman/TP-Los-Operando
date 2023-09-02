#include "kernel.h"


void atender_consola(void* conexion)
{
	int conexion_consola = (int)conexion;
	log_info(logger, "escuchando a consola");
    t_paquete* paquete_instrucciones = recibir_paquete(conexion_consola, logger);
	t_list* lista_instrucciones = crear_lista_instrucciones_para_el_buffer(paquete_instrucciones->buffer);
	t_pcb* pcb = crear_pcb(conexion_consola, lista_instrucciones, NUEVO, kernel_config.initialEstimate); 

	solicitud_memoria(pcb->pid, CREAR_PROCESO);
	t_tabla_memoria* proceso_memoria = recibir_memoria(memoria_socket, logger);
	pcb->tabla_segmentos = proceso_memoria->tabla_segmentos;
	pcb->tamanio_tabla = proceso_memoria->tam_tabla;

	log_info(logger, "Se crea el proceso %d en NEW", pcb->pid);
	agregar_a_nuevos(pcb);
}

void atender_cpu(void* conexion)
{
	int conexion_cpu = (int)conexion;
	log_info(logger, "escuchando a cpu");
	t_pcb* pcb = recibir_pcb(conexion_cpu, logger);
	log_info(logger, "recibi pcb");

	// hacer lo que tengo que hacer
	destruir_pcb(pcb); 
	close(conexion_cpu);

}