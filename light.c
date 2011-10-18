/**
 *	@file light.c Light*: definição e operações com fontes de luz.
 *
 *	@author 
 *			- Maira Noronha
 *			- Thiago Bastos
 *
 *	@date
 *			Criado em:			01 de Dezembro de 2002
 *			Última Modificação:	22 de Janeiro de 2003
 *
 *	@version 2.0
 */

#include "light.h"
#include <string.h>
#include <stdlib.h>

/**
 *   Luz com posição e intensidade.
 */
struct _Light
{
    /**
     *  Posição da luz.
     */
	Vector position;
	/**
     *  Intensidade da luz em rgb.
     */
	Color color;
};

/************************************************************************/
/* Definição das Funções Exportadas                                     */
/************************************************************************/
Light* lightCreate( Vector position, Color color )
{
	Light* light = (struct _Light *)malloc( sizeof(struct _Light) );

	light->position = position;
	light->color = color;

	return light;
}

Vector lightGetPosition( Light* light )
{
	return light->position;
}

Color lightGetColor( Light* light )
{
	return light->color;
}

void lightDestroy( Light* light )
{
	free( light );
}

void lightSetPosition( Light* light, Vector position )
{
	light->position = position;
}

void lightSetColor( Light* light, Color color )
{
	light->color = color;
}

