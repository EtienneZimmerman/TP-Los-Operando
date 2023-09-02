#include "kernel.h"

bool solicitar_apertura(t_pcb* pcb) {

    enviar_pcb(pcb, filesystem_socket, logger);
    char* respuesta[20];
    t_instruccion* ultima_instruccion = obtener_instruccion(pcb);
    recv(filesystem_socket, respuesta, sizeof(respuesta), 0);

    if(!strcmp(respuesta, "abierto")) {
        return true;
    }
    else {
        log_info(logger, "PID: %d - Crear Archivo: %s", pcb->pid, ultima_instruccion->parametros[0]);
        solicitar_creacion(ultima_instruccion); 
    }
}

void solicitar_creacion(t_instruccion* instruccion) {

    t_list* parametros = list_create();
    list_add(parametros, instruccion->parametros[0]);
    t_instruccion* instruccion_pedidofs = crear_instruccion(F_CREATE, parametros);
    t_buffer* buffer = crear_buffer_para_t_instruccion(instruccion_pedidofs);
    t_paquete* paquete = crear_paquete(buffer, CODIGO_INSTRUCCION);
    enviar_paquete(filesystem_socket, paquete, logger);
    char* respuesta[20];
    recv(filesystem_socket, respuesta, sizeof(respuesta), 0);
    destruir_paquete(paquete);

}

void solicitar_truncamiento(t_pcb* pcb) {
    enviar_pcb(pcb, filesystem_socket, logger);
    char* respuesta[20];
    recv(filesystem_socket, respuesta, sizeof(respuesta), 0);

}

void leer(t_pcb* pcb) {

}

void escribir(t_pcb* pcb) {

}