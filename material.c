/**
 *	@file material.c Material*: manutenção de materiais.
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

#include "material.h"
#include <string.h>
#include <stdlib.h>

/**
 *   Material* com o qual é feito um objeto.
 */
struct _Material
{
	/**
     *  Textura do material.
     */
	Image *texture;

	/**
     *  Cor base do material (difusa).
     */
	Color diffuseColor;
	/**
     *  Cor do brilho especular do material.
     */
	Color specularColor;

	/**
     *  Coeficiente que define o brilho especular.
     */
	double specularExponent;
	/**
     *  Fator de refletividade do material.
     */
	double reflectionFactor;
	/**
     *  Índice de refração do material.
     */
	double refractionFactor;
	/**
     *  Opacidade do material.
     */
	double opacityFactor;
};

/************************************************************************/
/* Definição das Funções Exportadas                                     */
/************************************************************************/
Material* matCreate( Image *texture, Color diffuseColor, 
					Color specularColor, double specularExponent,
					double reflectionFactor, double refractionFactor, double opacityFactor )
{
	Material* material = (struct _Material *)malloc( sizeof(struct _Material) );
	
	material->texture = texture;
	material->diffuseColor = diffuseColor;
	material->specularColor = specularColor;
	material->specularExponent = specularExponent;
	material->reflectionFactor = reflectionFactor;
	material->refractionFactor = refractionFactor;
	material->opacityFactor = opacityFactor;

	return material;
}

Color matGetDiffuse( Material* material, Vector textureCoordinate )
{
	int x;
	int y;
	int width;
	int height;

	if( material->texture == NULL )
	{
		return material->diffuseColor;
	}

	width = imgGetWidth(material->texture);
	height = imgGetHeight(material->texture);

	//imageGetDimensions( material->texture, &width, &height );
	
	x = ( (int)( textureCoordinate.x * ( width - 1 ) ) % width );
	y = ( (int)( textureCoordinate.y * ( height - 1 ) ) % height );

	return imageGetPixel( material->texture, x, y );
}

Color matGetSpecular( Material* material )
{
    return material->specularColor;
}

double matGetSpecularExponent( Material* material )
{
    return material->specularExponent;
}

double matGetReflectionFactor( Material* material )
{
   return material->reflectionFactor;
}

double matGetRefractionIndex( Material* material )
{
	return material->refractionFactor;
}

double matGetOpacity( Material* material )
{
	return material->opacityFactor;
}

void matDestroy( Material* material )
{
	free( material );
}

