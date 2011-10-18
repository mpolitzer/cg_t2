/**
 *	@file raytracing.c RayTracing: renderização de cenas por raytracing.
 *
 *	@date
 *			Criado em:			   02 de Dezembro de 2002
 *			Última Modificação:	   04 de Outubro de 2009
 *
 */

#include <math.h>
#include <float.h>
#include <string.h>
#include <stdlib.h>

#include "raytracing.h"
#include "color.h"
#include "algebra.h"


/************************************************************************/
/* Constantes Privadas                                                  */
/************************************************************************/
#define MAX( a, b ) ( ( a > b ) ? a : b )

/** Número máximo de recursões permitidas */
#define MAX_DEPTH	6


/************************************************************************/
/* Funções Privadas                                                     */
/************************************************************************/
/**
 *	Obtém uma cor através do traçado de um raio dentro de uma cena.
 *
 *	@param scene Handle para a cena sendo renderizada.
 *	@param eye Posição do observador, origem do raio.
 *	@param ray Direção do raio.
 *	@param depth Para controle do número máximo de recursões. Funções clientes devem
 *					passar 0 (zero). A cada recursão depth é incrementado até, no máximo,
 *					MAX_DEPTH. Quando MAX_DEPTH é atingido, recursões são ignoradas.
 *
 *	@return Cor resultante do traçado do raio.
 */
static Color shade( Scene* scene, Vector eye, Vector ray, Object* object, Vector point,
					     Vector normal, int depth );

/**
 *	Encontra o primeiro objeto interceptado pelo raio originado na posição especificada.
 *
 *	@param scene Cena.
 *	@param eye Posição do Observador (origem).
 *	@param ray Raio sendo traçado (direção).
 *	@param object Onde é retornado o objeto resultante. Não pode ser NULL.
 *	@return Distância entre 'eye' e a superfície do objeto interceptado pelo raio.
 *			DBL_MAX se nenhum objeto é interceptado pelo raio, neste caso
 *				'object' não é modificado.
 */
static double getNearestObject( Scene* scene, Vector eye, Vector ray, Object* *object );

/**
 *	Checa se objetos em uma cena impedem a luz de alcançar um ponto.
 *
 *	@param scene Cena.
 *	@param point Ponto sendo testado.
 *	@param rayToLight Um raio (direção) indo de 'point' até 'lightLocation'.
 *	@param lightLocation Localização da fonte de luz.
 *	@return Zero se nenhum objeto bloqueia a luz e não-zero caso contrário.
 */
static int isInShadow( Scene* scene, Vector point, Vector rayToLight, Vector lightLocation );


/************************************************************************/
/* Definição das Funções Exportadas                                     */
/************************************************************************/

Color rayTrace( Scene* scene, Vector eye, Vector ray, int depth )
{
	Object* object;
	double distance;

	Vector point;
	Vector normal;

	/* Calcula o primeiro objeto a ser atingido pelo raio */
	distance = getNearestObject( scene, eye, ray, &object );

	/* Se o raio não interceptou nenhum objeto... */
	if( distance == DBL_MAX )
	{
		return sceGetBackgroundColor( scene, eye, ray );
	}

	/* Calcula o ponto de interseção do raio com o objeto */
	point = algAdd( eye, algScale( distance, ray ) );

	/* Obtém o vetor normal ao objeto no ponto de interseção */
	normal =  objNormalAt( object, point );

	return shade( scene, eye, ray, object, point, normal, depth );
}

/************************************************************************/
/* Definição das Funções Privadas                                       */
/************************************************************************/
static Color shade( Scene* scene, Vector eye, Vector ray, Object* object, Vector point,
				   Vector normal, int depth )
{
	Material* material = sceGetMaterial(scene,objGetMaterial(object));
	double reflectionFactor = matGetReflectionFactor( material );
	double specularExponent = matGetSpecularExponent( material );
	double refractedIndex   = matGetRefractionIndex( material );
	double opacity = matGetOpacity( material );
	Color ambient = sceGetAmbientLight( scene );
	Color diffuse = matGetDiffuse( material, objTextureCoordinateAt( object, point ) );	
	Color specular = matGetSpecular( material );

	int nlights;
	int i;
	double cos, sin;
	Vector V;
	Vector Rr, Rt;
	Color colorRr, colorRt;
	Vector vt, T;

	/* Começa com a cor ambiente */
	Color color = colorMultiplication( diffuse, ambient );

	/* Adiciona a componente difusa */
	nlights = sceGetLightCount(scene);  /* numero de luzes na cena */
	for (i=0; i<nlights; i++) {
		Light *light     = sceGetLight(scene,i);  /* luz i da cena */
		Color lightcolor = lightGetColor(light);    /* cor da luz i */
		Vector lightpos  = lightGetPosition(light); /* posicao da luz i */
		Vector L         = algUnit(algSub(lightpos,point));  /* vetor do ponto para a luz i */
		cos              = algDot(L,algUnit(normal));        /* cosseno com a normal */
		if (cos>0 && isInShadow(scene,point,L,lightpos) == 0)   /* se for visivel para a luz */
			color = colorAddition(color,colorReflection(cos,lightcolor,diffuse));
	}

	/*Componente especular*/
	V  = algUnit(algMinus(ray));
	for (i=0; i<nlights; i++) {
		Light *light     = sceGetLight(scene,i);   /* ponteiro da luz i */
		Color lightcolor = lightGetColor(light);   /* cor da luz i */
		Vector lightpos  = lightGetPosition(light);  /* posicao da luz i */
		Vector L         = algUnit(algSub(lightpos,point));  /* aponta para a Luz i */
		Vector r         = algReflect(L, algUnit(normal));   /* reflexão da luz i na normal */
		cos              = algDot(r,V);                            
		if (cos >0 && isInShadow(scene,point,L,lightpos) == 0)
			color = colorAddition(color,colorReflection(pow(cos,specularExponent),lightcolor,specular));
	}


	depth ++;

	/*Reflexão*/ 
	Rr = algReflect(V,algUnit(normal));
	if ((reflectionFactor>0.001)&&(depth < MAX_DEPTH))
	{
		colorRr = rayTrace (scene,point,Rr,depth);
		colorRr = colorScale(reflectionFactor,colorRr);
		color = colorAddition(color,colorRr);
	}


	/*Transparência */ 
	if(((1-opacity)>0.001)&&(depth < MAX_DEPTH))
	{
		vt = algSub(algProj(V,algUnit(normal)),V);
		sin = (1.0/refractedIndex)*algNorm(vt);
		cos = sqrt(1.-sin*sin);
		T   = algUnit(vt);
		Rt  = algAdd(algScale(sin,T),algScale(-cos,algUnit(normal)));
		//Rt=algMinus(V);
		colorRt = rayTrace(scene,point,Rt,depth);
		color = colorAddition(color,colorScale(1-opacity,colorRt));
	}
	return color;
}

static double getNearestObject( Scene* scene, Vector eye, Vector ray, Object** object )
{
	int i;
	int objectCount = sceGetObjectCount( scene );

	double closest = DBL_MAX;

	/* Para cada objeto na cena */
	for( i = 0; i < objectCount; ++i ) {
		Object* currentObject = sceGetObject( scene, i );
		double distance = objIntercept( currentObject, eye, ray );
		
		if( distance > 0.001 && distance < closest )   /* 0.001 e' uma tolerancia (autointersecao) */
		{
			closest = distance;
			*object = currentObject;
		}
	}

	return closest;
}

static int isInShadow( Scene* scene, Vector point, Vector rayToLight, Vector lightLocation )
{
	int i;
	int objectCount = sceGetObjectCount( scene );

	/* maxDistance = distância de point até lightLocation */
	double maxDistance = algNorm( algSub( lightLocation, point ) );

	/* Para cada objeto na cena */
	for( i = 0; i < objectCount; ++i )
	{
		double distance = objIntercept( sceGetObject( scene, i ), point, rayToLight );
		
		if( distance > 0.1 && distance < maxDistance )
		{
			return 1;
		}
	}

	return 0;
}

