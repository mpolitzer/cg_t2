/**
 *	@file light.h Light*: definição e operações com fontes de luz.
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

#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "color.h"
#include "algebra.h"


/************************************************************************/
/* Tipos Exportados                                                     */
/************************************************************************/


typedef struct _Light Light;

/************************************************************************/
/* Funções Exportadas                                                   */
/************************************************************************/
/**
 *	Cria uma fonte de luz não-direcional com as propriedades especificadas.
 *
 *	@param position Posição da fonte de luz.
 *	@param color Cor da luz.
 *
 *	@return Handle para a fonte de luz.
 */
Light* lightCreate( Vector position, Color color );

/**
 *	Obtém a posição em que está localizada uma fonte de luz.
 *
 *	@param light Fonte de luz.
 *
 *	@return Posição da fonte de luz.
 */
Vector lightGetPosition( Light* light );

/**
 *	Obtém a cor de uma fonte de luz.
 *
 *	@param light Fonte de luz.
 *
 *	@return Cor da fonte de luz.
 */
Color lightGetColor( Light* light );

/**
 *	Destrói uma fonte de luz criada com lightCreate().
 *
 *	@param light Fonte de luz.
 */
void lightDestroy( Light* light );

/**
 *	Posiciona uma fonte de luz.
 *
 *	@param light Fonte de luz.
 *	@param position Posição da fonte de luz.
 */
void lightSetPosition( Light* light, Vector position );

/**
 *	Fixa a cor de uma fonte de luz.
 *
 *	@param light Fonte de luz.
 *	@param color Cor da fonte de luz.
 */
void lightSetColor( Light* light, Color color );

#endif

