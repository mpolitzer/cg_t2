/**
 *	@file object.c Object*: definição e operações com primitivas.
 *		As primitivas suportadas atualmente são: esferas, triângulos e paralelepípedos.
 *
 *	@date
 *			Criado em:			01 de Dezembro de 2002
 *			Última Modificação:	05 de outubro de 2009
 *
 */

#include "object.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "algebra.h"

/**
 *   Tipo objeto
 */
struct _Object
{
    /**
     *  Tipo do objeto.
     */
	int type;
	/**
     *  Material* do objeto.
     */
	int material;
	/**
     *  Dados do objeto.
     */
	void *data;
};

struct _Btree
{
	int op;
	struct _Object *left, *right;
};

/**
 *   Objeto esfera.
 */
struct _Sphere
{
	
    /**
	 *  Posição do centro da esfera na cena.
	 */
	Vector center;
	  
	/**
	 *  Raio da esfera.
	 */
	double radius;
};


/**
 *   Objeto caixa.
 */
struct _Box
{	
	/**
	 *  Vértice de baixo e à esquerda do paralelepípedo.
	 */
	Vector bottomLeft;	
	/**
	 *  Vértice de cima e à direita do paralelepípedo.
	 */
	Vector topRight;
};

/**
 *   Objeto triângulo.
 */
struct _Triangle
{	
	/**
	 *  Primeiro vértice do triângulo.
	 */
	Vector v0;
	/**
	 *  Segundo vértice do triângulo.
	 */
	Vector v1;
	/**
	 *  Terceiro vértice do triângulo.
	 */
	Vector v2;
	
	Vector tex0;  /* coordenada de textura do verive 0 */
	Vector tex1;  /* coordenada de textura do verive 1 */
	Vector tex2;  /* coordenada de textura do verive 2 */
};
/**
*   Objeto malha.
*/
struct _Mesh
{	
	/**
	*  Vértice de baixo e à esquerda do paralelepípedo.
	*/
	Vector bottomLeft;	
	/**
	*  Vértice de cima e à direita do paralelepípedo.
	*/
	Vector topRight;
	/**
	* Numero de vertices da malha.
	*/ 
	int nvertices;
	/**
	* Numero de triangulos da malha.
	*/ 
	int ntriangles;
	/**
	* Vetor dos vertices.
	*/ 
	float* coord;
	/**
	* Vetor da incidencia dos triangulos.
	*/
	int* triangle;
};
/************************************************************************/
/* Constantes Privadas                                                  */
/************************************************************************/
#define MIN( a, b ) ( ( a < b ) ? a : b )
#define MAX( a, b ) ( ( a > b ) ? a : b )

#ifndef EPSILON
#define EPSILON	1.0e-3
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

enum
{
	TYPE_UNKNOWN,
	TYPE_SPHERE,
	TYPE_TRIANGLE,
	TYPE_BOX,
	TYPE_MESH,
	TYPE_BTREE,
};

/************************************************************************/
/* Definição das Funções Exportadas                                     */
/************************************************************************/

Object* objCreateBtree(Object *left, Object *right, int op )
{
	Object* object;
	Btree* btree;

	object = (Object *)malloc( sizeof(Object) );
	btree = (Btree *)malloc( sizeof(Btree) );

	*btree = (Btree){ .left = left, .right = right, .op = op };

	object->type = TYPE_BTREE;
	object->data = btree;

	return object;
}

Object* objCreateSphere( int material, const Vector center, double radius )
{
	Object* object;
	Sphere *sphere;

	object = (Object *)malloc( sizeof(Object) );
	sphere = (Sphere *)malloc( sizeof(Sphere) );

	sphere->center = center;
	sphere->radius = radius;

	object->type = TYPE_SPHERE;
	object->material = material;
	object->data = sphere;

	return object;
}


Object* objCreateTriangle( int material, const Vector v0, const Vector v1, const Vector v2, 
						                const Vector tex0, const Vector tex1, const Vector tex2 )
{
	Object* object;
	Triangle *triangle;

	object = (Object *)malloc( sizeof(Object) );
	triangle = (Triangle *)malloc( sizeof(Triangle) );

	triangle->v0 = v0;
	triangle->v1 = v1;
	triangle->v2 = v2;

	triangle->tex0 = tex0;
	triangle->tex1 = tex1;
	triangle->tex2 = tex2;

	object->type = TYPE_TRIANGLE;
	object->material = material;
	object->data = triangle;

	return object;
}


Object* objCreateBox( int material, const Vector bottomLeft, const Vector topRight )
{
	Object* object;
	Box *box;

	object = (Object *)malloc( sizeof(Object) );
	box = (Box *)malloc( sizeof(Box) );

	box->bottomLeft = bottomLeft;
	box->topRight = topRight;

	object->type = TYPE_BOX;
	object->material = material;
	object->data = box;

	return object;
}

Object* objCreateMesh( int material, const Vector bottomLeft, const Vector topRight, const char* filename )
{
	Object* object;
	Mesh* mesh;
	FILE* fp=NULL;

	object = (Object *)malloc( sizeof(Object) );
	mesh = (Mesh*)malloc( sizeof(Mesh) );

	mesh->bottomLeft = bottomLeft;
	mesh->topRight = topRight;

	object->type = TYPE_MESH;
	object->material = material;
	object->data = mesh;

	fp = fopen(filename,"rt");
	if (fp!=NULL) {
		int i;
		int dummy;
		float xm,xM,ym,yM,zm,zM;

		dummy = fscanf(fp,"%d",&mesh->nvertices);
		mesh->coord = (float*)malloc(3*mesh->nvertices*sizeof(float));
		for (i=0;i<mesh->nvertices;i++){
			dummy = fscanf(fp," %f %f %f",&mesh->coord[3*i],&mesh->coord[3*i+1],&mesh->coord[3*i+2]);
		}
		dummy = fscanf(fp,"%d",&mesh->ntriangles);
		mesh->triangle=(int*)malloc(3*mesh->ntriangles*sizeof(int));
		for (i=0;i<mesh->ntriangles;i++){
			dummy = fscanf(fp," %d %d %d",&mesh->triangle[3*i],&mesh->triangle[3*i+1],&mesh->triangle[3*i+2]);
		}
		xm=xM=mesh->coord[0]; ym=yM=mesh->coord[1]; zm=zM=mesh->coord[2];
		for (i=1;i<mesh->nvertices;i++){
			xm=(xm<mesh->coord[3*i+0])?xm:mesh->coord[3*i+0];
			ym=(ym<mesh->coord[3*i+1])?ym:mesh->coord[3*i+1];
			zm=(zm<mesh->coord[3*i+2])?zm:mesh->coord[3*i+2];
			xM=(xM>mesh->coord[3*i+0])?xM:mesh->coord[3*i+0];
			yM=(yM>mesh->coord[3*i+1])?yM:mesh->coord[3*i+1];
			zM=(zM>mesh->coord[3*i+2])?zM:mesh->coord[3*i+2];
		}
		for (i=0;i<mesh->nvertices;i++){
			mesh->coord[3*i+0] = (float) (bottomLeft.x+(topRight.x-bottomLeft.x)*(mesh->coord[3*i+0]-xm)/(xM-xm));
			mesh->coord[3*i+1] = (float) (bottomLeft.y+(topRight.y-bottomLeft.y)*(mesh->coord[3*i+1]-ym)/(yM-ym));
			mesh->coord[3*i+2] = (float) (bottomLeft.z+(topRight.z-bottomLeft.z)*(mesh->coord[3*i+2]-zm)/(zM-zm));
		}
	}

	return object;
}

double objMeshIntercept(Mesh* mesh,Vector origin,Vector direction,double distance)
{
	int i;
	for (i=0;i<mesh->ntriangles;i++)
	{
		int p0 = mesh->triangle[3*i+0];
		int p1 = mesh->triangle[3*i+1];
		int p2 = mesh->triangle[3*i+2];

		Vector v0 = {mesh->coord[3*p0+0],mesh->coord[3*p0+1],mesh->coord[3*p0+2],1};
		Vector v1 = {mesh->coord[3*p1+0],mesh->coord[3*p1+1],mesh->coord[3*p1+2],1};
		Vector v2 = {mesh->coord[3*p2+0],mesh->coord[3*p2+1],mesh->coord[3*p2+2],1};

		double dividend, divisor;
		double distance = -1.0;

		Vector v0ToV1 = algSub( v1, v0 );
		Vector v1ToV2 = algSub( v2, v1 );
		Vector normal = algCross( v0ToV1, v1ToV2 );
		Vector eyeToV0 = algSub( v0, origin );

		dividend = algDot( eyeToV0, normal );
		divisor = algDot( direction, normal );

		if( divisor <= -EPSILON )
		{
			distance = ( dividend / divisor );
		}

		if( distance >= 0.0001 )
		{
			double a0, a1, a2;

			Vector v2ToV0 = algSub( v0, v2 );
			Vector p = algAdd( origin, algScale( distance, direction ) );
			Vector n0 = algCross( v0ToV1, algSub( p, v0 ) );
			Vector n1 = algCross( v1ToV2, algSub( p, v1 ) );
			Vector n2 = algCross( v2ToV0, algSub( p, v2 ) );

			normal = algUnit(normal);
			a0 = ( 0.5 * algDot( normal, n0 ) );
			a1 = ( 0.5 * algDot( normal, n1 ) );
			a2 = ( 0.5 * algDot( normal, n2 ) );

			if ( (a0>0) && (a1>0) && (a2>0) )  
				return distance;
		}

	}

	return -1.0;
}

double objInterceptExitW( Object* object, Vector eye, Vector ray )
{
	switch (object->type){
	case TYPE_SPHERE:
		{
			Sphere *s = (Sphere *)object->data;

			double a, b, c, delta;
			double distance = -1.0;

			Vector fromSphereToEye;

			fromSphereToEye = algSub( eye, s->center );

			a = algDot( ray, ray );
			b = ( 2.0 * algDot( ray, fromSphereToEye ) );
			c = ( algDot( fromSphereToEye, fromSphereToEye ) - ( s->radius * s->radius ) );

			delta = ( ( b * b ) - ( 4 * a * c ) );

			if( fabs( delta ) <= EPSILON )
			{
				distance = ( -b / (2 * a ) );
			}
			else if( delta > EPSILON )
			{
				double root = sqrt( delta );
				distance = MAX( ( ( -b + root ) / ( 2 * a ) ), ( ( -b - root ) / ( 2.0 * a ) )  );
			}

			return distance;
		}
	}
	return 0;
}

double objIntercept( Object* object, Vector eye, Vector ray )
{
	if (!object) return -1;
	switch( object->type )
	{
		Btree *bt;
		Vector v;
		double d1, d2, min;
		double i0, i1, o0, o1;
	case TYPE_BTREE:
		bt = (Btree *)object->data;
		if (bt->op == OP_UNION){
			d1 = objIntercept( bt->left, eye, ray );
			d2 = objIntercept( bt->right, eye, ray );
			min = MIN( d1, d2 );
			if (min < 0) return MAX( d1, d2 );
			return min;
		} else if (bt->op == OP_INTERSECT){
			i0 = objIntercept( bt->left, eye, ray );
			i1 = objIntercept( bt->right, eye, ray );
			o0 = objInterceptExitW( bt->left, eye, ray );
			o1 = objInterceptExitW( bt->right, eye, ray );

			if ((i0 < i1) && (o0 > i1)) return i1;
			if ((i1 < i0) && (o1 > i0)) return i0;
			return -1;
		} else if (bt->op == OP_DIFF){
			i0 = objIntercept( bt->left, eye, ray );
			i1 = objIntercept( bt->right, eye, ray );
			o0 = objInterceptExitW( bt->left, eye, ray );
			o1 = objInterceptExitW( bt->right, eye, ray );
#if 1
			if (i0 > i1) return i0;
			if (o0 > i1) return o0;
#else
			if (i0 < i1) return i0;
			if (o1 < o0) return o1;
#endif
			return -1;
		}
		 else if (bt->op == OP_INTERSECTSP){
			i0 = objIntercept( bt->left, eye, ray );
			i1 = objIntercept( bt->right, eye, ray );
			o0 = algNorm(objInterceptExit( bt->left, eye, ray ));
			o1 = algNorm(objInterceptExit( bt->right, eye, ray ));

			if ((i0 < i1) && (o0 > i1)) return i1;
			if ((i1 < i0) && (o1 > i0)) return i0;
			return -1;
		}
	case TYPE_SPHERE:
		{
			Sphere *s = (Sphere *)object->data;

			double a, b, c, delta;
			double distance = -1.0;

			Vector fromSphereToEye;

			fromSphereToEye = algSub( eye, s->center );

			a = algDot( ray, ray );
			b = ( 2.0 * algDot( ray, fromSphereToEye ) );
			c = ( algDot( fromSphereToEye, fromSphereToEye ) - ( s->radius * s->radius ) );

			delta = ( ( b * b ) - ( 4 * a * c ) );

			if( fabs( delta ) <= EPSILON )
			{
				distance = ( -b / (2 * a ) );
			}
			else if( delta > EPSILON )
			{
				double root = sqrt( delta );
				distance = MIN( ( ( -b + root ) / ( 2 * a ) ), ( ( -b - root ) / ( 2.0 * a ) )  );
			}

			return distance;
		}

	case TYPE_TRIANGLE:
		{
			Triangle *t = (Triangle *)object->data;


			double dividend, divisor;
			double distance = -1.0;

			Vector v0ToV1 = algSub( t->v1, t->v0 );
			Vector v1ToV2 = algSub( t->v2, t->v1 );
			Vector normal = algCross( v0ToV1, v1ToV2 );
			Vector eyeToV0 = algSub( t->v0, eye );

			dividend = algDot( eyeToV0, normal );
			divisor = algDot( ray, normal );

			if( divisor <= -EPSILON )
			{
				distance = ( dividend / divisor );
			}

			if( distance >= 0.0001 ) /* teste para ver se e' inteior */
			{
				double a0, a1, a2;

				Vector v2ToV0 = algSub( t->v0, t->v2 );
				Vector p = algAdd( eye, algScale( distance, ray ) );
				Vector n0 = algCross( v0ToV1, algSub( p, t->v0 ) );
				Vector n1 = algCross( v1ToV2, algSub( p, t->v1 ) );
				Vector n2 = algCross( v2ToV0, algSub( p, t->v2 ) );

				normal = algUnit(normal);
				a0 = ( 0.5 * algDot( normal, n0 ) );
				a1 = ( 0.5 * algDot( normal, n1 ) );
				a2 = ( 0.5 * algDot( normal, n2 ) );

				if ( (a0>0) && (a1>0) && (a2>0) )  
					return distance;
			}

			return -1.0;
		}

	case TYPE_BOX:
		{
			Box *box = (Box *)object->data;

			double xmin = box->bottomLeft.x;
			double ymin = box->bottomLeft.y;
			double zmin = box->bottomLeft.z;
			double xmax = box->topRight.x;
			double ymax = box->topRight.y;
			double zmax = box->topRight.z;

			double x, y, z;
			double distance = -1.0;

			if( ray.x > EPSILON || -ray.x > EPSILON )
			{
				if( ray.x > 0 )
				{
					x = xmin;
					distance = ( ( xmin - eye.x ) / ray.x );
				}
				else
				{
					x = xmax;
					distance = ( ( xmax - eye.x ) / ray.x );
				}

				if( distance > EPSILON )
				{
					y = ( eye.y + ( distance * ray.y ) ); 
					z = ( eye.z + ( distance * ray.z ) ); 
					if( ( y >= ymin ) && ( y <= ymax ) && ( z >= zmin ) && ( z <= zmax ) )
						return distance;
				}
			}

			if( ray.y > EPSILON || -ray.y > EPSILON )
			{
				if( ray.y > 0 )
				{
					y = ymin;
					distance = ( ( ymin - eye.y ) / ray.y );
				}
				else
				{
					y = ymax;
					distance = ( ( ymax - eye.y ) / ray.y );
				}

				if( distance > EPSILON )
				{
					x = ( eye.x + ( distance * ray.x ) ); 
					z = ( eye.z + ( distance * ray.z ) ); 
					if( ( x >= xmin ) && ( x <= xmax ) && ( z >= zmin ) && ( z <= zmax ) )
						return distance;
				}

			}

			if( ray.z > EPSILON || -ray.z > EPSILON )
			{
				if( ray.z > 0 )
				{
					z = zmin;
					distance = ( (zmin - eye.z ) / ray.z );
				}
				else
				{
					z = zmax;
					distance = ( ( zmax - eye.z ) / ray.z );
				}

				if( distance > EPSILON )
				{
					x = ( eye.x + ( distance * ray.x ) ); 
					y = ( eye.y + ( distance * ray.y ) ); 
					if( ( x >= xmin ) && ( x <= xmax ) && ( y >= ymin ) && ( y <= ymax ) )	
						return distance;
				}
			}

			return -1.0;
		}
	case TYPE_MESH:
		{
			Mesh* mesh = (Mesh*)object->data;

			double xmin = mesh->bottomLeft.x;
			double ymin = mesh->bottomLeft.y;
			double zmin = mesh->bottomLeft.z;
			double xmax = mesh->topRight.x;
			double ymax = mesh->topRight.y;
			double zmax = mesh->topRight.z;

			double x, y, z;
			double distance = -1.0;

			if( ray.x > EPSILON || -ray.x > EPSILON )
			{
				if( ray.x > 0 )
				{
					x = xmin;
					distance = ( ( xmin - eye.x ) / ray.x );
				}
				else
				{
					x = xmax;
					distance = ( ( xmax - eye.x ) / ray.x );
				}

				if( distance > EPSILON )
				{
					y = ( eye.y + ( distance * ray.y ) ); 
					z = ( eye.z + ( distance * ray.z ) ); 
					if( ( y >= ymin ) && ( y <= ymax ) && ( z >= zmin ) && ( z <= zmax ) )
						return objMeshIntercept(mesh,eye,ray,distance);
				}
			}

			if( ray.y > EPSILON || -ray.y > EPSILON )
			{
				if( ray.y > 0 )
				{
					y = ymin;
					distance = ( ( ymin - eye.y ) / ray.y );
				}
				else
				{
					y = ymax;
					distance = ( ( ymax - eye.y ) / ray.y );
				}

				if( distance > EPSILON )
				{
					x = ( eye.x + ( distance * ray.x ) ); 
					z = ( eye.z + ( distance * ray.z ) ); 
					if( ( x >= xmin ) && ( x <= xmax ) && ( z >= zmin ) && ( z <= zmax ) )
						return objMeshIntercept(mesh,eye,ray,distance);
				}

			}

			if( ray.z > EPSILON || -ray.z > EPSILON )
			{
				if( ray.z > 0 )
				{
					z = zmin;
					distance = ( (zmin - eye.z ) / ray.z );
				}
				else
				{
					z = zmax;
					distance = ( ( zmax - eye.z ) / ray.z );
				}

				if( distance > EPSILON )
				{
					x = ( eye.x + ( distance * ray.x ) ); 
					y = ( eye.y + ( distance * ray.y ) ); 
					if( ( x >= xmin ) && ( x <= xmax ) && ( y >= ymin ) && ( y <= ymax ) )	
						return objMeshIntercept(mesh,eye,ray,distance);
				}
			}

			return -1.0;
		}
	
	default:
		/* Tipo de Objeto Inválido: nunca deve acontecer */
		return -1.0;
	}
}

Vector objInterceptExit( Object* object, Vector point, Vector d )
{
	switch( object->type )
	{
		Btree *bt;
		Vector v1, v2;
	case TYPE_BTREE:
		bt = (Btree *)object->data;
		v1 = objInterceptExit( bt->left, point, d );
		v2 = objInterceptExit( bt->right, point, d );
		return algNorm(v1) > algNorm(v2) ? v1 : v2;
	case TYPE_SPHERE:
		{
			Sphere *s = (Sphere *)object->data;

			double a, b, c, delta, distance = 0;
			//double distance = -1.0;

			Vector fromSphereToEye;

			fromSphereToEye = algSub( point, s->center );

			a = algDot( d, d );
			b = ( 2.0 * algDot( d, fromSphereToEye ) );
			c = ( algDot( fromSphereToEye, fromSphereToEye ) - ( s->radius * s->radius ) );

			delta = ( ( b * b ) - ( 4 * a * c ) );

			if( fabs( delta ) <= EPSILON )
			{
				distance = ( -b / (2 * a ) );
			}
			else if( delta > EPSILON )
			{
				double root = sqrt( delta );
				distance = MAX( ( ( -b + root ) / ( 2 * a ) ), ( ( -b - root ) / ( 2.0 * a ) )  );
			}

			return algAdd(point, algScale(distance, d));
		}

	case TYPE_TRIANGLE:
	case TYPE_BOX:
		break;
	}
	return point;
}

int do_btree_check( Object *o, Vector point )
{
	if (o->type == TYPE_BTREE ){
		Btree *bt = (Btree *)o->data;
		do_btree_check( bt->left, point );
		do_btree_check( bt->right, point );
	} else if (o->type == TYPE_SPHERE){
		Sphere *s = (Sphere *)o->data;
		Vector v = algSub( point, s->center );

		if ( fabs( s->radius - algNorm( v )) < EPSILON )
			return 1;
		return 0;
	}
	return -1;
}

Vector objNormalAt( Object* object, Vector point )
{
	if( object->type == TYPE_BTREE )
	{
		Btree *bt = (Btree *)object->data;

		if (do_btree_check( bt->left, point ) > 0)
			return objNormalAt( bt->left, point );
		if (do_btree_check( bt->right, point ) > 0)
			return objNormalAt( bt->right, point );
		return algVector( 0, 0, 0, 1 );
	}
	else if( object->type == TYPE_SPHERE )
	{
		Sphere *sphere = (Sphere *)object->data;

		return algScale( ( 1.0 / sphere->radius ),
					algSub( point, sphere->center ) );
	}
	else if ( object->type == TYPE_TRIANGLE )
	{
		Triangle *triangle = (Triangle *)object->data;

		return algCross( algSub( triangle->v1, triangle->v0 ),
					algSub( triangle->v2, triangle->v0 ) );
	}
	else if ( object->type == TYPE_BOX )
	{
		Box *box = (Box *)object->data;
		/* Seleciona a face mais próxima de point */
		if( fabs( point.x - box->bottomLeft.x ) < EPSILON )
		{
			return algVector( -1, 0, 0, 1  );
		}
		else if( fabs( point.x - box->topRight.x ) < EPSILON )
		{
			return algVector( 1, 0, 0, 1 );
		}
		else if( fabs( point.y - box->bottomLeft.y ) < EPSILON )
		{
			return algVector( 0, -1, 0, 1 );
		}
		else if( fabs( point.y - box->topRight.y ) < EPSILON )
		{
			return algVector( 0, 1, 0, 1 );
		}
		else if( fabs( point.z - box->bottomLeft.z ) < EPSILON )
		{
			return algVector( 0, 0, -1, 1 );
		}
		else if( fabs( point.z - box->topRight.z ) < EPSILON )
		{
			return algVector( 0, 0, 1, 1 );
		}
		else
		{
			return algVector( 0, 0, 0, 1 );
		}
	} 
	else
	{
		/* Tipo de Objeto Inválido: nunca deve acontecer */
		return algVector( 0, 0, 0, 1 );
	}
}

Vector objTextureCoordinateAt( Object* object, Vector point )
{
	if( object->type == TYPE_SPHERE )
	{
   /*...*/
		return algVector( 0, 0, 0, 1 );
	} 
	else if( object->type == TYPE_TRIANGLE )
	{
   /*...*/
		return algVector( 0, 0, 0, 1 );
	} 
	else if( object->type == TYPE_BOX )
	{
   /*...*/
		return algVector( 0, 0, 0, 1 );
	} 

	/* Tipo de Objeto Inválido: nunca deve acontecer */
	return algVector( 0, 0, 0, 1 );	
}

int objGetMaterial( Object* object )
{
	if (object->type == TYPE_BTREE) {
		int m1, m2;

		Btree *bt = (Btree *) object->data;
		m1 = objGetMaterial(bt->left);
		m2 = objGetMaterial(bt->right);
		return MAX(m1, m2);
	}
	return object->material;
}

void objDestroy( Object* o )
{
	if ( o ) return;
	if ( o->type == TYPE_BTREE ){
		Btree *bt = o->data;
		objDestroy( bt->left );
		objDestroy( bt->right );
	}
	free( o->data );
	free( o );
}
