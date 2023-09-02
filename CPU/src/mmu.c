#include "CPU.h"
#include <math.h>



int32_t traducir_direccion(uint32_t direccion_logica, t_pcb *pcb, int conexion_kernel, uint32_t cantidad_bytes) 
{
    uint32_t num_segmento = obtener_numero_segmento(direccion_logica);
    uint32_t desplazamiento_segmento = obtener_desplazamineto_segmento(direccion_logica);
    
    t_segmento segmento = pcb->tabla_segmentos[num_segmento]; 
    if((segmento.base + desplazamiento_segmento + cantidad_bytes) > segmento.limite){ // evaluo si estoy dentro del limite
        int tam_seg = segmento.base + segmento.limite;
        log_info(logger, "PID: %d - Error SEG_FAULT - Segmento: %u - offset: %u - Tamanio: %d", pcb->pid , num_segmento, desplazamiento_segmento, tam_seg);
        //                                                                          ayuda
        pcb->estado = SEG_FAULT;
        enviar_pcb(pcb, conexion_kernel, logger);
        temporal_stop(temporizador);
        pcb->tiempo_ejecucion = temporal_gettime(temporizador);
        temporal_destroy(temporizador);
        return -1;
    }
    return segmento.base + desplazamiento_segmento;
}


uint32_t obtener_numero_segmento(uint32_t direccion_logica) 
{
    // => saco el offet
    // floor -> redondea para abajo
	return floor(direccion_logica/tam_max_segmento);
    //                            ^^^^^^^^^^^^^^^^^^^^^^^^
    //                                     offset
}


uint32_t obtener_desplazamineto_segmento(uint32_t direccion_logica) 
{
    // => me quedo con el offset 
	return direccion_logica % tam_max_segmento;
}