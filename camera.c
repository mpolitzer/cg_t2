/**
 *	@file camera.c Camera*: definição de câmeras e cálculos relacionados.
 *
 *	@author
 *			- Maira Noronha
 *			- Thiago Bastos
 *			- Mauricio Carneiro
 *
 *	@date
 *			Criado em:			30 de Novembro de 2002
 *			Última Modificação:	22 de Janeiro de 2003
 *
 *	@version 2.0
 */

#include "camera.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>


/************************************************************************/
/* Constantes Privadas                                                  */
/************************************************************************/
#define M_PI	3.14159265358979323846

/**
 *   Camera no mesmo formato do OpenGL.
 */
struct _Camera
{
	/* Definição da câmera */

    /**
     * Posição da câmera (do observador).
     */
	Vector eye;
	/**
	 * Direção para onde o observador está olhando.
	 */
	Vector at;
	/**
	 * Direção perpendicular a at que indica a orientação vertical da câmera.
	 */
	Vector up;
	/**
	 * Abertura da câmera (ângulo de visão), de 0 a 180.
	 */
	double fovy;
	/**
	 * Distância de eye até nearp (onde a cena será projetada).
	 */
	double nearp;
	/**
	 * Distância de eye até farp (background).
	 */
	double farp;
	/**
	 * Largura da tela em pixels.
	 */
	double screenWidth;
	/**
	 * Altura da tela em pixels
	 */
	double screenHeight;

	/* Estado interno */

	/**
	 * Direção x
	 */
	Vector xAxis;
	/**
	 * Direção y
	 */
	Vector yAxis;
	/**
	 * Direção z
	 */
	Vector zAxis;
	/**
	 * Origem do near plane.
	 */
	Vector nearOrigin;
	/**
	 * Vetor u do near plane.
	 */
	Vector nearU;
	/**
	 * Vetor v do near plane.
	 */
	Vector nearV;
	/**
	 * Origem do far plane.
	 */
	Vector farOrigin;
	/**
	 * Vetor normal ao far plane.
	 */
	Vector farNormal;
	/**
	 * Vetor u do far plane.
	 */
	Vector farU;
	/**
	 * Vetor v do far plane.
	 */
	Vector farV;
};


/************************************************************************/
/* Funções Privadas                                                     */
/************************************************************************/
static void camResize( Camera* camera, int screenWidth, int screenHeight );


/************************************************************************/
/* Definição das Funções Exportadas                                     */
/************************************************************************/
Camera* camCreate( Vector eye, Vector at, Vector up, double fovy, double nearp, double farp,
					int screenWidth, int screenHeight )
{
	Camera* camera = (struct _Camera *)malloc( sizeof(struct _Camera) );

	/* Copia propriedades */
	camera->eye = eye;
	camera->at = at;
	camera->up = up;
	camera->fovy = fovy;
	camera->nearp = nearp;
	camera->farp = farp;

	/* Calcula sistema de coordenadas da câmera */
	camera->zAxis = algUnit( algSub( eye, at ) );
	camera->xAxis = algUnit( algCross( up, camera->zAxis ) );
	camera->yAxis = algCross( camera->zAxis, camera->xAxis );

	/* Inicia estado da câmera com as dimensões especificadas */
	camResize( camera, screenWidth, screenHeight );

	return camera;
}

void camGetFarPlane( Camera* camera, Vector *origin, Vector *normal, Vector *u, Vector *v )
{
	*origin = camera->farOrigin;
	*normal = camera->farNormal;
	*u = camera->farU;
	*v = camera->farV;	
}

Vector camGetEye( Camera* camera )
{
	return camera->eye;
}

Vector camGetRay( Camera* camera, double x, double y )
{
	Vector u = algScale( ( x / camera->screenWidth ), camera->nearU );
	Vector v = algScale( ( y / camera->screenHeight ), camera->nearV );
	Vector point = algAdd( algAdd( camera->nearOrigin, u ), v );

	return algUnit( algSub( point, camera->eye ) );
}

int camGetScreenWidth( Camera* camera )
{
	return (int)camera->screenWidth;
}

int camGetScreenHeight( Camera* camera )
{
	return (int)camera->screenHeight;
}


void camDestroy( Camera* camera )
{
	free( camera );
}


/************************************************************************/
/* Definição das Funções Privadas                                       */
/************************************************************************/
static void camResize( Camera* camera, int screenWidth, int screenHeight )
{
	double sx, sy, sz;
  
	camera->screenWidth = screenWidth;
	camera->screenHeight = screenHeight;

	/* Calcula a origem do near plane */
	sz = camera->nearp;
	sy = ( sz * tan( ( M_PI * camera->fovy ) / ( 2.0 * 180.0 ) ) );
	sx = ( ( sy * screenWidth ) / screenHeight );
	camera->nearOrigin = algLinComb( 4,
								1.0, camera->eye,
								-sz, camera->zAxis,
								-sy, camera->yAxis,
								-sx, camera->xAxis );

	/* Calcula os eixos (u,v) do near plane */
	camera->nearU = algScale( ( 2 * sx ), camera->xAxis );
	camera->nearV = algScale( ( 2 * sy ), camera->yAxis );

	/* Calcula a origem do far plane */
	sz *= ( camera->farp / camera->nearp );
	sy *= ( camera->farp / camera->nearp );
	sx *= ( camera->farp / camera->nearp );
	camera->farOrigin = algLinComb( 4,
								1.0, camera->eye,
								-sz, camera->zAxis,
								-sy, camera->yAxis,
								-sx, camera->xAxis );

	/* Calcula os eixos (u,v) do far plane */
	camera->farU = algScale( ( 2 * sx ), camera->xAxis );
	camera->farV = algScale( ( 2 * sy ), camera->yAxis );
	camera->farNormal = algUnit( algCross( camera->farU, camera->farV ) );
}
