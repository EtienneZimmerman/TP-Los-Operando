 #include "utils-shared.h"
 
 void agregar_parametro_a_instruccion(t_list* parametros, t_instruccion* instruccion)
{
    int i = 0;
    if(parametros != NULL)
        while (i < instruccion->cant_parametros)
        {
            instruccion->parametros[i] = list_get(parametros, i);
            i++;
        }   
    instruccion->param_length1 = 0;      ////////////////////////
    instruccion->param_length2 = 0;      //     inicializo     //
    instruccion->param_length3 = 0;      //      tamanios      //
    instruccion->param_length4 = 0;      ////////////////////////

    if(instruccion->cant_parametros >= 1)
        instruccion->param_length1 = strlen(instruccion->parametros[0]) + 1;
    if (instruccion->cant_parametros >= 2)
        instruccion->param_length2 = strlen(instruccion->parametros[1]) + 1;
    if (instruccion->cant_parametros >= 3)
        instruccion->param_length3 = strlen(instruccion->parametros[2]) + 1;
    if (instruccion->cant_parametros >= 4)
        instruccion->param_length4 = strlen(instruccion->parametros[3]) + 1;
}



t_instruccion* crear_instruccion(t_identificador identificador, t_list* parametros)
{
    t_instruccion* instruccion_tmp = malloc(sizeof(t_instruccion));

    instruccion_tmp->identificador = identificador;
    if (list_size(parametros) < 1 ) {
        instruccion_tmp->cant_parametros = 0;
        instruccion_tmp->parametros = NULL;
        instruccion_tmp->param_length1 = 0;
        instruccion_tmp->param_length2 = 0;
        instruccion_tmp->param_length3 = 0;
        instruccion_tmp->param_length4 = 0;
    } else {      
        instruccion_tmp->cant_parametros = list_size(parametros);
        instruccion_tmp->parametros = malloc(sizeof(char*) * instruccion_tmp->cant_parametros);
        agregar_parametro_a_instruccion(parametros, instruccion_tmp);
    }

    return instruccion_tmp;

}