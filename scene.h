/**
 *	@file scene.h Scene: definição e manutenção de cenas.
 *
 *	@author 
 *			- Maira Noronha
 *			- Thiago Bastos
 *
 *	@date
 *			Criado em:			02 de Dezembro de 2002
 *			Última Modificação:	4 de Junho de 2003
 *
 *	@version 2.0
 */

#ifndef _SCENE_H_
#define _SCENE_H_

#include "image.h"
#include "light.h"
#include "camera.h"
#include "object.h"
#include "material.h"


/************************************************************************/
/* Constantes Exportadas                                                */
/************************************************************************/
#define MAX_MATERIALS	64
#define MAX_OBJECTS		128
#define MAX_LIGHTS		100
#define FILENAME_MAXLEN	64

#ifndef EPSILON
#define EPSILON	1.0e-10
#endif


/************************************************************************/
/* Tipos Exportados                                                     */
/************************************************************************/

typedef struct _Scene Scene;


/************************************************************************/
/* Funções Exportadas                                                   */
/************************************************************************/
/**
 *	Obtém uma cor de fundo traçando um raio dentro de uma cena.
 *	É necessário que uma câmera tenha sido definida para a cena.
 *	Se o raio atingir o far plane e houver uma imagem de fundo, a cor é retirada dessa
 *	imagem. Senão, a cor de fundo padrão é retornada.
 *
 *	@param scene Handle para uma cena.
 *	@param eye Origem do raio.
 *	@param ray Direção do raio.
 *
 *	@return Cor de fundo para o raio fornecido na cena fornecida.
 */
Color sceGetBackgroundColor( Scene* scene, Vector eye, Vector ray );

/**
 *	Obtém a luz ambiente de uma cena.
 */
Color sceGetAmbientLight( Scene* scene );

/**
 *	Obtém a câmera de uma cena.
 */
Camera* sceGetCamera( Scene* scene );

/**
 *	Obtém o número de objetos existentes em uma cena.
 *
 *	@param scene Handle para uma cena.
 *
 *	@return Número de objetos na cena.
 */
int sceGetObjectCount( Scene* scene );

/**
 *	Obtém um objeto de uma cena.
 *
 *	@param scene Handle para uma cena.
 *	@param index Índice do objeto a ser obtido (de 0 a objectCount - 1).
 *
 *	@return Objeto no índice especificado (NULL se o índice for inválido).
 */
Object* sceGetObject( Scene* scene, int index );

/**
 *	Obtém o número de fontes de luz existentes em uma cena.
 *
 *	@param scene Handle para uma cena.
 *	
 *	@return Número de luzes na cena.
 */
int sceGetLightCount( Scene* scene );

/**
 *	Obtém uma fonte de luz de uma cena.
 *
 *	@param scene Handle para uma cena.
 *	@param index Índice da fonte de luz a ser obtida (de 0 a lightCount - 1).
 *
 *	@return A fonte de luz no índice especificado (NULL se o índice for inválido).
 */
Light* sceGetLight( Scene* scene, int index );

/**
 *	Lê uma cena a partir de um arquivo em formato rt4.
 *
 *	@param filename nome do arquivo que contém a cena.
 *
 *	@return Cena criada (NULL se o arquivo for inválido).
 */
Scene* sceLoad( const char *filename );

/**
 *	Obtém o número de materiais de uma cena.
 */
int sceGetMaterialCount( Scene* scene );

/**
 *	Obtém um material de uma cena.
 *
 *	@param scene Handle para uma cena.
 *	@param index Índice do material (de 0 a materialCount - 1).
 *
 *	@return O material no índice especificado (NULL se o índice for inválido).
 */
Material* sceGetMaterial( Scene* scene, int index );

/**
 *	Destrói uma cena.
 */
void sceDestroy( Scene* scene );

#endif

