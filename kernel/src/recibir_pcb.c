

#include "kernel.h"

pthread_t hilo_IO;
t_list* cola_bloqueados_IO;

void recibir_pcb_de_cpu(int cpu_socket)
{
    t_pcb* pcb_actualizado = recibir_pcb(cpu_socket, logger);
    t_instruccion* ultima_instruccion = obtener_instruccion(pcb_actualizado);
    t_pcb* pcb_en_ejecucion;
    if(pcb_actualizado->estado == SEG_FAULT){
        terminar_proceso(pcb_actualizado, "SEGMENTATION FAULT");
    }

    if(!strcmp(kernel_config.schedulerAlgorithm, "HRRN")){
        actualizar_estimado_rafaga(pcb_actualizado);
    }
    
    switch(ultima_instruccion->identificador){
        case EXIT:
            pcb_en_ejecucion = sacar_pcb_de_ejecucion();
            terminar_proceso(pcb_actualizado, "SUCCESS");
            destruir_pcb(pcb_en_ejecucion);
        break;
        case YIELD:
            pcb_en_ejecucion = sacar_pcb_de_ejecucion();
            destruir_pcb(pcb_en_ejecucion);
            agregar_a_listos(pcb_actualizado);
        break;
        case I_O:
            pcb_en_ejecucion = sacar_pcb_de_ejecucion();
            log_info(logger,"PID: %d - Bloqueado por: IO", pcb_en_ejecucion->pid);
            destruir_pcb(pcb_en_ejecucion);
            ejecutar_IO(pcb_actualizado, ultima_instruccion);
        break;
        case SIGNAL:
            pcb_en_ejecucion = sacar_pcb_de_ejecucion();
            destruir_pcb(pcb_en_ejecucion);
            ejecutar_signal(pcb_actualizado, ultima_instruccion);
        break;
        case WAIT:
            pcb_en_ejecucion = sacar_pcb_de_ejecucion();
            destruir_pcb(pcb_en_ejecucion);
            ejecutar_wait(pcb_actualizado, ultima_instruccion);
        break;
        case CREATE_SEGMENT:
            pcb_en_ejecucion = sacar_pcb_de_ejecucion();
            poner_en_ejecucion(pcb_actualizado);
            crear_segmento(ultima_instruccion, pcb_actualizado);
        break;
            case DELETE_SEGMENT:
            pcb_en_ejecucion = sacar_pcb_de_ejecucion();
            poner_en_ejecucion(pcb_actualizado);
            eliminar_segmento(ultima_instruccion, pcb_actualizado);
        break;
        case MOV_IN:
        case MOV_OUT: 
            pcb_en_ejecucion = sacar_pcb_de_ejecucion();
            destruir_pcb(pcb_en_ejecucion);
            terminar_proceso(pcb_actualizado, "SEGMENTATION FAULT");
        break;
        case F_OPEN: //INCOMPLETO
            pcb_en_ejecucion = sacar_pcb_de_ejecucion();
            log_info(logger, "PID: %d - Abrir Archivo: %s", pcb_actualizado->pid, ultima_instruccion->parametros[0]);
            solicitar_apertura(pcb_actualizado);
            agregar_a_listos(pcb_actualizado); 
        break;
        case F_TRUNCATE: //INCOMPLETO
            log_info(logger, "PID: %d - Archivo: %s - Tamaño: %s ", pcb_actualizado->pid, ultima_instruccion->parametros[0], ultima_instruccion->parametros[1]);
            pcb_en_ejecucion = sacar_pcb_de_ejecucion();
            destruir_pcb(pcb_en_ejecucion);
            solicitar_truncamiento(pcb_actualizado);

        break;
        case F_SEEK://pendiente

        log_info(logger, "PID: %d- Actualizar puntero Archivo: %s - Puntero <PUNTERO>", pcb_actualizado->pid, ultima_instruccion->parametros[0]);
        break;
        case F_READ: //INCOMPLETO
            leer(pcb_actualizado);
            //log_info(logger, "PID: %d - Leer Archivo: %s - Puntero % - Dirección Memoria %u- Tamaño %s", pcb_actualizado->pid, ultima_instruccion->parametros[0], ,direccion_fisica, ultima_instruccion->parametros[2]);
       
        break;
        case F_WRITE: //INCOMPLETO
            escribir(pcb_actualizado);
            //log_info(logger, "PID: %d - Escribir Archivo: %s - Puntero % - Dirección Memoria %u- Tamaño %s", pcb_actualizado->pid, ultima_instruccion->parametros[0], ,direccion_fisica, ultima_instruccion->parametros[2]);
        break;
        case F_CLOSE: //pendiente
        log_info(logger, "PID: %d - Cerrar Archivo: %s", pcb_actualizado->pid, ultima_instruccion->parametros[0]);
        break;
    }
}

t_pcb* sacar_pcb_de_ejecucion()
{
    pthread_mutex_lock(&mutex_pcb_a_ejecutar);
    t_pcb* pcb = pcb_a_ejecutar;
    pcb_a_ejecutar = NULL;
    pthread_mutex_unlock(&mutex_pcb_a_ejecutar);
    sacar_de_cola_procesos(pcb);
    return pcb;
}

void ejecutar_IO(t_pcb* pcb, t_instruccion* instruccion)
{
    agregar_bloqueados_IO(pcb);
    log_info(logger, "PID: %d - Ejecuta IO: %d", pcb->pid, atoi(instruccion->parametros[0]));
    pthread_create(&hilo_IO, NULL, (void*)pasar_a_IO, (void*)atoi(instruccion->parametros[0]));
    pthread_detach(hilo_IO);
}

void pasar_a_IO(int tiempo)
{
    sleep(tiempo);
    t_pcb* pcb = sacar_de_bloqueado_IO();
    sacar_de_cola_procesos(pcb);
    agregar_a_listos(pcb);
}

void agregar_bloqueados_IO(t_pcb* pcb)
{
    list_add(cola_bloqueados_IO, pcb);
    cambiar_estado(pcb, BLOQUEADO);
    agregar_a_cola_procesos(pcb);
}

t_pcb* sacar_de_bloqueado_IO()
{
    return list_remove(cola_bloqueados_IO, 0);
    
}