#ifndef CPU_H_
#define CPU_H_


#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include "utils-shared.h"
#define CONFIG_PATH "./cfg/CPU.config"
#define LOG_PATH "./cfg/CPU.log"


//************* VARIABLES  *************//
//************* GLOBALES   *************//

extern t_registro registro_cpu;

extern t_log* logger;

extern int conexion_con_memoria;
extern char *ipCPU;
extern char *ipMemoria;
extern char *puertoMemoria;
extern char *valorMemoria;
extern char *puerto_escucha;
extern int tam_max_segmento;
extern int retardo_instruccion;
extern t_temporal* temporizador;


t_log* iniciar_logger(void);
t_config* iniciar_config(void);
void terminar_programa(int, t_log*, t_config*);
void atender_kernel(void* conexion);

//************* REGISTROS *************//

void guardarRegistro(char* registro, char* nuevo_valor, t_registro* registros);
char* leerRegistro(char* registro, t_registro* registros);
void mostrar_por_pantalla(t_registro registro);
uint32_t tamanio_registro(char* registro);

//*************** MMU ***************//

int32_t traducir_direccion(uint32_t direccion_logica, t_pcb *pcb, int conexion_kernel, uint32_t cantidad_bytes);
uint32_t obtener_numero_segmento(uint32_t direccion_logica);
uint32_t obtener_desplazamineto_segmento(uint32_t direccion_logica);

//************** CICLO *************//
void comenzar_ciclo_de_instruccion(t_pcb* pcb, int conexion_con_kernel);
t_instruccion* fetch(t_pcb * pcb);
void decode_y_execute(t_instruccion* instruccion_a_ejecutar, t_pcb* pcb, int conexion_con_kernel);

//************** MEMORIA ********** //
char* MOV_IN_memoria(pid_t pid, uint32_t direccion_fisica, uint32_t registro);
void MOV_OUT_memoria(pid_t pid, uint32_t direccion_fisica, char* valor_leido);
char* consultar_valor_registro(t_registro *registros, char *registro);
void crear_segmento(t_instruccion* instruccion, t_pcb* pcb, int64_t temporizador);
void eliminar_segmento(t_instruccion* instruccion, t_pcb* pcb, int64_t temporizador);
void recibir_creacion(t_pcb* pcb, int64_t temporizador, t_instruccion* instruccion_pedido);
void actualizar_tabla(t_paquete paquete, t_pcb* pcb);



#endif /* CPU_H_ */