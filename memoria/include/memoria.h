#ifndef MEMORIA_H_
#define MEMORIA_H_


#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h> 
#include "utils-shared.h"
#define LOG_PATH "./cfg/memoria.log"
#define CONFIG_PATH "./cfg/memoria.config"

//************ ESTRUCTURAS   ************//

typedef struct{
    pid_t pid;
    //int tamanio;
} t_new_procesos;

typedef enum{
    BEST,
    WORST,
    FIRST
}t_algoritmo;

//************* VARIABLES  *************//
//************* GLOBALES   *************//

extern int memoria_libre;
extern t_list* tablas_segmentos;
extern int memoria_libre;
extern void* memoria_fisica;
extern t_list* lista_huecos;
extern t_algoritmo algoritmo;

extern char*  puerto;
extern int tamanio_memoria;
extern int tamanio_segmento_0;
extern int cantidad_segmentos;
extern int retardo_memoria;
extern int retardo_compactacion;
extern char* algoritmo_asignacion;
extern char* ip_memoria;
extern t_log* logger;
extern pthread_t hilo_de_escucha_cpu;
extern pthread_t hilo_de_escucha_fs;
extern pthread_t hilo_de_escucha_kernel; 
extern t_list* procesos_en_memoria;
extern t_segmento segmento_cero;
extern t_segmento segmento_vacio; 

extern int conexion_kernel;

//************* -----------  *************//

void levantar_config(char* path);
void crear_estructuras();
int asignar_espacio_en_memoria();
void inicializarMemoria();

//************* CONEXIONES  *************//

void escuchar(int puerto_escucha);
void atender_cpu(void* conexion);
void atender_kernel(void* conexion);
void atender_fs(void* conexion);
void escucha_general(void* conexion);
void escucha_general(void* conexion);


//************* SEGMENTOS  *************//

t_algoritmo asignar_algoritmo();
void crear_segmento(pid_t pid, int id, int tam_segmento, int conexion);
void eliminar_segmentos(t_tabla_memoria* tabla);
void borrarSegmento(t_tabla_memoria* tabla, int id);
int espacio_hueco(t_segmento* segmento);
void eliminar_segmentos(t_tabla_memoria* tabla);
bool ordenar_segun_base(t_segmento* seg_1, t_segmento* seg_2);
void agregar_hueco(t_segmento* segmento_a_borrar);
void borrar_segmento(pid_t pid, int id, int conexion);
void error_falta_memoria(int conexion);
void borrar_segmento_en_memoria(t_tabla_memoria* proceso, int id);

////************* HUECO  *************//

t_segmento buscar_hueco(int tam_segmento);
t_segmento modificar_lista_huecos(t_segmento* hueco, int tam_segmento, int index);
void crear_hueco(int limite, int base);
int tamanio_hueco(t_segmento* hueco);
bool tam_mas_grande(t_segmento* h_1, t_segmento* h_2);
bool tam_mas_chico(t_segmento* h_1, t_segmento* h_2);
bool base_menor(t_segmento* h_1, t_segmento* h_2);
int calcular_total();
void inicializar_lista_huecos();


//************* PROCESOS  *************//

void crear_proceso_memoria(pid_t pid, int conexion_cpu);
void enviar_proceso(t_tabla_memoria* nuevoProceso, int socket);
bool hay_espacio_libre(t_list *segmentos_vacios);
int obtener_tam_segmento(t_segmento *segmento);
void eliminar_proceso(pid_t pid);
t_tabla_memoria* buscar_proceso(pid_t pid);

//************* COMPACTAR  *************//

void modificar_contiguo (t_segmento* hueco);
void resultado_compactacion();
void compactar_memoria();
void pedir_compactacion(int conexion);
void recibo_respuesta(int una_conexion);

#endif /* MEMORIA_H_ */