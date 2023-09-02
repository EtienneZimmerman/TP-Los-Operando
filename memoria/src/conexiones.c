#include "memoria.h"

void escuchar(int puerto_escucha) {

    pthread_t thread_cpu;
    pthread_t thread_filesystem;
    pthread_t thread_kernel;

    while (1) {
        int cliente = esperar_cliente(puerto_escucha);
        char tipo_cliente[25];
        recv(cliente, tipo_cliente, sizeof(tipo_cliente), 0);
        
        if (strcmp(tipo_cliente, "hola, soy kernel") == 0) {
            pthread_create(&thread_kernel, NULL, atender_kernel, (void*)cliente);

        } else if (strcmp(tipo_cliente, "hola, soy filesystem") == 0) {
            pthread_create(&thread_filesystem, NULL, atender_fs, (void*)cliente);

        } else if (strcmp(tipo_cliente, "hola, soy cpu") == 0) {
            pthread_create(&thread_cpu, NULL, atender_cpu, (void*)cliente);

        } else {
            log_error(logger, "Cliente desconocido");
            break;
        }
    }

    pthread_join(thread_cpu, NULL);
    pthread_join(thread_filesystem, NULL);
    pthread_join(thread_kernel, NULL);

    return;
}


void atender_cpu(void* conexion)
{
	int conexion_cpu = (int) conexion;
	log_info(logger, "Escuchando a memoria");

    t_paquete* paquete;
    while (1)
    {
        paquete = recibir_paquete(conexion_cpu, logger);
        uint32_t offset = 0;
        t_instruccion* pedidoMemoria = crear_instruccion_para_el_buffer(paquete->buffer, &offset);
       
        pid_t pid = (pid_t)strtoul(pedidoMemoria->parametros[0], NULL, 10);
        int32_t direccion_fisica = (int32_t)strtoul(pedidoMemoria->parametros[1], NULL, 10);
        char* valor_registro = malloc(sizeof(char)*17);
        t_paquete* respuesta;
        uint32_t tamanio_registro;
        t_buffer* buffer;
        t_list* parametros_respuesta = list_create();
        t_instruccion* respuesta_memoria;
        switch (pedidoMemoria->identificador) {
            case MOV_IN:
                tamanio_registro = (uint32_t)strtoul(pedidoMemoria->parametros[2], NULL, 10);
                void* inicio_lectura = memoria_fisica + direccion_fisica;
                memcpy(valor_registro, inicio_lectura, sizeof(char)*tamanio_registro);
                
                list_add(parametros_respuesta, valor_registro);
                respuesta_memoria = crear_instruccion(MOV_IN, parametros_respuesta);
                buffer = crear_buffer_para_t_instruccion(respuesta_memoria);
                respuesta = crear_paquete(buffer, CODIGO_INSTRUCCION_MEMORIA);
                log_info(logger, "PID: %u - Acción: LEER - Dirección física: %d - Tamaño: %u - Origen: CPU", pid, direccion_fisica, tamanio_registro);
            break;

            case MOV_OUT:
                valor_registro = pedidoMemoria->parametros[2];
                tamanio_registro = sizeof(char)*strlen(valor_registro);
                memcpy((memoria_fisica+direccion_fisica), valor_registro, tamanio_registro);
                respuesta_memoria = crear_instruccion(MOV_OUT, parametros_respuesta);
                buffer = crear_buffer_para_t_instruccion(parametros_respuesta);
                respuesta = crear_paquete(buffer, MOV_OUT);
                log_info(logger, "PID: %u - Acción: ESCRIBIR - Dirección física: %d - Tamaño: %u - Origen: CPU", pid, direccion_fisica, tamanio_registro);
                valor_registro = "";
            break;

            default:
                log_info(logger, "Mensaje no reconocido");
            break;
        }
        usleep(retardo_memoria*1000);
        enviar_paquete(conexion_cpu, respuesta, logger);
   		destruir_paquete(respuesta);
        destruir_paquete(paquete);
        destruir_instruccion(respuesta_memoria);
        //free(valor_registro);
    }
    log_info(logger, "Se cerrará la conexión (CPU - Memoria)");
    log_info(logger, "Se desconectó un cliente");
    close(conexion_cpu);

}

void atender_kernel(void* conexion)
{
	conexion_kernel = (int)conexion;
	log_info(logger, "escuchando a kernel");

	while(1){
		t_paquete *paquete = recibir_paquete(conexion_kernel, logger); 
		uint32_t offset = 0;
		t_instruccion *instruccion =  crear_instruccion_para_el_buffer(paquete->buffer, &offset); 
		pid_t pid;
		int id;
        int tam_segmento;
		switch(instruccion->identificador){
			case CREATE_SEGMENT:
				id = strtoul(instruccion->parametros[0], NULL, 10);
                tam_segmento = strtoul(instruccion->parametros[1], NULL, 10);
                pid = (pid_t)(uint32_t)strtoul(instruccion->parametros[2], NULL, 10);
                crear_segmento(pid, id, tam_segmento, conexion_kernel);
				break;
			case DELETE_SEGMENT:
				id  = strtoul(instruccion->parametros[0], NULL, 10);
                pid = (pid_t)(uint32_t)strtoul(instruccion->parametros[1], NULL, 10);
                borrar_segmento(pid, id, conexion_kernel);
                break;
			break;
			case CREAR_PROCESO:
				pid = (pid_t)atoi(instruccion->parametros[0]);
				crear_proceso_memoria(pid, conexion);
			break;
			case ELIMINAR_PROCESO:
                pid = (pid_t)atoi(instruccion->parametros[0]);
				eliminar_proceso(pid);
			break;
			default:
				log_error(logger, "mensaje no reconocido");
				return;
			break;
		}
	 	destruir_paquete(paquete);
        destruir_instruccion(instruccion);
    }
    log_info(logger, "cierro conexion con kernel");
    
	close(conexion_kernel);
}
	


void atender_fs(void* conexion)
{
	int conexion_fs = (int)conexion;
	log_info(logger, "escuchando a filesystem");
	t_pcb* pcb = recibir_pcb(conexion_fs, logger);
	//recibo algo
	t_paquete *paquete = recibir_paquete(conexion_fs, logger); 
	while(1)
    {
        paquete = recibir_paquete(conexion_fs, logger);
        t_fs_pedido* fs_pedido = crear_pedido_para_buffer_fs(paquete->buffer);
        pid_t pid = fs_pedido->pid;
        int32_t direccion_fisica = fs_pedido->direccion_fisica;
        t_paquete* paquete_rta;
        uint32_t lectura;
        t_instruccion* instr_respuesta;
        t_list* parametros = list_create();
        t_buffer* buffer; 
    
        switch (fs_pedido->instruccion->identificador) {
            case F_READ: 
                char* archivo = malloc(sizeof(fs_pedido->instruccion->parametros[0])); 
                archivo = fs_pedido->instruccion->parametros[0];          
                lectura = sizeof(char)*strlen(archivo);
                memcpy((memoria_fisica+direccion_fisica), archivo, lectura); 
                //pasarle el valor
                instr_respuesta = crear_instruccion(F_READ, parametros);
                buffer = crear_buffer_para_t_instruccion(instr_respuesta);
                paquete_rta = crear_paquete(buffer, F_READ_OK);
                log_info(logger, "PID: %d - Acción: ESCRIBIR - Dirección física: %d - Tamaño: %u - Origen: FS", pid, direccion_fisica, lectura);
                archivo = ""; // inicializo para proxima
                break;
            case F_WRITE: 
                lectura = (uint32_t)strtoul(fs_pedido->instruccion->parametros[2], NULL, 10);
                char* escritura = malloc(lectura);
                void* inicio_escritura = memoria_fisica + direccion_fisica;
                memcpy(escritura, inicio_escritura, sizeof(char)*lectura); 
                list_add(parametros, escritura);
                instr_respuesta = crear_instruccion(F_WRITE, parametros);
                buffer = crear_buffer_para_t_instruccion(instr_respuesta);
                paquete_rta = crear_paquete(buffer, CODIGO_INSTRUCCION_MEMORIA);
                log_info(logger, "PID: %u - Acción: LEER - Dirección física: %d - Tamaño: %u - Origen: FS", pid, direccion_fisica, lectura);
                break;
            default:
                log_info(logger, "Mensaje no reconocido");
                break;
        }
        
        usleep(retardo_memoria*1000);
        enviar_paquete(conexion_fs, paquete_rta, logger);
	    destruir_paquete(paquete_rta);
        destruir_paquete(paquete); 
    }

    log_info(logger, "Cierro conexion con fs");
    close(conexion_fs);   
    } 

bool hay_espacio_libre(t_list *segmentos_vacios)
{
	int espacio_total = 0;
	for(int i = 0; i < list_size(segmentos_vacios); i++){
		t_segmento *segmento = list_get(segmentos_vacios, i);
		espacio_total += obtener_tam_segmento(segmento);
	}
	return espacio_total > 0;
}

int obtener_tam_segmento(t_segmento *segmento)
{
	return segmento->limite - segmento->base;
}