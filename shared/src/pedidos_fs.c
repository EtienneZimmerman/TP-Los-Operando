#include "utils-shared.h"



t_buffer* crear_buffer_para_t_fs_pedido(t_fs_pedido* fs_pedido) 
{
    t_buffer* buffer = malloc(sizeof(t_buffer));

    uint32_t tam_instruccion =  sizeof(pid_t) +             //pid
                                sizeof(int32_t) +           //direc_f
                                sizeof(uint32_t) * 4 +      //
                                espacio_de_array_parametros(fs_pedido->instruccion);
    
    buffer->size = sizeof(pid_t) + sizeof(int32_t) + tam_instruccion;

    void* stream = malloc(buffer->size);

    int offset = 0;
    memcpy(stream + offset, &fs_pedido->pid, sizeof(pid_t));
    offset += sizeof(pid_t);
    memcpy(stream + offset, &fs_pedido->direccion_fisica, sizeof(int32_t));
    offset += sizeof(int32_t);

    t_buffer* buffer_instr = crear_buffer_para_t_instruccion(fs_pedido->instruccion);
    void *stream_instr = buffer_instr->stream;
    memcpy(stream + offset, stream_instr, buffer_instr->size);
    offset += buffer_instr->size;
    buffer->stream = stream;
    
    free(buffer_instr);
    return buffer;
}


void enviar_fs_pedido(t_fs_pedido* fs_pedido, int conexion, t_log* logger) 
{
    t_buffer* buffer = crear_buffer_para_t_fs_pedido(fs_pedido);
    t_paquete *paquete = crear_paquete(buffer, INFO_FS);
    enviar_paquete(conexion, paquete, logger);
}


t_fs_pedido* crear_pedido_para_buffer_fs(t_buffer* buffer) 
{
    t_fs_pedido* fs_pedido = malloc(sizeof(t_fs_pedido));
    void* stream = buffer->stream;
    uint32_t offset = 0;
    // PID -> direc -> instruccion
    memcpy(&(fs_pedido->pid), stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(&(fs_pedido->direccion_fisica), stream + offset, sizeof(int32_t));
    offset += sizeof(int32_t);
    fs_pedido->instruccion = crear_instruccion_para_el_buffer(buffer, &offset);
    
    return fs_pedido;
}