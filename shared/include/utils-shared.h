#ifndef SHARED_UTILS_H
#define SHARED_UTILS_H


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/temporal.h>
#include <commons/collections/list.h>
#include<commons/config.h>
#include <assert.h>
#include <commons/string.h>
#include <stdint.h>
#include <commons/collections/dictionary.h>

#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <commons/collections/queue.h>
#include <semaphore.h>

#define INITIAL_STREAM_SIZE 64
#define CODIGO_INSTRUCCION 20
#define CODIGO_PCB 80
#define CODIGO_INSTRUCCION_MEMORIA 65
#define ERROR_MEMORIA 40
#define INFO_FS 75
#define COMPACTACION 35

extern t_log* logger;

//typedef enum
//{
//	MENSAJE,
//	PAQUETE,
//	INSTRUCCION,
//	PCB
//}op_code;

typedef struct {
	int size;
	void* stream;
}t_buffer;

typedef struct
{
	uint32_t codigo_operacion;
	t_buffer* buffer;
} t_paquete;

typedef struct{
    int id;
    int base;
    int limite;
} t_segmento;


typedef enum {
	SET,
	MOV_OUT,
	WAIT,
	I_O,
	SIGNAL,
	MOV_IN,
	F_OPEN,
	YIELD,
	F_TRUNCATE,
	F_SEEK,
	CREATE_SEGMENT,
	F_WRITE,
	F_READ,
	F_CREATE,
	DELETE_SEGMENT,
	F_CLOSE,
	EXIT,
	CONTEXTO,
	CREAR_PROCESO,
	ELIMINAR_PROCESO,
	ERROR,
	COMPACTAR,
	F_READ_OK
}t_identificador;

typedef struct{
	t_identificador identificador;
	uint32_t cant_parametros;
	uint32_t param_length1;
	uint32_t param_length2;
	uint32_t param_length3;
	uint32_t param_length4;
	char **parametros;

}t_instruccion;

typedef struct{
    char AX[5], BX[5], CX[5], DX[5];
    char EAX[9], EBX[9], ECX[9], EDX[9];
    char RAX[17], RBX[17], RCX[17], RDX[17];
}t_registro;

typedef enum{
	NUEVO,
	LISTO,
	EJECUTANDO,
	BLOQUEADO,
	TERMINADO,
	SEG_FAULT
}t_estado;

typedef struct{
	int conexion; //para cerrar el proceso 
    pid_t pid;
	int p_counter;
    t_estado estado; //enum
	t_list* instrucciones; // Lista de instrucciones
    t_temporal* tiempo_llegada;		
    int64_t tiempo_ejecucion;
	t_registro registros;	// Contexto de la cpu
	double estimado_rafaga; 
	t_segmento* tabla_segmentos;
	uint32_t tamanio_tabla;
	//archivos
	uint32_t direccion_fisica;
}t_pcb;

typedef struct{
	pid_t pid;
	uint32_t tam_tabla;
	t_segmento* tabla_segmentos;
}t_tabla_memoria;


typedef struct
{
	int numero_segmento;
	int desplazamiento;
}t_direc_fisica;


typedef struct 
{
    pid_t pid;
    int32_t direccion_fisica;
    t_instruccion* instruccion;
}t_fs_pedido;


int crear_conexion(char* ip, char* puerto);
t_paquete* crear_super_paquete(void);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void liberar_conexion(int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
void* recibir_buffer(int* size, int socket_cliente);
void destruir_buffer(t_buffer* buffer);
uint32_t espacio_de_array_parametros(t_instruccion *instruccion);
t_paquete *crear_paquete(t_buffer *buffer, int codigo_operacion);
bool enviar_instrucciones(int socket, t_list *lista_instrucciones, t_log *logger);
t_buffer *crear_buffer_para_t_instruccion(t_instruccion *instruccion);
bool enviar_paquete(int socket, t_paquete *paquete, t_log *logger);
void destruir_paquete(t_paquete *paquete);
t_paquete *recibir_paquete(int socket, t_log *logger);
t_buffer* crear_buffer();
int iniciar_servidor(char *puerto, char* ip, t_log* logger);
int esperar_cliente(int socket_servidor);

t_buffer *null_buffer();
t_list *crear_lista_instrucciones_para_el_buffer( t_buffer *buffer);
t_instruccion *crear_instruccion_para_el_buffer(t_buffer *buffer, uint32_t *offset);
t_buffer *crear_buffer__para_t_lista_instrucciones(t_list *lista_instrucciones);
void enviar_pcb(t_pcb* pcb, int socket, t_log* logger);
t_pcb* crear_pcb(int conexion, t_list* lista_instrucciones, t_estado estado, double estimado_rafaga);
t_buffer* crear_buffer_pcb(t_pcb* pcb, t_log* logger);
t_pcb* recibir_pcb(int socket_cliente, t_log* logger);
t_pcb* deserializar_buffer_pcb(t_buffer* buffer, t_log* logger);
void destruir_pcb(t_pcb* pcb);
void destruir_instruccion(t_instruccion *instruccion);
t_registro inicializar_registros();
pid_t obtener_pid();


t_buffer* crear_buffer_para_t_fs_pedido(t_fs_pedido* fs_pedido);
void enviar_fs_pedido(t_fs_pedido* fs_pedido, int conexion, t_log* logger);
t_fs_pedido* crear_pedido_para_buffer_fs(t_buffer* buffer);

t_instruccion* crear_instruccion(t_identificador identificador, t_list* parametros);
void agregar_parametro_a_instruccion(t_list *parametros, t_instruccion *instruccion);


t_tabla_memoria* deserializar_buffer_para_tabla_memoria(t_buffer* buffer);
void pedir_memoria(t_instruccion* pedido, int socket, t_log* logger);
t_tabla_memoria* recibir_memoria(int socket, t_log* logger);
t_buffer* crear_buffer_tabla_memoria(t_tabla_memoria* nuevo_proceso);
void enviar_procesos(int conexion);
t_instruccion* obtener_instruccion(t_pcb* pcb);


#endif