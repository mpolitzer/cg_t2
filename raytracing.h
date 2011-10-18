/**
 *	@file raytracing.h RayTracing: renderização de cenas por raytracing.
 *
 *	@author 
 *			- Maira Noronha
 *			- Thiago Bastos
 *
 *	@date
 *			Criado em:			02 de Dezembro de 2002
 *			Última Modificação:	7 de Setembro de 2004
 *
 *	@version 2.0
 */

#ifndef _RAYTRACING_H_
#define _RAYTRACING_H_

#include "scene.h"
#include "algebra.h"
#include "color.h"


/************************************************************************/
/* Funções Exportadas                                                   */
/************************************************************************/
/**
 *	Calcula a cor correspondente ao raio que parte de eye na direcao ray.
 *
 *	@param scene Handle para cena.
 *	@param eye   vetor de posicao da origem do raio.
 *  @param ray   vetor de direcao do raio.
 *  @param depth nivel de recursao do raio (inicialmente deve ser passado como 0).
 *
 *	@return cor  correspondente ao raio.
 */
Color rayTrace( Scene* scene, Vector eye, Vector ray, int depth );
#endif

