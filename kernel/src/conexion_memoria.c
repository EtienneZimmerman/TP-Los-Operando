#include "kernel.h"

pid_t pid_aux;

void solicitud_memoria(pid_t pid, t_estado solicitud)
{
    t_list* parametros = list_create();
    int pid_int = (int)pid;
    char* pid_str = string_itoa(pid_int);
    list_add(parametros, pid_str);
    t_instruccion* instruccion_memoria = crear_instruccion(solicitud, parametros);
    pedir_memoria(instruccion_memoria, memoria_socket, logger);
}

void crear_segmento(t_instruccion* instruccion, t_pcb* pcb) 
{
    t_list* parametros = list_create();
    uint32_t pid = pcb->pid;
    char* pid_str = string_itoa(pid);
    list_add(parametros, instruccion->parametros[0]);
    list_add(parametros, instruccion->parametros[1]);
    list_add(parametros, pid_str);
    t_instruccion* instruccion_pedido = crear_instruccion(CREATE_SEGMENT, parametros);
    pedir_memoria(instruccion_pedido, memoria_socket , logger);
    log_info(logger, "PID: %u - Crear Segmento - Id: %s - Tamaño: %s", pid, instruccion->parametros[0], instruccion->parametros[1]);
    recibir_creacion(pcb, instruccion_pedido);
}


void eliminar_segmento(t_instruccion* instruccion, t_pcb* pcb) 
{
    t_list* parametros = list_create();
    uint32_t pid = pcb->pid;
    char* pid_str = string_itoa(pid);
    list_add(parametros, instruccion->parametros[0]);
    list_add(parametros, pid_str);
    t_instruccion* instruccion_pedido = crear_instruccion(DELETE_SEGMENT, parametros);
    pedir_memoria(instruccion_pedido, memoria_socket, logger);
    t_paquete *paquete = recibir_paquete(memoria_socket, logger);
    log_info(logger, "PID: %u - Eliminar Segmento - Id Segmento: %s", pid, instruccion->parametros[0]);
    actualizar_tabla(paquete, pcb);
}

void recibir_creacion(t_pcb* pcb, t_instruccion* instruccion_pedido) 
{
    t_paquete *paquete = recibir_paquete(memoria_socket, logger);
    switch(paquete->codigo_operacion) {
        
        case CODIGO_INSTRUCCION_MEMORIA: 
            actualizar_tabla (paquete, pcb);
        break;
        case ERROR_MEMORIA:
            t_pcb* pcb_en_ejecucion = sacar_pcb_de_ejecucion();
            terminar_proceso(pcb_en_ejecucion, "OUT_OF_MEMORY");
        break;
        case COMPACTACION:
            t_list* parametros = list_create();
            log_info(logger, "Compactación: Se solicitó compactación");
            t_instruccion* pedidoCompactacion = crear_instruccion(COMPACTACION, parametros);
            log_info(logger, "Esperando fin de Filesystem");
            //esperar_fin_FS();
            log_info(logger, "Cantidad procesos en sistema %u", list_size(cola_procesos));
            pedir_memoria(pedidoCompactacion, memoria_socket, logger); 
            actualizar_procesos_recibidos(memoria_socket);
            log_debug(logger, "Se finalizó el proceso de compactación");
            pedir_memoria(instruccion_pedido, memoria_socket, logger); 
            recibir_creacion(pcb, instruccion_pedido);
        break;
        default:
            log_error(logger, "mensaje desconocido");

    }
    destruir_paquete(paquete);
}

void actualizar_tabla(t_paquete* paquete, t_pcb* pcb)
{
    t_tabla_memoria* tabla_memoria = deserializar_buffer_para_tabla_memoria(paquete->buffer);
    pcb->tabla_segmentos = tabla_memoria->tabla_segmentos;
    pcb->tamanio_tabla = tabla_memoria->tam_tabla;
    //reenvio a cpu 
    int conexion_memoria = crear_conexion(kernel_config.cpuIP, kernel_config.cpuPort);
    enviar_pcb(pcb, conexion_memoria, logger);
    log_info(logger, "El PCB con ID %d se reenvio a  CPU", pcb->pid);
    recibir_pcb_de_cpu(conexion_memoria); 
    close(conexion_memoria);
}

void actualizar_procesos_recibidos() 
{
    t_paquete* paquete = recibir_paquete(memoria_socket, logger); 
    uint32_t offset = 0;
    while (offset < paquete->buffer->size)
    {
        t_buffer* buffer = malloc(sizeof(t_buffer));
        uint32_t size_restante = paquete->buffer->size - offset;
        buffer->size = size_restante;
        buffer->stream = malloc(size_restante);
        memcpy(buffer->stream, paquete->buffer->stream + offset, size_restante);

        t_tabla_memoria *proceso = deserializar_buffer_para_tabla_memoria(buffer);
        offset +=  sizeof(pid_t) +              
                   sizeof(uint32_t) +                     
                   (sizeof(int) + sizeof(int) * 2) * proceso->tam_tabla;  
        
        pid_aux = proceso->pid;
        t_pcb* pcb_a_actualizar = buscar_pcb(proceso->pid);
        pcb_a_actualizar->tabla_segmentos = proceso->tabla_segmentos;
        pcb_a_actualizar->tamanio_tabla = proceso->tam_tabla;

        free(buffer->stream);
        free(buffer);
    }
    return;
}

t_pcb* buscar_pcb(pid_t* pid)
{

    pthread_mutex_lock(&procesosEnSistemaMutex);
    pid_aux = pid;
    t_pcb* retorno = list_find(cola_procesos, mismo_pcb);
    pthread_mutex_unlock(&procesosEnSistemaMutex);
    return retorno;
}

bool mismo_pcb(t_pcb* pcb)
{
    return pcb->pid == pid_aux;
}