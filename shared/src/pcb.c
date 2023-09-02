#include "utils-shared.h"

int size_registros = sizeof(char[4]) * 4 + sizeof(char[8])* 4 + sizeof(char[16]) * 4; 

void enviar_pcb(t_pcb* pcb, int socket, t_log* logger){
	t_buffer* buffer = crear_buffer_pcb(pcb, logger);
	t_paquete* paquete = crear_paquete(buffer, CODIGO_PCB);
	enviar_paquete(socket, paquete, logger);
	destruir_paquete(paquete);
}

t_pcb* crear_pcb(int conexion, t_list* lista_instrucciones, t_estado estado, double estimado_rafaga)
{
	t_pcb* pcb = malloc(sizeof(t_pcb));
    pcb->pid = obtener_pid();
	pcb->conexion = conexion;
    pcb->instrucciones = lista_instrucciones; 
	pcb->p_counter = 0; // ip inicializado
	pcb->estado = estado; 
    pcb->registros = inicializar_registros();
	pcb->tabla_segmentos = NULL; 
	pcb->tamanio_tabla = 0;
	pcb->estimado_rafaga = estimado_rafaga; 
	pcb->direccion_fisica = 0;
	return pcb;
}

pid_t obtener_pid() {
    static pid_t contador_pid = 0; // Contador estático para mantener el último PID asignado
    return ++contador_pid;
}

t_registro inicializar_registros()
{
	t_registro registro;

	strcpy(registro.AX,  "\0");
	strcpy(registro.BX,  "\0");
	strcpy(registro.CX,  "\0");
	strcpy(registro.DX,  "\0");
	strcpy(registro.EAX, "\0");
	strcpy(registro.EBX, "\0");
	strcpy(registro.ECX, "\0");
	strcpy(registro.EDX, "\0");
	strcpy(registro.RAX, "\0");
	strcpy(registro.RBX, "\0");
	strcpy(registro.RCX, "\0");
	strcpy(registro.RDX, "\0");
	
	return registro;
}

t_buffer *crear_buffer_pcb(t_pcb *pcb, t_log *logger)
{
	t_buffer* buffer = crear_buffer();
	t_buffer* buffer_instrucciones = crear_buffer__para_t_lista_instrucciones(pcb->instrucciones);

	uint32_t size_total = sizeof(uint32_t)*3    // direc_fisica y tamanio tabla
						+ sizeof(int)*2         // conexion, p_counter
						+ sizeof(pid_t)  		// pid
						//+ sizeof(int32_t)*2   // 
						+ size_registros		//
						+ sizeof(int64_t)		// tiempo ejecucion
						+ (sizeof(int32_t) + sizeof(uint32_t) * 2) * pcb->tamanio_tabla  
						// (id + base + segmento) * cuan grande es la tabla 
						// ^^^^ esta bien ?? ^^^^
						+ buffer_instrucciones->size; 

	buffer->size = size_total;

	void* stream = malloc(size_total);
	int offset = 0;

	//paso pcb
	memcpy(stream + offset, &pcb->conexion, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &pcb->pid, sizeof(pid_t));
	offset += sizeof(pid_t);
	memcpy(stream + offset, &pcb->p_counter, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &pcb->estado, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, &pcb->direccion_fisica, sizeof(int32_t));
	offset += sizeof(int32_t);
	memcpy(stream + offset, &pcb->tiempo_ejecucion, sizeof(int64_t));
	offset += sizeof(int64_t);
	memcpy(stream + offset, &pcb->tamanio_tabla, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    for(int i = 0; i < pcb->tamanio_tabla; i++)
    {
        t_segmento segmento = pcb->tabla_segmentos[i];
        memcpy(stream + offset, &segmento.id, sizeof(int));
        offset += sizeof(int);
        memcpy(stream + offset, &segmento.base, sizeof(int));
        offset += sizeof(int);
        memcpy(stream + offset, &segmento.limite, sizeof(int));
        offset += sizeof(int);
    }
    
	memcpy(stream + offset, &pcb->registros, size_registros);
	offset += size_registros;
	memcpy(stream + offset, buffer_instrucciones->stream, buffer_instrucciones->size);
	offset += buffer_instrucciones->size;

	buffer->stream = stream;
	destruir_buffer(buffer_instrucciones);
	return buffer;
}

t_pcb* recibir_pcb(int socket_cliente, t_log* logger)
{
	t_paquete* paquete = recibir_paquete(socket_cliente, logger);
	t_pcb* pcb = deserializar_buffer_pcb(paquete->buffer, logger);

	eliminar_paquete(paquete);
	return pcb;
}

t_pcb* deserializar_buffer_pcb(t_buffer* buffer, t_log* logger)
{
	t_pcb* pcb = malloc(sizeof(t_pcb));

	void* stream = buffer->stream;
	int size_restante = buffer->size;
	
	memcpy(&(pcb->conexion), stream, sizeof(int));
	stream += sizeof(int);
	size_restante -= sizeof(int);
	memcpy(&(pcb->pid), stream, sizeof(pid_t));
	stream += sizeof(pid_t);
	size_restante -= sizeof(pid_t);
    memcpy(&(pcb->p_counter), stream, sizeof(int));
	stream += sizeof(int);
	size_restante -= sizeof(int);
    memcpy(&(pcb->estado), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	size_restante -= sizeof(uint32_t);
	memcpy(&(pcb->direccion_fisica), stream, sizeof(int32_t));
	stream += sizeof(int32_t);
	size_restante -= sizeof(int32_t);
	memcpy(&(pcb->tiempo_ejecucion), stream, sizeof(int64_t));
	stream += sizeof(int64_t);
	size_restante -= sizeof(int64_t);
    memcpy(&(pcb->tamanio_tabla), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
	size_restante -= sizeof(uint32_t);
    pcb->tabla_segmentos = malloc(sizeof(t_segmento) * pcb->tamanio_tabla);
    t_segmento* tabla_segmento = pcb->tabla_segmentos;
    for (int i = 0; i < pcb->tamanio_tabla; i++) {
        memcpy(&(tabla_segmento[i].id), stream, sizeof(int));
        stream += sizeof(int);
		size_restante -= sizeof(int);
        memcpy(&(tabla_segmento[i].base), stream, sizeof(int));
        stream += sizeof(int);
		size_restante -= sizeof(int);
        memcpy(&(tabla_segmento[i].limite), stream, sizeof(int));
        stream += sizeof(int);
		size_restante -= sizeof(int);
    }

	memcpy(&(pcb->registros), stream, size_registros);
	stream += size_registros;
	size_restante -= size_registros;
	memcpy(buffer->stream, stream, size_restante);
	//free(stream);  
	buffer->size=size_restante;
	//pcb->tabla_archivos = list_create();
	pcb->instrucciones = list_create();
	t_list *lista_instrucciones = crear_lista_instrucciones_para_el_buffer(buffer);
	list_add_all(pcb->instrucciones, lista_instrucciones);
	return pcb;
	
}

void destruir_pcb(t_pcb* pcb)
{
	list_destroy_and_destroy_elements(pcb->instrucciones, (void*)destruir_instruccion);
	free(pcb);
}
void destruir_instruccion(t_instruccion *instruccion)
{
    if (instruccion == NULL)
    {
        return;
    }
    for (int i = 0; i < instruccion->cant_parametros; i++)
    {
        if (instruccion->parametros[i] != NULL)
        {
            free(instruccion->parametros[i]);
        }
    }
    free(instruccion->parametros);
    free(instruccion);
}