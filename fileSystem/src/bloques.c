#include "fileSystem.h"

int calcular_bloques_adicionales(uint32_t nuevo_tamanio, t_fcb *fcb)
{

    double archivo_tam = (double)fcb->tam_archivo;
    double nuevo_tam = (double)nuevo_tamanio;
    double bloque_tam = (double)superbloque->tam_bloque;

    int actuales = (int)ceil(archivo_tam / bloque_tam);
    int nuevos = (int)ceil(nuevo_tam / bloque_tam);

    int bloques_ad = nuevos - actuales;

    return bloques_ad;
}

int calcular_bloques_de_mas(uint32_t nuevo_tamanio, t_fcb *fcb)
{

    double archivo_tam = (double)fcb->tam_archivo;
    double nuevo_tam = (double)nuevo_tamanio;
    double bloque_tam = (double)superbloque->tam_bloque;

    int actuales = (int)ceil(archivo_tam / bloque_tam);
    int nuevos = (int)ceil(nuevo_tam / bloque_tam);

    int bloques_de_mas = actuales - nuevos;

    return bloques_de_mas;
}

uint32_t buscar_bloque_libre(t_bitarray *bitmap)
{

    uint32_t bloque_libre = -1;

    uint32_t tamanio_bitmap = bitarray_get_max_bit(bitmap);
    for (int i = 0; i < tamanio_bitmap; i++)
    {

        if (!bitarray_test_bit(bitmap, i))
        { // el bit en la pos i del bitmap es 0 osea esta libre =>
            log_info(logger, "Acceso a Bitmap - Bloque: %d - Estado: %d", i, bitarray_test_bit(bitmap, i));
            bitarray_set_bit(bitmap, i); // lo seteo como ocupado (1)

            bloque_libre = i;
            break;
        }
    }
    return bloque_libre;
}

void liberar_bloques(t_fcb *fcb, int bloques_para_liberar)
{
    int bloques_totales_cant = ceil(fcb->tam_archivo/superbloque->tam_bloque);
    int archivo_bloques = open(path_bloques, O_RDWR);

    while (bloques_para_liberar > 0 && bloques_totales_cant > 1)
    {
        uint32_t leido;
        lseek(archivo_bloques, (int)fcb->puntero_indirecto * superbloque->tam_bloque + (bloques_totales_cant - 1) * sizeof(uint32_t), SEEK_SET); 
        read(archivo_bloques, &leido, sizeof(uint32_t));
        sleep(retardo_acceso_bloque / 1000);
        liberar_bloque_bitmap(leido);
        bloques_totales_cant--;
        bloques_para_liberar--;
    }
    if (bloques_totales_cant == 1 && bloques_para_liberar == 1)
    {
        liberar_bloque_bitmap(fcb->puntero_directo);
        liberar_bloque_bitmap(fcb->puntero_indirecto);
    }
}

int liberar_bloque_bitmap(uint32_t bloque)
{
    log_info(logger, "Acceso a Bitmap - Bloque: %u - Estado: %d", bloque, bitarray_test_bit(bitmap, bloque));
    bitarray_clean_bit(bitmap, bloque);
}

void asignar_bloque(t_fcb *fcb, uint32_t bloques_adicionales)
{
    uint32_t bloques_asignados = ceil(fcb->tam_archivo/superbloque->tam_bloque);
    for (uint32_t i = 0; i < bloques_adicionales; i++)
    {
        uint32_t bloque_libre = buscar_bloque_libre(bitmap);
        if (bloque_libre != -1)
        { 
            escribir_bloque_en_archivo(fcb->puntero_indirecto, bloques_asignados - 1 + i, bloque_libre);
        }
        else
        {
            log_error(logger, "No hay bloques libres\n");
            break;
        }
    }
    bloques_asignados += bloques_adicionales;
}

void escribir_bloque_en_archivo(uint32_t bloque, uint32_t posicion, uint32_t valor)
{
    int archivo_bloques = open(path_bloques, O_RDWR); //=~ rb+

    if (archivo_bloques == -1)
    {
        log_error(logger, "No se pudo abrir el archivo de bloques");
        return;
    }

    lseek(archivo_bloques, bloque * superbloque->tam_bloque + posicion * sizeof(uint32_t), SEEK_SET); //m muevo
    sleep(retardo_acceso_bloque / 1000);
    write(archivo_bloques, &valor, sizeof(uint32_t));

    close(archivo_bloques);
}