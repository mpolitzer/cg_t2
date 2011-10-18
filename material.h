/**
 *	@file material.h Material*: manutenção de materiais.
 *
 *	@author
 *			- Maira Noronha
 *			- Thiago Bastos
 *			- Mauricio Carneiro
 *
 *	@date
 *			Criado em:			1 de Dezembro de 2002
 *			Última Modificação:	4 de Junho de 2003
 *
 *	@version 2.0
 */

#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "color.h"
#include "image.h"
#include "algebra.h"


/************************************************************************/
/* Tipos Exportados                                                     */
/************************************************************************/


typedef struct _Material Material;


/************************************************************************/
/* Funções Exportadas                                                   */
/************************************************************************/
/**
 *	Cria um novo material com as propriedades especificadas.
 *
 *	@param texture Imagem contendo textura do material (pode ser NULL).
 *	@param diffusecolor Cor base do material (é substituido pela textura, quando presente).
 *	@param specularColor Cor do brilho especular para este material.
 *	@param specularExponent Coeficiente que define o brilho especular.
 *	@param reflectionFactor Fator de refletividade do material: 0 se não reflete nada,
 *				1 se totalmente reflexivo).
 *	@param refractionFactor Índice de refração do material, para materiais transparentes.
 *	@param opacityFactor Opacidade do material (1 para 100% opaco, 0 para 100% transparente).
 *
 *	@return Handle para o material criado.
 */
Material* matCreate( Image *texture, Color diffuseColor, 
					Color specularColor, double specularExponent,
					double reflectionFactor, double refractionFactor, double opacityFactor );

/**
 *	Obtém a cor difusa para um objeto composto de um material.
 *
 *	@param material Handle para o material do objeto.
 *	@param textureCoordinate Coordenada de textura calculada para o objeto em questão.
 *
 *	@return Cor difusa do objeto num certo ponto.
 */
Color matGetDiffuse( Material* material, Vector textureCoordinate );

/**
 *	Obtém a cor do brilho especular de um material.
 */
Color matGetSpecular( Material* material );

/**
 *	Obtém o coeficiente do brilho especular (o expoente N).
 */
double matGetSpecularExponent( Material* material );

/**
 *	Obtém o fator de refletividade do material: 1.0f para 100% reflexivo.
 */
double matGetReflectionFactor( Material* material );

/**
 *	Obtém o índice de refração de um material (usado para implementar transparência).
 */
double matGetRefractionIndex( Material* material );

/**
 *	Obtém a opacidade de um material: 1.0f para 100% opaco, 0.0f para 100% transparente.
 */
double matGetOpacity( Material* material );

/**
 *	Destrói um material criado com matCreate().
 */
void matDestroy( Material* material );

#endif

