#include "utils-shared.h"


t_buffer* crear_buffer_tabla_memoria(t_tabla_memoria* nuevo_proceso)
{
    t_buffer* buffer = malloc(sizeof(t_buffer));
    
    buffer->size =    sizeof(pid_t)     // pid
                    + sizeof(uint32_t)  // tam_tabla
                    + (sizeof(int)*3) * nuevo_proceso->tam_tabla;  // segmento
    
    void* stream = malloc(buffer->size);

    int offset = 0;

    memcpy(stream + offset, &nuevo_proceso->pid, sizeof(pid_t));
    offset += sizeof(pid_t);
    memcpy(stream + offset, &nuevo_proceso->tam_tabla, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    for(int i=0; i < nuevo_proceso->tam_tabla; i++){
        t_segmento segmento = nuevo_proceso->tabla_segmentos[i];
        memcpy(stream + offset, &segmento.id, sizeof(int));
        offset += sizeof(int);
        memcpy(stream + offset, &segmento.base, sizeof(int));
        offset += sizeof(int);
        memcpy(stream + offset, &segmento.limite, sizeof(int));
        offset += sizeof(int);
    }
    buffer->stream = stream;
    return buffer;
    }



t_tabla_memoria* recibir_memoria(int socket, t_log* logger)
{
    t_paquete *paquete = recibir_paquete(socket, logger);
    t_tabla_memoria* respuesta_memoria = deserializar_buffer_para_tabla_memoria(paquete->buffer);
    destruir_paquete(paquete);
	return respuesta_memoria;
}


void pedir_memoria(t_instruccion* pedido, int socket, t_log* logger) 
{
    t_buffer* buffer = crear_buffer_para_t_instruccion(pedido); 
    t_paquete* paquete = crear_paquete(buffer, CODIGO_INSTRUCCION);
    enviar_paquete(socket, paquete, logger);
    destruir_paquete(paquete);
}


t_tabla_memoria* deserializar_buffer_para_tabla_memoria(t_buffer* buffer)
{
    t_tabla_memoria* proceso = malloc(sizeof(t_tabla_memoria));

    void* stream = buffer->stream;

    memcpy(&(proceso->pid), stream, sizeof(pid_t));
    stream += sizeof(pid_t);
    memcpy(&(proceso->tam_tabla), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    proceso->tabla_segmentos = malloc(sizeof(t_segmento) * proceso->tam_tabla);
    t_segmento* tabla_segmento = proceso->tabla_segmentos;

    for (int i = 0; i < proceso->tam_tabla; i++) {
        memcpy(&(tabla_segmento[i].id), stream, sizeof(int));
        stream += sizeof(int);
        memcpy(&(tabla_segmento[i].base), stream, sizeof(int));
        stream += sizeof(int);
        memcpy(&(tabla_segmento[i].limite), stream, sizeof(int));
        stream += sizeof(int);
    }
    return proceso;
}

t_buffer* crear_buffer_para_t_pedido_file_system(t_fs_pedido* pedido_filesystem) 
{
    t_buffer* buffer = malloc(sizeof(t_buffer));

    uint32_t tam_instruccion = sizeof(uint32_t) + sizeof(int32_t) + sizeof(uint32_t) * 4 + 
                              espacio_de_array_parametros(pedido_filesystem->instruccion);
    buffer->size = sizeof(uint32_t) + sizeof(int32_t) + tam_instruccion;

    void* stream = malloc(buffer->size);
    int offset = 0;
    memcpy(stream + offset, &pedido_filesystem->pid, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &pedido_filesystem->direccion_fisica, sizeof(int32_t));
    offset += sizeof(int32_t);

    t_buffer* buffer_instruccion = crear_buffer_para_t_instruccion(pedido_filesystem->instruccion);
    void *stream_instruccion = buffer_instruccion->stream;
    memcpy(stream + offset, stream_instruccion, buffer_instruccion->size);
    offset += buffer_instruccion->size;
    buffer->stream = stream;
    
    free(buffer_instruccion);
    return buffer;
}


void enviar_pedido_file_system(t_fs_pedido* pedido_filesystem, int conexion, t_log* logger_kernel) 
{
    t_buffer* buffer = crear_buffer_para_t_pedido_file_system(pedido_filesystem);
    t_paquete *paquete = crear_paquete(buffer, 79);
    enviar_paquete(conexion, paquete, logger_kernel);
}


t_fs_pedido* crear_pedido_file_system_para_el_buffer(t_buffer* buffer) 
{
    t_fs_pedido* pedido_filesystem = malloc(sizeof(t_fs_pedido));
    void* stream = buffer->stream;
    uint32_t offset = 0;
    memcpy(&(pedido_filesystem->pid), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(&(pedido_filesystem->direccion_fisica), stream + offset, sizeof(int32_t));
    offset += sizeof(int32_t);
    pedido_filesystem->instruccion = crear_instruccion_para_el_buffer(buffer, &offset);
    return pedido_filesystem;
}