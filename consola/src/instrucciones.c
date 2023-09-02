#include "consola.h"

t_identificador buscar_identificador(char *identificador)
{
    t_identificador id;
    if (string_equals_ignore_case(identificador, "F_READ"))
        id = F_READ;
    else if (string_equals_ignore_case(identificador, "F_WRITE"))
        id = F_WRITE;
    else if (string_equals_ignore_case(identificador, "SET"))
        id = SET;
    else if (string_equals_ignore_case(identificador, "MOV_IN"))
        id = MOV_IN;
    else if (string_equals_ignore_case(identificador, "MOV_OUT"))
        id = MOV_OUT;
    else if (string_equals_ignore_case(identificador, "F_TRUNCATE"))
        id = F_TRUNCATE;
    else if (string_equals_ignore_case(identificador, "F_SEEK"))
        id = F_SEEK;
    else if (string_equals_ignore_case(identificador, "CREATE_SEGMENT"))
        id = CREATE_SEGMENT;
    else if (string_equals_ignore_case(identificador, "I/O"))
        id = I_O;
    else if (string_equals_ignore_case(identificador, "WAIT"))
        id = WAIT;
    else if (string_equals_ignore_case(identificador, "SIGNAL"))
        id = SIGNAL;
    else if (string_equals_ignore_case(identificador, "F_OPEN"))
        id = F_OPEN;
    else if (string_equals_ignore_case(identificador, "F_CLOSE"))
        id = F_CLOSE;
    else if (string_equals_ignore_case(identificador, "DELETE_SEGMENT"))
        id = DELETE_SEGMENT;
    else if (string_equals_ignore_case(identificador, "EXIT"))
        id = EXIT;
    else if (string_equals_ignore_case(identificador, "YIELD"))
        id = YIELD;
    return id;
}

t_list* parsear_pseudocodigo(FILE* archivo_instrucciones) {
    ////// creo variable para leer linea //////
    char* linea = malloc(sizeof(char) * 1024);
    size_t len = sizeof(linea);  
    ////// creo lista para las instrucciones //////
    t_list* lista_instrucciones = list_create();

    ////// leo linea del archivo //////
    while ((getline(&linea, &len, archivo_instrucciones)) != -1) {
        t_list* palabras = list_create();
        char* t = strtok(linea, "\n");   
        char** tokens = string_split(t, " "); 
        ////// ^^ separo linea en palabras //////

        int i = 1;
        while(tokens[i] != NULL){
            ////// por cada token guardo en palabras //////
            list_add(palabras, (void*) tokens[i]);
            i++;
        }
        t_identificador identificador = buscar_identificador(tokens[0]); 
        t_instruccion* instruccion = crear_instruccion(identificador, palabras);  
        list_add(lista_instrucciones, instruccion);     
       
        free(tokens);
        list_destroy(palabras); 
    }
    free(linea);
    return lista_instrucciones;
}

