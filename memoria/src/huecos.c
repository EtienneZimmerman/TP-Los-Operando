#include "memoria.h"


t_segmento buscar_hueco(int tam_segmento)
{
    t_segmento* hueco = NULL;

    if(algoritmo == WORST) {
        list_sort(lista_huecos, tam_mas_grande);
        hueco = list_get(lista_huecos, 0);
        int tam_hueco = hueco->limite - hueco->base;
        if(tam_hueco >= tam_segmento){
            return modificar_lista_huecos(hueco, tam_segmento, 0);
        }
    }
    else{
        if(algoritmo == BEST){
            list_sort(lista_huecos, tam_mas_chico);
        }
        else if(algoritmo == FIRST){
            list_sort(lista_huecos, base_menor);
        }

        int i = 0;
        while(i < list_size(lista_huecos)){
            hueco = list_get(lista_huecos, i);
            int tam_hueco = hueco->limite - hueco->base;
            if(tam_hueco >= tam_segmento){
                return modificar_lista_huecos(hueco, tam_segmento, i);
            }
            i++;
        }
    }
    int total_tamanio = calcular_total();
    if(total_tamanio >= tam_segmento){
        hueco->id = -1;
        return *hueco;
    }
    else{
        hueco->id = -2; 
        return *hueco;
    }
}

void inicializar_lista_huecos()
{
    t_segmento* hueco = malloc(sizeof(t_segmento));
    hueco->base = segmento_cero.limite;
    hueco->limite = tamanio_memoria;
    list_add(lista_huecos, hueco);
}

t_segmento modificar_lista_huecos(t_segmento* hueco, int tam_segmento, int index) 
{
    t_segmento segmento_aux;
    segmento_aux.base = hueco->base;
    segmento_aux.limite = tam_segmento + segmento_aux.base;

    hueco->base = segmento_aux.limite;
    if(hueco->base == hueco->limite){
        list_remove(lista_huecos, index);
    }
    return segmento_aux;
}

void crear_hueco(int limite, int base)
{
    t_segmento* hueco_a_crear = malloc(sizeof(t_segmento)); 
    hueco_a_crear->base   = base;
    hueco_a_crear->limite = limite;

    list_add_sorted(lista_huecos, hueco_a_crear, base_menor); 

    if(list_size(lista_huecos) == 1){
        return;
    }

    t_segmento* hueco_sgte = list_get(lista_huecos, 1); 
    if(hueco_sgte->base == hueco_a_crear->limite){ 
        hueco_a_crear->limite = hueco_sgte->limite;
        t_segmento* hueco_Aux = list_remove(lista_huecos, 1);
    }
    return;
}

// ****************** funciones comparar *****************//

int tamanio_hueco(t_segmento* hueco) 
{
    return hueco->limite - hueco->base;
}

bool tam_mas_grande(t_segmento* h_1, t_segmento* h_2) 
{
    return tamanio_hueco(h_1) > tamanio_hueco(h_2);
}

bool tam_mas_chico(t_segmento* h_1, t_segmento* h_2) 
{
    return tamanio_hueco(h_1) < tamanio_hueco(h_2);
}

bool base_menor(t_segmento* h_1, t_segmento* h_2) 
{
    return h_1->base < h_2->base;
}


//********************** calcular **********************//

int calcular_total()
{
    int i = 0;
    int tamanioTotal = 0;
    while(i < list_size(lista_huecos)) {
        t_segmento* hueco = list_get(lista_huecos, i);
        tamanioTotal += tamanio_hueco(hueco);
        i++;
    }

    return tamanioTotal;
}

int espacio_hueco(t_segmento* segmento)
{
	return segmento->limite - segmento->base;
}

void agregar_hueco(t_segmento* segmento_a_borrar)
{
    int j = 0;
    for(int i = 0; i < list_size(lista_huecos); i++) {
        t_segmento* un_hueco = list_get(lista_huecos, i); 

        // evaluo si el segmento esta pegado a otro para unificar el hueco 

        if( un_hueco->base == segmento_a_borrar->limite ){ //si esta antes de un hueco 
            un_hueco->base =  segmento_a_borrar->base;
            segmento_a_borrar->limite = un_hueco->limite;
            j++;
        }
        else if (un_hueco->limite == segmento_a_borrar->base) { // si esta depues de un hueco
            un_hueco->limite = segmento_a_borrar->limite;
            segmento_a_borrar->base = un_hueco->base;
            j++;
        }
        if (j == 2) return;
    }
    list_add_sorted(lista_huecos, segmento_a_borrar, ordenar_segun_base);
    return;
}

bool ordenar_segun_base(t_segmento* seg_1, t_segmento* seg_2)
{
	return seg_1->base < seg_2->base;
}

int obtenertam_hueco(t_segmento* hueco) {
    return hueco->limite - hueco->base;
}