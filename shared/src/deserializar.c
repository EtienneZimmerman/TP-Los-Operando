#include "utils-shared.h"

t_list *crear_lista_instrucciones_para_el_buffer( t_buffer *buffer)
{
    t_list *lista_instrucciones = list_create();
    uint32_t offset = 0;
    while (offset < buffer->size)
    {
        t_instruccion *instruccion = crear_instruccion_para_el_buffer(buffer, &offset);
        list_add(lista_instrucciones, instruccion);
    }
    return lista_instrucciones;
}

t_instruccion *crear_instruccion_para_el_buffer(t_buffer *buffer, uint32_t *offset)
{
    t_instruccion *instruccion = malloc(sizeof(t_instruccion));
    void *stream = buffer->stream;
    uint32_t offset_aux = 0;
    stream += (*offset);

    memcpy(&(instruccion->identificador), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(instruccion->cant_parametros), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(instruccion->param_length1), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(instruccion->param_length2), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(instruccion->param_length3), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(instruccion->param_length4), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    offset_aux += sizeof(uint32_t) * 6;
    instruccion->parametros = NULL;
    if (instruccion->cant_parametros == 0)
    {
        *offset += offset_aux;
        return instruccion;
    }
    instruccion->parametros = (char **)malloc(instruccion->cant_parametros * sizeof(char *));
    instruccion->parametros[0] = malloc(instruccion->param_length1);
    memcpy(instruccion->parametros[0], stream, instruccion->param_length1);
    offset_aux += instruccion->param_length1;
    stream += instruccion->param_length1;
    if (instruccion->param_length1 == 1)
    {
        offset_aux -= 1;
        stream -= 1;
    }
    if (instruccion->cant_parametros == 1)
    {
        *offset += offset_aux;
        return instruccion;
    }
    instruccion->parametros[1] = malloc(instruccion->param_length2);
    memcpy(instruccion->parametros[1], stream, instruccion->param_length2);
    offset_aux += instruccion->param_length2;
    stream += instruccion->param_length2;
    if (instruccion->param_length2 == 1)
    {
        offset_aux -= 1;
        stream -= 1;
    }
    if (instruccion->cant_parametros == 2)
    {
        *offset += offset_aux;
        return instruccion;
    }
    instruccion->parametros[2] = malloc(instruccion->param_length3);
    memcpy(instruccion->parametros[2], stream, instruccion->param_length3);
    offset_aux += instruccion->param_length3;
    stream += instruccion->param_length3;
    if (instruccion->param_length3 == 1)
    {
        offset_aux -= 1;
        stream -= 1;
    }
    if (instruccion->cant_parametros == 3)
    {
        *offset += offset_aux;
        return instruccion;
    }
    instruccion->parametros[3] = malloc(instruccion->param_length4);
    memcpy(instruccion->parametros[3], stream, instruccion->param_length4);
    offset_aux += instruccion->param_length4;
    stream += instruccion->param_length4;

    if (instruccion->param_length4 == 1)
    {
        offset_aux -= 1;
        stream -= 1;
    }
    
    *offset += offset_aux;

    return instruccion;
}