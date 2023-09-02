#include "memoria.h"


void crear_proceso_memoria(pid_t pid, int conexion_cpu) 
{
    t_tabla_memoria* proceso_nuevo = malloc(sizeof(t_tabla_memoria));
    t_segmento* tabla_segmento = (t_segmento*)calloc((uint32_t)cantidad_segmentos , sizeof(t_segmento)); //cambio a confirmar
    tabla_segmento[0] = segmento_cero;
    proceso_nuevo->pid = pid;
    proceso_nuevo->tabla_segmentos = tabla_segmento;
    proceso_nuevo->tam_tabla = cantidad_segmentos;
    list_add(procesos_en_memoria, proceso_nuevo); 

    for(int i = 1; i < cantidad_segmentos; i++){
        tabla_segmento[i] = segmento_vacio; 
    }
    log_info(logger, "Creacion de Proceso PID: %u", pid);
    enviar_proceso(proceso_nuevo, conexion_cpu);
}

void enviar_proceso(t_tabla_memoria* proceso_nuevo, int socket)
{
    t_buffer* buffer = crear_buffer_tabla_memoria(proceso_nuevo);
    t_paquete* paquete = crear_paquete(buffer, CODIGO_INSTRUCCION_MEMORIA);
    enviar_paquete(socket, paquete, logger);
	destruir_paquete(paquete);
}

void eliminar_proceso(pid_t pid)
{
    for(int i=0; i < list_size(procesos_en_memoria); i++){
        t_tabla_memoria* tabla = list_get(procesos_en_memoria, i);
        if(tabla->pid == pid){
            eliminar_segmentos(tabla);
            free(tabla->tabla_segmentos); // ver si dejar
            free(tabla);
            list_remove(procesos_en_memoria, i);
        }
    
    }
    log_info(logger, "Eliminacion de Proceso PID: %u", pid);
}