#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_


#include <stdbool.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <dirent.h>

#include "utils-shared.h"
#define MAX_LINE_LENGTH 24
#define LOG_PATH "./cfg/fileSystem.log"
#define CONFIG_PATH "./cfg/fileSystem.config"



//************* ESTRUCTURAS *************//
typedef struct {
	int tam_bloque;
	int cantidad_bloques;
} t_superbloque;

typedef struct{
	char* archivo; 
	int tam_archivo;
	uint32_t puntero_directo;
	uint32_t puntero_indirecto;
}t_fcb;


//************* VARIABLES  *************//
//************* GLOBALES   *************//

extern t_log* logger;
extern t_config *config;
extern int conexionConMemoria;
extern char *ipMemoria;
extern char *puertoMemoria;
extern char *puertoEscucha;
extern char *path_superbloque;
extern char *path_bitmap;
extern char *path_bloques;
extern char *path_fcb;
extern int retardo_acceso_bloque;
extern int tamanio_bloque;
extern int cant_bloques; 
extern char* ipFs;

extern t_list* lista_fcbs;
extern t_superbloque* superbloque;
extern t_bitarray* bitmap;

//************* ---------- *************//

void iniciar_logger(void);
void iniciar_config(void);
void terminar_programa(int conexion, t_log *logger, t_config *config);
void levantar_estructuras();
void levantar_superbloque();
void levantar_bitmap();
void levantar_fcbs();
void levantar_bloques();
void atender_kernel(void* socket_kernel);
char pedir_a_memoria(t_instruccion* instruccion, int conexionConMemoria);
//************* MEMORIA *************//
void mandar_a_memoria(char* valor, t_direc_fisica* direccion, int socketMemoria, t_instruccion *instruccion);
t_direc_fisica* traducir_direccion(int direc_logica);

//************* BLOQUES *************//

int cant_bloques_nec(int tam_archivo);
void terminar_programa(int conexion, t_log *logger, t_config *config);
int bloqueLogicoAFisico(t_fcb* fcb, int num_bloque);
void borrar_bloques(int bloques_ocupados, int cant_bloques_a_borrar, t_fcb *fcb);
void agregar_bloques(int bloques_ocupados, int cant_bloques_a_agregar, t_fcb *fcb);
uint32_t asignar_bloque_libre();
int calcular_bloques_adicionales(uint32_t nuevo_tamanio, t_fcb *fcb);
int calcular_bloques_de_mas(uint32_t nuevo_tamanio, t_fcb *fcb);
void achicar(t_fcb* fcb, uint32_t tamanio_nuevo);
void agrandar(t_fcb* fcb, uint32_t tamanio_nuevo);
void liberar_bloques(t_fcb *fcb, int bloques_a_liberar);
int liberar_bloque_bitmap(uint32_t bloque_a_liberar);
void escribir_bloque_en_archivo(uint32_t bloque, uint32_t pos_en_bloque, uint32_t valor);
void asignar_bloque(t_fcb *fcb, uint32_t bloques_adicionales);

//************* ARCHIVOS *************//

bool abrir_archivo(char* archivo);
int crear_archivo(char *archivo);
t_fcb* encontrar_fcb(t_list* lista_fcbs, char* archivo);
void actualizar_fcb(t_fcb *fcb);
void truncar_archivo(char *archivo, int tamanio);

void leer_archivo(char *nombre, int puntero_archivo, int cantidad_bytes_para_leer, int direccion_fisica, pid_t pid);
void escribir(t_pcb* pcb, int una_conexion);
void escribir_archivo(char* nombre, int puntero_archivo, char* datos_a_escribir, int cantidad_bytes_para_escribir);

#endif /* FILESYSTEM_H_ */