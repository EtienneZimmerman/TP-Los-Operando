#include "utils-shared.h"

bool enviar_instrucciones(int socket, t_list *lista_instrucciones, t_log *logger)
{
    t_buffer *buffer = crear_buffer__para_t_lista_instrucciones(lista_instrucciones);
    t_paquete *paquete = crear_paquete(buffer, CODIGO_INSTRUCCION);
    bool res = enviar_paquete(socket, paquete, logger);
    destruir_paquete(paquete);
    return res;
}

t_instruccion* obtener_instruccion(t_pcb* pcb) {
    return list_get(pcb->instrucciones, pcb->p_counter - 1);
}

t_buffer *crear_buffer__para_t_lista_instrucciones(t_list *lista_instrucciones) // creo un buffer para muchas instrucciones
{
    t_buffer *buffer = malloc(sizeof(t_buffer));
    uint32_t size_total = 0;
    for (int i = 0; i < list_size(lista_instrucciones); i++)
    {
        t_instruccion *instruccion = list_get(lista_instrucciones, i);
        t_buffer *buffer_instruccion = crear_buffer_para_t_instruccion(instruccion); // 10 - 20 - 4
        size_total += buffer_instruccion->size;
        destruir_buffer(buffer_instruccion);
    }
    // creo el stream y copio los datos de cada buffer
    void *stream = malloc(size_total);
    buffer->size = size_total;
    uint32_t offset = 0;
    for (int i = 0; i < list_size(lista_instrucciones); i++)
    {
        t_buffer *buffer_instruccion = crear_buffer_para_t_instruccion(list_get(lista_instrucciones, i));
        uint32_t size = buffer_instruccion->size;
        void *stream_instruccion = buffer_instruccion->stream;
        memcpy(stream + offset, stream_instruccion, size);
        offset += size;
        destruir_buffer(buffer_instruccion);
    }
    buffer->stream = stream;
    return buffer;
}

t_buffer *crear_buffer_para_t_instruccion(t_instruccion *instruccion) // creo un buffer para una instruccion
{
    t_buffer *buffer = malloc(sizeof(t_buffer));
    buffer->size = sizeof(uint32_t) +                         // identificador
                   sizeof(uint32_t) +                         // cant_parametros
                   espacio_de_array_parametros(instruccion) + // p1 + p2 + p3 + p4
                   sizeof(uint32_t) * 4;                      // p1_length, p2_length, p3_length, p4_length
    void *stream = calloc(buffer->size, 1);

    int offset = 0;
    memcpy(stream + offset, &instruccion->identificador, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &instruccion->cant_parametros, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &instruccion->param_length1, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &instruccion->param_length2, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &instruccion->param_length3, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &instruccion->param_length4, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    for (int i = 0; i < instruccion->cant_parametros; i++)
    {
        memcpy(stream + offset, instruccion->parametros[i], strlen(instruccion->parametros[i]) + 1);
        offset += strlen(instruccion->parametros[i]) + 1;
    }
    buffer->stream = stream;
    return buffer;
}

uint32_t espacio_de_array_parametros(t_instruccion *instruccion)
{
    uint32_t espacio = 0;
    for (int i = 0; i < instruccion->cant_parametros; i++)
        espacio += strlen(instruccion->parametros[i]) + 1;
    return espacio;
}