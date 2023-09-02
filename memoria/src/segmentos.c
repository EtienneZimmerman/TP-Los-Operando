#include "memoria.h"

void crear_segmento(pid_t pid, int id, int tam_segmento, int conexion) 
{
    t_tabla_memoria* proceso_buscado = buscar_proceso(pid);
    t_segmento hueco = buscar_hueco(tam_segmento);
    switch(hueco.id) {
        case -1: 
            log_info(logger, "Solicito compactacion");
            pedir_compactacion(conexion); 
            t_paquete* paquete = recibir_paquete(conexion, logger);
            destruir_paquete(paquete);
            log_info(logger, "Procedo a compactar");
            compactar_memoria();
            sleep(retardo_compactacion / 1000);
            resultado_compactacion();
            enviar_procesos(conexion);
        break;
        case -2:
            //no hay memoria
            error_falta_memoria(conexion);
        break;
        default:
            t_segmento* segmento_nuevo = malloc(sizeof(t_segmento));
            segmento_nuevo->id = id;
            segmento_nuevo->base = hueco.base;
            segmento_nuevo->limite = hueco.limite;
            proceso_buscado->tabla_segmentos[id] = *segmento_nuevo;
            log_info(logger, "PID: %u - Crear Segmento: %d - Base: %u - TAMAÑO: %u", pid, id, hueco.base, tamanio_hueco(segmento_nuevo));
            enviar_proceso(proceso_buscado, conexion);
        break;
    }
    return;
}

void error_falta_memoria(int conexion)
{
    t_list* parametros = list_create();
    t_instruccion* instruccion_error = crear_instruccion(ERROR, parametros);

    t_buffer* buffer = crear_buffer_para_t_instruccion(instruccion_error); 
    t_paquete* paquete = crear_paquete(buffer, ERROR_MEMORIA);
    enviar_paquete(conexion, paquete, logger);
    destruir_paquete(paquete);
    destruir_instruccion(instruccion_error);
    return;
}

void borrar_segmento(pid_t pid, int id, int conexion) 
{
    t_tabla_memoria* proceso;
     for(int i = 0; i < list_size(procesos_en_memoria); i++) {
        proceso = list_get(procesos_en_memoria, i);
        if(proceso->pid == pid) {
            break; // si lo encuentro salgo del bucle;
            }
     }
    borrar_segmento_en_memoria(proceso, id);
    enviar_proceso(proceso, conexion);
}

void eliminar_segmentos(t_tabla_memoria* tabla)
{
    for(int i=1; i < tabla->tam_tabla; i++){
        int id = tabla->tabla_segmentos[i].id;
        if(id != -1){
            borrar_segmento(tabla->pid, id, conexion_kernel);
        }
    }
}
void borrar_segmento_en_memoria(t_tabla_memoria* proceso, int id)
{
    t_segmento* segmento_victima = &proceso->tabla_segmentos[id];
    log_info(logger, "PID: %u - Eliminar Segmento: %d - Base: %u - TAMAÑO: %u", proceso->pid, id, segmento_victima->base, tamanio_hueco(segmento_victima));
    agregar_hueco(segmento_victima);
    proceso->tabla_segmentos[id].id = -1;
}


t_tabla_memoria* buscar_proceso(pid_t pid)
{
    for(int i=0; i < list_size(procesos_en_memoria); i++){
        t_tabla_memoria* proceso = list_get(procesos_en_memoria, i);
        if(proceso->pid == pid){
            return proceso;
        }
    }
    log_error(logger, "ERROR - No se encontro proceso con el pid %d" , pid);

}