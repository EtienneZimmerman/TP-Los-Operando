#include "CPU.h"


void guardarRegistro(char* registro, char* nuevo_valor, t_registro* registros){

	if (string_equals_ignore_case(registro, "AX"))
	{
		memcpy(registros->AX, nuevo_valor, sizeof(registros->AX) - 1);
        registros->AX[sizeof(registros->AX) - 1] = '\0';
	} else if (string_equals_ignore_case(registro, "BX"))
	{
		memcpy(registros->BX, nuevo_valor, sizeof(registros->BX) - 1);
        registros->BX[sizeof(registros->BX) - 1] = '\0';
	}else if (string_equals_ignore_case(registro, "CX"))
	{
		memcpy(registros->CX, nuevo_valor, sizeof(registros->CX) - 1);
        registros->CX[sizeof(registros->CX) - 1] = '\0';
	} else if (string_equals_ignore_case(registro, "DX"))
	{
		memcpy(registros->DX, nuevo_valor, sizeof(registros->DX) - 1);
        registros->DX[sizeof(registros->DX) - 1] = '\0';
	} else if (string_equals_ignore_case(registro, "EAX"))
	{
		memcpy(registros->EAX, nuevo_valor, sizeof(registros->EAX) - 1);
        registros->EAX[sizeof(registros->EAX) - 1] = '\0';
	} else if (string_equals_ignore_case(registro, "EBX"))
	{
		memcpy(registros->EBX, nuevo_valor, sizeof(registros->EBX) - 1);
        registros->EBX[sizeof(registros->EBX) - 1] = '\0';
	} else if (string_equals_ignore_case(registro, "ECX"))
	{
		memcpy(registros->ECX, nuevo_valor, sizeof(registros->ECX) - 1);
        registros->ECX[sizeof(registros->ECX) - 1] = '\0';
	} else if (string_equals_ignore_case(registro, "EDX"))
	{
		memcpy(registros->EDX, nuevo_valor, sizeof(registros->EDX) - 1);
        registros->EDX[sizeof(registros->EDX) - 1] = '\0';
	} else if (string_equals_ignore_case(registro, "RAX"))
	{
		memcpy(registros->RAX, nuevo_valor, sizeof(registros->RAX) - 1);
        registros->RAX[sizeof(registros->RAX) - 1] = '\0';
	} else if (string_equals_ignore_case(registro, "RBX"))
	{
		memcpy(registros->RBX, nuevo_valor, sizeof(registros->RBX) - 1);
        registros->RBX[sizeof(registros->RBX) - 1] = '\0';
	} else if (string_equals_ignore_case(registro, "RCX"))
	{
		memcpy(registros->RCX, nuevo_valor, sizeof(registros->RCX) - 1);
        registros->RCX[sizeof(registros->RCX) - 1] = '\0';
	} else if (string_equals_ignore_case(registro, "RDX"))
	{
		memcpy(registros->RDX, nuevo_valor, sizeof(registros->RDX) - 1);
        registros->RDX[sizeof(registros->RDX) - 1] = '\0';
	}

}

void mostrar_por_pantalla(t_registro registro)
{
	printf("----------------------------------------------\n");
	printf("valores registros: \n");
	printf("- AX  = %s \n", registro.AX );
	printf("- BX  = %s \n", registro.BX );
	printf("- CX  = %s \n", registro.CX );
	printf("- DX  = %s \n", registro.DX );
	printf("- EAX = %s \n", registro.EAX);
	printf("- EBX = %s \n", registro.EBX);
	printf("- ECX = %s \n", registro.ECX);
	printf("- EDX = %s \n", registro.EDX);
	printf("- RAX = %s \n", registro.RAX);
	printf("- RBX = %s \n", registro.RBX);
	printf("- RCX = %s \n", registro.RCX);
	printf("- RDX = %s \n", registro.RDX);
	printf("----------------------------------------------\n");
}

uint32_t tamanio_registro(char* registro)
{
	
	if(! string_equals_ignore_case(registro,"AX")){
		return 4;
	}
	if(! string_equals_ignore_case(registro,"BX")){
		return 4;
	}
	if(! string_equals_ignore_case(registro,"CX")){
		return 4;
	}
	if(! string_equals_ignore_case(registro,"DX")){
		return 4;
	}
	if(! string_equals_ignore_case(registro,"EAX")){
		return 8;
	}
	if(! string_equals_ignore_case(registro,"EBX")){
		return 8;
	}
	if(! string_equals_ignore_case(registro,"ECX")){
		return 8;
	}
	if(! string_equals_ignore_case(registro,"EDX")){
		return 8;
	}
	if(! string_equals_ignore_case(registro,"RAX")){
		return 16;
	}
	if(! string_equals_ignore_case(registro,"RBX")){
		return 16;
	}
	if(! string_equals_ignore_case(registro,"RCX")){
		return 16;
	}
	if(! string_equals_ignore_case(registro,"RDX")){
		return 16;
	}
}

char* consultar_valor_registro(t_registro *registros, char *registro) { 
	if (string_equals_ignore_case(registro, "AX"))
	{
		return registros->AX;
	} 
	if (string_equals_ignore_case(registro, "BX"))
	{
		return registros->BX;
	}
	if (string_equals_ignore_case(registro, "CX"))
	{
		return registros->CX;
	}
	if (string_equals_ignore_case(registro, "DX"))
	{
		return registros->DX;
	}
	if (string_equals_ignore_case(registro, "EAX"))
	{
		return registros->EAX;
	}
	if (string_equals_ignore_case(registro, "EBX"))
	{
		return registros->EBX;
	}
	if (string_equals_ignore_case(registro, "ECX"))
	{
		return registros->ECX;
	}
	if (string_equals_ignore_case(registro, "EDX"))
	{
		return registros->EDX;
	}
	if (string_equals_ignore_case(registro, "RAX"))
	{
		return registros->RAX;
	}
	if (string_equals_ignore_case(registro, "RBX"))
	{
		return registros->RBX;
	}
	if (string_equals_ignore_case(registro, "RCX"))
	{
		return registros->RCX;
	}
	if (string_equals_ignore_case(registro, "RDX"))
	{
		return registros->RDX;
	}
}