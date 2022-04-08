#include "estructuras.h"
#include "sala.h"
#include "objeto.h"
#include "interaccion.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINEA 1028
#define ERROR -1


int crear_interacciones(sala_t *sala, int *tamanio_interacciones, FILE *arch_interacciones)
{
	char *string_interaccion = malloc(LINEA * sizeof(char));
	if(string_interaccion == NULL)
		return ERROR;
	
	int elemento_leido = fscanf(arch_interacciones, "%[^\n]\n", string_interaccion);

	if(elemento_leido != 1)
		return ERROR;

	struct interaccion *interaccion_aux;
	struct interaccion **interacciones_aux;

	while(elemento_leido == 1){

		interaccion_aux = interaccion_crear_desde_string(string_interaccion);

		interacciones_aux = realloc(sala->interacciones, (((long unsigned int)(*tamanio_interacciones)+1) * sizeof(struct interaccion*)));
		if(interacciones_aux == NULL)
			return ERROR;
		
		interacciones_aux[*tamanio_interacciones] = interaccion_aux;
		(*tamanio_interacciones)++;
		sala->interacciones = interacciones_aux;

		elemento_leido = fscanf(arch_interacciones, "%[^\n]\n", string_interaccion);
	}

	sala->cantidad_interacciones = *tamanio_interacciones;

	free(string_interaccion);

	return 0;
}

int crear_objetos(sala_t *sala, int *tamanio_objetos, FILE *arch_objetos)
{
	char *string_objeto = malloc(LINEA * sizeof(char));
	if(string_objeto == NULL)
		return ERROR;

	int elemento_leido = fscanf(arch_objetos, "%[^\n]\n", string_objeto);

	if(elemento_leido != 1)
		return ERROR;

	struct objeto *objeto_auxiliar;
	struct objeto **objetos_aux;
	
	while(elemento_leido == 1){

		objeto_auxiliar = objeto_crear_desde_string(string_objeto);

		objetos_aux = realloc(sala->objetos, (((long unsigned int)(*tamanio_objetos)+1) * sizeof(struct objeto*)));;
		if(objetos_aux == NULL)
			return ERROR;

		objetos_aux[*tamanio_objetos] = objeto_auxiliar;
		(*tamanio_objetos)++;
		sala->objetos = objetos_aux;

		elemento_leido = fscanf(arch_objetos, "%[^\n]\n", string_objeto);
	}

	sala->cantidad_objetos = (*tamanio_objetos);
	
	if(sala->objetos == NULL)
		return ERROR;
	
	free(string_objeto);

	return 0;
}

/* 
 * Verifica que los archivos recibidos hayan sido abiertos correctamente
 *
 * En el caso de que uno de los dos o ambos no se hayan podido abrir, devuelve -1.
 * 
 * Si ambos fueron abiertos correctamente devuelve 0
 * 
*/
int verificar_apertura_archivos(FILE *arch_objetos, FILE *arch_interacciones)
{
	if(arch_objetos == NULL){
		perror("No se pudo abrir el archivo de objetos\n");
		return -1;
	}
	if(arch_interacciones == NULL){
		perror("No se pudo abrir el archivo de interacciones\n");
		fclose(arch_objetos);
		return -1;
	}
	return 0;
}


sala_t *sala_crear_desde_archivos(const char *objetos, const char *interacciones)
{

	FILE *arch_objetos = fopen(objetos, "r");
	FILE *arch_interacciones = fopen(interacciones, "r");

	if(verificar_apertura_archivos(arch_objetos, arch_interacciones) == -1)
		return NULL;
	

	sala_t *sala = malloc(sizeof(sala_t));	
	if(sala == NULL){
		printf("No se reservó memoria para sala\n");
		return NULL;
	} 

	//OBJETOS
	int tamanio_objetos = 0;
	
	sala->objetos = NULL;
	
	if(crear_objetos(sala, &tamanio_objetos, arch_objetos) == -1)
		return NULL;
		
	if(sala->objetos == NULL)
		return NULL;

	//INTERACCIONES
	
	sala->interacciones = NULL;

	int tamanio_interacciones = 0;

	if(crear_interacciones(sala, &tamanio_interacciones, arch_interacciones) == -1)
		return NULL;

	

	if(sala->interacciones == NULL)
		return NULL;


	fclose(arch_objetos);
	fclose(arch_interacciones);
	return sala;

}


char **sala_obtener_nombre_objetos(sala_t *sala, int *cantidad)
{
	*cantidad = sala->cantidad_objetos;
	char **string = malloc((long unsigned int) ((*cantidad)+1) * sizeof(char*));
	if(string == NULL){
		*cantidad = -1;
		return NULL;
	}	
	
	char *nombre; 

	for(int i = 0; i < *cantidad; i++){
		nombre = malloc(LINEA * sizeof(char));
		strcpy(nombre, sala->objetos[i]->nombre);
		string[i] = nombre;		
	}


	return string;

}


bool sala_es_interaccion_valida(sala_t *sala, const char *verbo, const char *objeto1, const char *objeto2)
{
	int contador = 0;

	for(int i = 0; i < sala->cantidad_interacciones; i++){
		if(strcmp(sala->interacciones[i]->verbo, verbo) == 0)
			contador++;

		if(strcmp(sala->interacciones[i]->objeto, objeto1) == 0)
			contador++;

		if(strcmp(sala->interacciones[i]->objeto_parametro, objeto2) == 0)
			contador++;

		if(contador == 3)
			return true;
		
		contador = 0;
	}

	return false;
}


void sala_destruir(sala_t *sala)
{
	for(int i = 0; i < sala->cantidad_objetos; i++){
		free(sala->objetos[i]);
	}

	for(int i = 0; i < sala->cantidad_interacciones; i++){
		free(sala->interacciones[i]);
	}
	
	free(sala->interacciones);
		
	free(sala);
}