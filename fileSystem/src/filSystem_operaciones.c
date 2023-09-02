#include "fileSystem.h"

bool abrir_archivo(char* archivo) {
    for(int i = 0; i < list_size(lista_fcbs); i++) {
        t_fcb* pcb_nuevo = list_get(lista_fcbs, i);
        if(strcmp(pcb_nuevo->archivo, archivo) == 0) {
            return true; 
        }
    }
    return false;
}

t_fcb* encontrar_fcb(t_list* lista_fcbs, char* archivo) {
    for(int i = 0; i < list_size(lista_fcbs); i++) {
        t_fcb* pcb_nuevo = list_get(lista_fcbs, i);
        if(strcmp(pcb_nuevo->archivo, archivo) == 0) {
            return pcb_nuevo; 
        }
    }
}

int crear_archivo(char *archivo) {
    t_fcb *fcb = malloc(sizeof(t_fcb));
    fcb->archivo = malloc(strlen(archivo) + 1);

    strcpy(fcb->archivo, archivo);
    fcb->tam_archivo = 0;
    fcb->puntero_directo = 0;
    fcb->puntero_indirecto = 0;
    char *ruta_fcb = malloc(strlen(path_fcb) + strlen(archivo) + 2);
    strcpy(ruta_fcb, path_fcb);
    strcat(ruta_fcb, "/");
    strcat(ruta_fcb, archivo);

    FILE *file_fcb = fopen(ruta_fcb, "w");
    if (file_fcb == NULL) {
        free(ruta_fcb);
        free(fcb->archivo);
        free(fcb); 
        return -1; 
        // en case de error
    }

    fprintf(file_fcb, "NOMBRE_ARCHIVO=%s\n", fcb->archivo);
    fprintf(file_fcb, "TAMANIO_ARCHIVO=%d\n", fcb->tam_archivo);
    fprintf(file_fcb, "PUNTERO_DIRECTO=%u\n", fcb->puntero_directo);
    fprintf(file_fcb, "PUNTERO_INDIRECTO=%u", fcb->puntero_indirecto);

    fclose(file_fcb);
    list_add(lista_fcbs, fcb);
    free(ruta_fcb);
    log_info(logger, "Crear Archivo: %s", archivo);
    return 0; 
}

void actualizar_fcb (t_fcb *fcb) {
    char *ruta_fcb = malloc(strlen(path_fcb) + strlen(fcb->archivo) + 2);
    strcpy(ruta_fcb, path_fcb);
    strcat(ruta_fcb, "/");
    strcat(ruta_fcb, fcb->archivo);
    t_config *fcb_config = config_create(ruta_fcb);

    config_set_value(fcb_config, "NOMBRE_ARCHIVO", fcb->archivo);
    config_set_value(fcb_config, "TAMANIO_ARCHIVO", string_itoa(fcb->tam_archivo));
    config_set_value(fcb_config, "PUNTERO_DIRECTO", string_itoa(fcb->puntero_directo));
    config_set_value(fcb_config, "PUNTERO_INDIRECTO", string_itoa(fcb->puntero_indirecto));
    config_save(fcb_config);
}

void truncar_archivo(char *archivo, int tamanio){
    log_info(logger, "Truncar Archivo: %s - Tamaño: %d", archivo, tamanio);

    t_fcb* fcb = encontrar_fcb(lista_fcbs, archivo);
    if(fcb->tam_archivo > tamanio) {
        achicar(fcb, tamanio); 
    }
    else {
        agrandar(fcb, tamanio);
    }
    actualizar_fcb (fcb);
    list_clean(lista_fcbs);
    levantar_fcbs(path_fcb);

}

void achicar(t_fcb* fcb, uint32_t tamanio_nuevo) {
    
    int bloques_de_mas = calcular_bloques_de_mas(tamanio_nuevo, fcb);

    liberar_bloques(fcb, bloques_de_mas);

    fcb->tam_archivo = tamanio_nuevo;
} 

void agrandar(t_fcb* fcb, uint32_t tamanio_nuevo) {

    uint32_t bloques_adicionales = calcular_bloques_adicionales(tamanio_nuevo, fcb);

    if (bloques_adicionales > 0)
    {
        
            uint32_t bloque_libre; 
                if(fcb->tam_archivo == 0) { 
                    bloque_libre = buscar_bloque_libre(bitmap);
                    fcb->puntero_directo = bloque_libre;
                    bloque_libre = buscar_bloque_libre(bitmap);  
                    fcb->puntero_indirecto = bloque_libre; 
                    bloques_adicionales--;
                        } 
                    asignar_bloque(fcb, bloques_adicionales);
    }
    fcb->tam_archivo = tamanio_nuevo;
}


void agregar_bloques(int ocupados, int bloques_a_sumar, t_fcb *fcb){
    if(ocupados == 0){
        fcb->puntero_directo = buscar_bloque_libre();
        bloques_a_sumar -= 1; 
    }
    else{
        for(int i = 0; i < bloques_a_sumar; i++){
            fcb->puntero_indirecto = buscar_bloque_libre();
        }
    }
    
}

void leer_archivo(char *archivo, int puntero_archivo, int bytes_para_leer, int direccion_fisica, pid_t pid) {
    t_fcb *fcb = encontrar_fcb(lista_fcbs, archivo);
    
    int arch_bloques = open(path_bloques, O_RDWR);

    char* leido = malloc(bytes_para_leer + 1);
    strcpy(leido, "");
    int lectura_bloque_actual = 0;
    int puntero_en_bloque = 0;
    int bloque_leido;

    while (bytes_para_leer > 0)
    {
        int bloque_actual = ceil((double)puntero_archivo/(double)superbloque->tam_bloque);
        puntero_en_bloque = puntero_archivo%superbloque->tam_bloque;
        if(puntero_en_bloque == 0) {
            bloque_actual += 1;
        }
        if ((superbloque->tam_bloque - puntero_en_bloque) > bytes_para_leer) 
        { 
            lectura_bloque_actual = bytes_para_leer; 
            } 
        else { 
            lectura_bloque_actual = superbloque->tam_bloque - puntero_en_bloque; 
            }
        
        if(bloque_actual == 1) {
            lseek(arch_bloques, (fcb->puntero_directo) * superbloque->tam_bloque + puntero_en_bloque, SEEK_SET);
            log_info(logger, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", archivo, bloque_actual, fcb->puntero_directo);
            char* datos_leidos_bloque = malloc(lectura_bloque_actual);
            ssize_t numBytesLeidos = read(arch_bloques, datos_leidos_bloque, lectura_bloque_actual);
            strcat(leido, datos_leidos_bloque);
            puntero_archivo += lectura_bloque_actual;
            bytes_para_leer -= lectura_bloque_actual;
            sleep(retardo_acceso_bloque / 1000);
        }
        else {
            log_info(logger, "Acceso Bloque - Archivo: %s - Bloque Archivo: 2 - Bloque File System %d ", archivo, fcb->puntero_indirecto);       
            lseek(arch_bloques, (int)fcb->puntero_indirecto * superbloque->tam_bloque + (bloque_actual - 2) * sizeof(uint32_t), SEEK_SET);
            read(arch_bloques, &bloque_leido, sizeof(uint32_t));
            log_info(logger, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", archivo, bloque_actual+1, bloque_leido);
            lseek(arch_bloques, bloque_leido * superbloque->tam_bloque + puntero_en_bloque, SEEK_SET);
            char* datos_leidos_bloque = malloc(lectura_bloque_actual);
            ssize_t numBytesLeidos = read(arch_bloques, datos_leidos_bloque, lectura_bloque_actual);
            sleep(retardo_acceso_bloque / 1000);
            strcat(leido, datos_leidos_bloque);
            puntero_archivo += lectura_bloque_actual;
            bytes_para_leer -= lectura_bloque_actual;
            
        }

    }
    sleep(retardo_acceso_bloque / 1000);
    close(arch_bloques);
    
    t_list* param = list_create();
    list_add(param, leido);
    
    t_fs_pedido* pedido_fs = malloc(sizeof(t_fs_pedido));
    pedido_fs->instruccion = crear_instruccion(F_READ, param);
    pedido_fs->pid = pid;
    pedido_fs->direccion_fisica = direccion_fisica;
    t_buffer *buffer = crear_buffer_para_t_pedido_file_system(pedido_fs);
    t_paquete *paquete_a_enviar = crear_paquete(buffer, 79); // poner código
    enviar_paquete(conexionConMemoria, paquete_a_enviar, logger);
    list_destroy(param);

    t_paquete *paquete_a_recibir = recibir_paquete(conexionConMemoria, logger);

    destruir_paquete(paquete_a_enviar);
    destruir_paquete(paquete_a_recibir);


}

void escribir(t_pcb* pcb, int conexion) {

    t_fs_pedido* pedido_fs = malloc(sizeof(t_fs_pedido));
    t_instruccion* instruccion = obtener_instruccion(pcb);
    pedido_fs->instruccion = instruccion;
    pedido_fs->pid = pcb->pid;
    pedido_fs->direccion_fisica = pcb->direccion_fisica;
    enviar_pedido_file_system(pedido_fs, conexionConMemoria, logger);
    t_paquete *paquete = recibir_paquete(conexionConMemoria, logger);
	int offset=0;
    t_instruccion* respuestaMemoria = crear_instruccion_para_el_buffer(paquete->buffer,&offset);
    log_info(logger , "Escribir Archivo: %s - Puntero: %s - Memoria: %d - Tamaño: %s", pedido_fs->instruccion->parametros[0], pedido_fs->instruccion->parametros[1], pedido_fs->direccion_fisica, pedido_fs->instruccion->parametros[2]);
    escribir_archivo(instruccion->parametros[0], atoi(instruccion->parametros[1]), respuestaMemoria->parametros[0], atoi(pedido_fs->instruccion->parametros[2])); 
    destruir_paquete(paquete);
    send(conexion, "SI", strlen("SI"), 0);

}

void escribir_archivo(char* archivo, int puntero_archivo, char* data_escritura, int bytes_para_escribir) {
    t_fcb* fcb = encontrar_fcb(lista_fcbs, archivo);

    if (fcb == NULL)
        return;

    if (fcb->tam_archivo < puntero_archivo + bytes_para_escribir) {
        log_error(logger, "FALLA - Archivo: %s no se pudo escribir, los bytes solicitados superan su tamanio", archivo);
        free(fcb->archivo);
        free(fcb);
        return;
    }

    int arch_bloques = open(path_bloques, O_RDWR);
    int bytes_escritos = 0;
    int bytes_para_escribir_en_bloque_actual = 0;
    int bloque_leido;
    int puntero_en_bloque = 0;

    while (bytes_para_escribir > 0) {
        int bloque_actual = ceil((double)puntero_archivo / (double)superbloque->tam_bloque);
        puntero_en_bloque  = puntero_archivo % superbloque->tam_bloque;
        if(puntero_en_bloque == 0) {
            bloque_actual += 1;
        }

        if ((superbloque->tam_bloque - puntero_en_bloque) > bytes_para_escribir) {
            bytes_para_escribir_en_bloque_actual = bytes_para_escribir;
        } else {
            bytes_para_escribir_en_bloque_actual = superbloque->tam_bloque - puntero_en_bloque;
        }

        if (bloque_actual == 1) {
            lseek(arch_bloques, (fcb->puntero_directo) * superbloque->tam_bloque + puntero_en_bloque, SEEK_SET);
            log_info(logger, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", archivo, bloque_actual, fcb->puntero_directo);
            char* datos_leidos_bloque = string_substring_from(data_escritura, bytes_escritos);
            write(arch_bloques, datos_leidos_bloque, bytes_para_escribir_en_bloque_actual);
            puntero_archivo += bytes_para_escribir_en_bloque_actual;
            bytes_escritos += bytes_para_escribir_en_bloque_actual;
            bytes_para_escribir -= bytes_para_escribir_en_bloque_actual;
            sleep(retardo_acceso_bloque/ 1000);
        } else {
            log_info(logger, "Acceso Bloque - Archivo: %s - Bloque Archivo: 2 - Bloque File System %d ", archivo, fcb->puntero_indirecto);       
            lseek(arch_bloques, (int)fcb->puntero_indirecto * superbloque->tam_bloque + (bloque_actual - 2) * sizeof(uint32_t), SEEK_SET);
            read(arch_bloques, &bloque_leido, sizeof(uint32_t));
            log_info(logger, "Acceso Bloque - Archivo: %s - Bloque Archivo: %d - Bloque File System %d ", archivo, bloque_actual+1, bloque_leido);
            lseek(arch_bloques, bloque_leido * superbloque->tam_bloque + puntero_en_bloque, SEEK_SET);
            char* datos_leidos_bloque = string_substring_from(data_escritura, bytes_escritos);
            write(arch_bloques, datos_leidos_bloque, bytes_para_escribir_en_bloque_actual);
            sleep(retardo_acceso_bloque / 1000);
            puntero_archivo += bytes_para_escribir_en_bloque_actual;
            bytes_escritos += bytes_para_escribir_en_bloque_actual;
            bytes_para_escribir -= bytes_para_escribir_en_bloque_actual;
        }
    }

    sleep(retardo_acceso_bloque / 1000);
    close(arch_bloques);
}

