#include "utils-shared.h"

int iniciar_servidor(char *puerto, char* ip, t_log* logger)
{
	
	int socket_servidor = -1;	// Si hay error retornará 

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &servinfo);

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family,
	                         servinfo->ai_socktype,
	                         servinfo->ai_protocol);

	// Asociamos el socket a un puerto

	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

	// Escuchamos las conexiones entrantes

	listen(socket_servidor, SOMAXCONN);


	log_trace(logger, "Listo para escuchar a mi cliente");

	freeaddrinfo(servinfo);


	return socket_servidor;
}

int esperar_cliente(int socket_servidor)
{

	int socket_cliente;

	socket_cliente = accept(socket_servidor, NULL, NULL);

	return socket_cliente;
}

t_paquete *recibir_paquete(int socket, t_log *logger)
{
    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->stream = NULL;
    paquete->buffer->size = 0;
    paquete->codigo_operacion = 0;

    // Primero recibimos el codigo de operacion
    recv(socket, &(paquete->codigo_operacion), sizeof(uint8_t), 0);
    //log_info(logger, "Recibido codigo de operacion: %d", paquete->codigo_operacion);
    // Después ya podemos recibir el buffer. Primero su tamaño seguido del contenido
    recv(socket, &(paquete->buffer->size), sizeof(uint32_t), 0);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    recv(socket, paquete->buffer->stream, paquete->buffer->size, 0);

    return paquete;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	//log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}


int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}


void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}





