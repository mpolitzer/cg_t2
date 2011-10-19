/**
 *	@file scene.c Scene: definição e manutenção de cenas.
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

#include "scene.h"
#include "raytracing.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/timeb.h>


/**
 *   Cena com a camera, os objetos, as luzes e a imagem/cor de fundo.
 */
struct _Scene
{
	/**
     *  Camera* da cena
     */
	Camera* camera;

    /**
     *  Cor de fundo da cena
     */
	Color bgColor;
	/**
     *  Imagem de fundo da cena
     */
	Image* bgImage;

	/**
     *  Número de materiais existentes na cena.
     */
	int materialCount;
	/**
     *  Vetor com os materiais existentes na cena.
     */
	Material* materials[MAX_MATERIALS];

	/**
     *  Número de objetos existentes na cena.
     */
	int objectCount;
	/**
     *  Vetor com os objetos existentes na cena.
     */
	Object* objects[MAX_OBJECTS];

	/**
     *  Intensidade rgb da luz ambiente da cena
     */
	Color ambientLight;
	/**
     *  Número de fontes de luz existentes na cena.
     */
	int lightCount;
	/**
     *  Vetor com as fontes de luz existentes na cena.
     */
	Light* lights[MAX_LIGHTS];
};

/************************************************************************/
/* Definição das Funções Exportadas                                     */
/************************************************************************/
Color sceGetBackgroundColor( Scene* scene, Vector eye, Vector ray )
{
	double planeDistance, divisor, distance, scaleU, scaleV;
	Vector farOrigin, farNormal, farU, farV, point, pointFromOrigin;

	if( scene->bgImage == NULL || scene->camera == NULL )
	{
		return scene->bgColor;
	}

	/* Obtém informações sobre o far plane */
	camGetFarPlane( scene->camera, &farOrigin, &farNormal, &farU, &farV );

	/* Cos(alpha) entre a normal do plano e o raio */
	divisor = algDot( ray, farNormal );

	/* Se o raio se distancia ou é paralelo ao far plane */
	if( divisor > 0 || -divisor < EPSILON )
	{
		return scene->bgColor;
	}

	planeDistance = algDot( farOrigin, farNormal );

	distance = ( ( planeDistance - algDot( eye, farNormal ) ) / divisor );

	/* Se o raio se distancia do far plane */
	if( distance < 0 )
	{
		return scene->bgColor;
	}

	point = algAdd( eye, algScale( distance, ray ) );

	pointFromOrigin = algSub( point, farOrigin );

	divisor = algDot( farU, farU );
	scaleU = ( algDot( farU, pointFromOrigin ) / divisor );

	divisor = algDot( farV, farV );
	scaleV = ( algDot( farV, pointFromOrigin ) / divisor );

	/* Se o raio não intercepta o far plane (ou seja, a imagem de fundo)... */
	if( scaleU < 0 || scaleV < 0 || scaleU > 1 || scaleV > 1 )
	{
		return scene->bgColor;
	}
	
	return imageGetPixel( scene->bgImage,
							(int)( scaleU * camGetScreenWidth( scene->camera ) ),
							(int)( scaleV * camGetScreenHeight( scene->camera ) ) );
}

Color sceGetAmbientLight( Scene* scene )
{
	return scene->ambientLight;
}

Camera* sceGetCamera( Scene* scene )
{
    return scene->camera;
}

int sceGetObjectCount( Scene* scene )
{
	return scene->objectCount;
}

Object* sceGetObject( Scene* scene, int index )
{
	if( index < 0 || index >= scene->objectCount )
	{
		return NULL;
	}

	return scene->objects[index];
}

int sceGetLightCount( Scene* scene )
{
	return scene->lightCount;
}

Light* sceGetLight( Scene* scene, int index )
{
	if( index < 0 || index >= scene->lightCount )
	{
		return NULL;
	}

	return scene->lights[index];
}

Scene* sceLoad( const char *filename )
{
	FILE *file;
	char buffer[512];

	Scene* scene;

	/* indices dos objetos para btree e qual operação será realizada. */
	int obj1, obj2, op;
	
	Color bgColor;
	Color ambientLight;
	char backgroundFileName[FILENAME_MAXLEN];

	/* Camera* */
	Vector eye = algVector( 0,0,0,1 );
	Vector at = algVector( 0,0,0,1 );
	Vector up = algVector( 0,0,0,1 );
	double fovy;
	double nearp;
	double farp;
	int screenWidth;
	int screenHeight;

	/* Material* */
	Color diffuse;
	Color specular;
	double specularExponent;
	double reflective;
	double refractive;
	double opacity;
	char textureFileName[FILENAME_MAXLEN];
	
	/* Lights & Objects */
	Color lightColor;
	int material;
	Vector pos1 = algVector( 0,0,0,1 );
	Vector pos2 = algVector( 0,0,0,1 );
	Vector pos3 = algVector( 0,0,0,1 );
	Vector tex1 = algVector( 0,0,0,1 );
	Vector tex2 = algVector( 0,0,0,1 );
	Vector tex3 = algVector( 0,0,0,1 );
	double radius;
	
	file = fopen( filename, "rt" );
	if( !file )
	{
		return NULL;
	}

	scene = (struct _Scene *)malloc( sizeof(struct _Scene) );
	if( !scene )
	{
		return NULL;
	}

	/* Default (undefined) values: */
	scene->camera = NULL;
	scene->bgImage = NULL;
	scene->objectCount = 0;
	scene->lightCount = 0;
	scene->materialCount = 0;
	
	while( fgets( buffer, sizeof(buffer), file ) ) 
	{
		if( sscanf( buffer, "RT %lf\n", &at ) == 1 )
		{
			/* Ignore File Version Information */
		}
		else if( sscanf( buffer, "CAMERA %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %d %d\n", &eye.x, &eye.y, &eye.z, &at.x, &at.y, &at.z, &up.x, &up.y, &up.z, &fovy, &nearp, &farp, &screenWidth, &screenHeight ) == 14) 
		{
			if( scene->camera )
			{
				camDestroy( scene->camera );
			}

			scene->camera = camCreate( eye, at, up, fovy, nearp, farp, screenWidth, screenHeight );
		} 
		else if( sscanf( buffer, "SCENE %f %f %f %f %f %f %s\n", &bgColor.red, &bgColor.green, &bgColor.blue, &ambientLight.red, &ambientLight.green, &ambientLight.blue, backgroundFileName ) == 7 ) 
		{
			bgColor = colorNormalize( bgColor );
			ambientLight = colorNormalize( ambientLight );

			scene->bgColor = bgColor;
			scene->ambientLight = ambientLight;

			if( scene->bgImage )
			{
				imgDestroy( scene->bgImage );
			}

			if( strcmp( backgroundFileName, "null") == 0 )
			{
				scene->bgImage = NULL;
			} 
			else 
			{
				scene->bgImage = imgReadBMP (backgroundFileName);
			}
		} 
		else if( sscanf( buffer, "MATERIAL %f %f %f %f %f %f %lf %lf %lf %lf %s\n", &diffuse.red, &diffuse.green, &diffuse.blue, &specular.red, &specular.green, &specular.blue, &specularExponent, &reflective, &refractive, &opacity, textureFileName ) == 11 ) 
		{
			Image *image = NULL;
			
			if( strcmp( textureFileName, "null") != 0 )
			{
				image = imgReadBMP (textureFileName);
			}

			if( scene->materialCount >= MAX_MATERIALS )
			{
				imgDestroy( image );
				fprintf( stderr, "sceLoad: Foi ultrapassado o limite de definicoes de materiais na cena. Ignorando." );
				continue;
			}

			diffuse = colorNormalize( diffuse );
			specular = colorNormalize( specular );

			scene->materials[scene->materialCount++] = matCreate( image, diffuse, specular, specularExponent, reflective, refractive, opacity );
		} 
		else if( sscanf( buffer, "LIGHT %lf %lf %lf %f %f %f\n", &pos1.x, &pos1.y, &pos1.z, &lightColor.red, &lightColor.green, &lightColor.blue ) == 6 )
		{
			if( scene->lightCount >= MAX_LIGHTS )
			{
				fprintf( stderr, "sceLoad: Foi ultrapassado o limite de definicoes de luzes na cena. Ignorando." );
				continue;
			}

			lightColor = colorNormalize( lightColor );

			scene->lights[scene->lightCount++] = lightCreate( pos1, lightColor );
		} 
		else if( sscanf( buffer, "SPHERE %d %lf %lf %lf %lf\n", &material, &radius, &pos1.x,&pos1.y,&pos1.z ) == 5 ) 
		{
			if( scene->objectCount >= MAX_OBJECTS )
			{
				fprintf( stderr, "sceLoad: Foi ultrapassado o limite de definicoes de objetos na cena. Ignorando." );
				continue;
			}

		scene->objects[scene->objectCount++] = objCreateSphere( material, pos1, radius );
		} 
		else if( sscanf( buffer, "TRIANGLE %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n", &material, &pos1.x, &pos1.y, &pos1.z, &pos2.x, &pos2.y, &pos2.z, &pos3.x, &pos3.y, &pos3.z, &tex1.x, &tex1.y, &tex2.x, &tex2.y, &tex3.x, &tex3.y) == 16 ) 
		{
			if( scene->objectCount >= MAX_OBJECTS )
			{
				fprintf( stderr, "sceLoad: Foi ultrapassado o limite de definicoes de objetos na cena. Ignorando." );
				continue;
			}
			
			scene->objects[scene->objectCount++] = objCreateTriangle( material, pos1, pos2, pos3, tex1, tex2, tex3 );
		}
	  	else if( sscanf( buffer, "BOX %d %lf %lf %lf %lf %lf %lf\n", &material, &pos1.x, &pos1.y, &pos1.z, &pos2.x, &pos2.y, &pos2.z ) == 7 ) 
		{
			if( scene->objectCount >= MAX_OBJECTS )
			{
				fprintf( stderr, "sceLoad: Foi ultrapassado o limite de definicoes de objetos na cena. Ignorando." );
				continue;
			}

			scene->objects[scene->objectCount++] = objCreateBox( material, pos1, pos2 );
		} 
		else if( sscanf( buffer, "MESH %d %lf %lf %lf %lf %lf %lf %s\n", &material, &pos1.x, &pos1.y, &pos1.z, &pos2.x, &pos2.y, &pos2.z, buffer ) == 8 ) 
		{
			if( scene->objectCount >= MAX_OBJECTS )
			{
				fprintf( stderr, "sceLoad: Foi ultrapassado o limite de definicoes de objetos na cena. Ignorando." );
				continue;
			}

			scene->objects[scene->objectCount++] = objCreateMesh( material, pos1, pos2, buffer );
		}
		else if( sscanf( buffer, "BTREE %d %d %d\n", &obj1, &obj2, &op ) == 3 )
		{
				if( scene->objectCount >= MAX_OBJECTS )
			{
				fprintf( stderr, "sceLoad: Foi ultrapassado o limite de definicoes de objetos na cena. Ignorando." );
				continue;
			}
			scene->objects[scene->objectCount++] = objCreateBtree(
					scene->objects[obj1],
					scene->objects[obj2],
					op);
			scene->objects[obj1] = NULL;
			scene->objects[obj2] = NULL;
		}
		else
		{			
			printf( "sceLoad: Ignorando comando:\n %s\n", buffer );
		}
	}

	/* Adjust background image to screen size */
	if( scene->camera && scene->bgImage )
	{
		scene->bgImage = imgResize( scene->bgImage, camGetScreenWidth( scene->camera ),
			camGetScreenHeight( scene->camera ) );
	}

	fclose( file );

	return scene;
}

int sceGetMaterialCount( Scene* scene )
{
	return scene->materialCount;
}

Material* sceGetMaterial( Scene* scene, int index )
{
	return scene->materials[index];
}

void sceDestroy( Scene* scene )
{
	int i;

	camDestroy( scene->camera );
	imgDestroy( scene->bgImage );

	for( i = 0; i < scene->objectCount; ++i )
	{
		objDestroy( scene->objects[i] );
	}

	for( i = 0; i < scene->materialCount; ++i )
	{
		matDestroy( scene->materials[i] );
	}
	
	free( scene );
}

