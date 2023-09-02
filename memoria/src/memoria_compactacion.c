#include "memoria.h"

void compactar_memoria() 
{
    list_sort(lista_huecos, base_menor);
    t_segmento* hueco = list_get(lista_huecos, 0);
    while(hueco->limite != tamanio_memoria) {
        modificar_contiguo(hueco);
        hueco = list_get(lista_huecos, 0);
    }
    return;
}

void pedir_compactacion(int conexion)
{
    t_list* parametros = list_create();
    t_instruccion* instruccion_compactar = crear_instruccion(COMPACTAR, parametros);

    t_buffer* buffer = crear_buffer_para_t_instruccion(instruccion_compactar); 
    t_paquete* paquete = crear_paquete(buffer, COMPACTACION);
    enviar_paquete(conexion, paquete, logger);
    destruir_paquete(paquete);
    destruir_instruccion(instruccion_compactar);
    return;
}

void recibo_respuesta(int una_conexion)
{
    t_paquete* paquete = recibir_paquete(una_conexion, logger);
    destruir_paquete(paquete);
    return;
}

void resultado_compactacion() 
{
    log_info(logger, "------------------------------------------");
    for(int i = 0; i < list_size(procesos_en_memoria); i++) {
        t_tabla_memoria* proceso = list_get(procesos_en_memoria, i);
        t_segmento* segmentos = proceso->tabla_segmentos;
        int indiceTabla = 0;
        while(indiceTabla < proceso->tam_tabla) {
            if(segmentos[indiceTabla].id != -1) {  
                t_segmento segmento = segmentos[indiceTabla];
                log_info(logger, "PID: %d - Segmento: %d - Base: %u - Tamaño %d", proceso->pid, segmento.id, segmento.base, tamanio_hueco(&segmento));
            } 
            indiceTabla++;
        }
    }
}

void modificar_contiguo (t_segmento* hueco) 
{
    int indice = 0;
    t_tabla_memoria* proceso;
    while(indice < list_size(procesos_en_memoria)) {
        proceso  = list_get(procesos_en_memoria, indice);
        t_segmento* segmentos = proceso->tabla_segmentos;
        int indiceTabla = 0;
        while(indiceTabla < proceso->tam_tabla){
            if(segmentos[indiceTabla].id != -1 && segmentos[indiceTabla].base == hueco->limite){  
                int tam_segmento = segmentos[indiceTabla].limite - segmentos[indiceTabla].base;
                int anterior_limite = segmentos[indiceTabla].limite;
                int anterior_base = segmentos[indiceTabla].base;
                segmentos[indiceTabla].base = hueco->base;
                segmentos[indiceTabla].limite = segmentos[indiceTabla].base + tam_segmento;
                void* destino = memoria_fisica + segmentos[indiceTabla].base; //pregunta
                void* origen  = memoria_fisica + anterior_base;
                // actualizo segento
                memmove(destino, origen, tam_segmento); 
                list_remove_element(lista_huecos, hueco);
                crear_hueco(anterior_limite, segmentos[indiceTabla].limite);
                return;
            }
            indiceTabla++;
        }
        indice++;
    }
    return;
}

void enviar_procesos(int una_conexion)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));
    uint32_t size_total = 0;
    for (int i = 0; i < list_size(procesos_en_memoria); i++)
    {
        t_buffer *buffer_proceso = crear_buffer_tabla_memoria(list_get(procesos_en_memoria, i));
        size_total += buffer_proceso->size;
        destruir_buffer(buffer_proceso);
    }

    // creo el stream y copio los datos de cada buffer
    void *stream = malloc(size_total);
    buffer->size = size_total;
    uint32_t offset = 0;
    for (int i = 0; i < list_size(procesos_en_memoria); i++)
    {
        t_buffer *buffer_proceso = crear_buffer_tabla_memoria(list_get(procesos_en_memoria, i));
        uint32_t size = buffer_proceso->size;
        void *stream_proceso = buffer_proceso->stream;
        memcpy(stream + offset, stream_proceso, size);
        offset += size;
        destruir_buffer(buffer_proceso);
    }
    buffer->stream = stream;
    t_paquete* paquete = crear_paquete(buffer, COMPACTACION);
    enviar_paquete(una_conexion, paquete, logger);
    destruir_paquete(paquete);
    return;
}