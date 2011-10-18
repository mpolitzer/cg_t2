/**
 *   @file algebra.c Algebra: operações com vetores e matrizes.
 *   @author Maira Noronha, Rodrigo Espinha e Marcelo Gattass
 *
 *   @date
 *         Criado em:             3/5/2003.
 *         Última Modificação:   26/5/2003.
 *
 *   @version 1
 */


#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include "algebra.h"

#ifndef M_PI
#define M_PI       3.14159265358979323846264338
#endif

#define real double

/************************************************************************/
/* Funções Exportadas                                                   */
/************************************************************************/

Vector algVector( real x, real y, real z, real w ) 
{
  Vector v = {x, y, z, w};
  return v;
}

real algGetX(Vector vector) 
{
  return vector.x;
}

real algGetY(Vector vector) 
{
  return vector.y;
}

real algGetZ(Vector vector) 
{
  return vector.z;
}

real algGetW(Vector vector) 
{
  return vector.w;
}

Vector algCartesian( Vector vector ) 
{
  if ((vector.w!=1)&&((vector.w>1.0e-9)||(vector.w<-1.0e-9))) 
  {
   vector.x /= vector.w;
   vector.y /= vector.w;
   vector.z /= vector.w;
   vector.w  = 1;
   }
  return vector;
}

Vector algAdd( Vector v1, Vector v2 ) 
{
  Vector v = {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w};
  return v;
}

Vector algScale( real scalar, Vector vector ) 
{
  Vector v = {scalar*vector.x, scalar*vector.y, scalar*vector.z, vector.w};
  return v;
}

Vector algSub( Vector v1, Vector v2 ) 
{
  Vector v = {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w};
  return v;
}

Vector algMinus( Vector vector ) 
{
  Vector v = {-vector.x, -vector.y, -vector.z, vector.w};
  return v;
}

real algNorm( Vector vector ) 
{
  return (real) sqrt(vector.x*vector.x + vector.y*vector.y + vector.z*vector.z);
}

Vector algUnit( Vector vector ) 
{
  real n = algNorm(vector);
  if ( n > 1e-9 ) {
    return algScale(1/n, vector);
  } else {
    return vector;
  }
}

real algDot( Vector v1, Vector v2 ) 
{
  return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

real algDot4( Vector v1, Vector v2 ) 
{
  return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w*v2.w;
}

Vector algProj( Vector ofVector, Vector ontoVector ) 
{
	Vector vProjectVector;
	real fNewScale = algDot(ofVector, ontoVector)/algDot(ontoVector,ontoVector);
	vProjectVector = algScale(fNewScale , ontoVector);
	return vProjectVector;
}

Vector algCross( Vector v1, Vector v2 ) 
{
  Vector v = {
    v1.y*v2.z - v1.z*v2.y, 
    v1.z*v2.x - v1.x*v2.z, 
    v1.x*v2.y - v1.y*v2.x,
    1
  };
  return v;
}

Vector algReflect( Vector ofVector, Vector aroundVector ) 
{
 Vector vProj = algProj(ofVector,aroundVector);
 Vector vIncrH = algSub(vProj,ofVector);
 Vector vReflect = algAdd(vProj, vIncrH);
 return vReflect;
}

Vector algLinComb( int count, ... ) 
{
  Vector result = { 0, 0, 0, 1 };
	va_list arglist;

	va_start( arglist, count );

	while( count-- )
	{
		real scalar = va_arg( arglist, real );
		Vector vector = va_arg( arglist, Vector );

		result.x += ( scalar * vector.x );
		result.y += ( scalar * vector.y );
		result.z += ( scalar * vector.z );
	}
	va_end( arglist );
	return result;
}

Matrix algMatrix4x4( real a11, real a12, real a13, real a14, 
                     real a21, real a22, real a23, real a24,
                     real a31, real a32, real a33, real a34,
                     real a41, real a42, real a43, real a44) 
{
  Matrix m;
  m.m[0]=a11;  m.m[4]=a12;  m.m[8]=a13;  m.m[12]=a14;
  m.m[1]=a21;  m.m[5]=a22;  m.m[9]=a23;  m.m[13]=a24;
  m.m[2]=a31;  m.m[6]=a32;  m.m[10]=a33; m.m[14]=a34;
  m.m[3]=a41;  m.m[7]=a42;  m.m[11]=a43; m.m[15]=a44;
  return m;
}

real algGetElem(Matrix m, int i, int j) 
{
  return m.m[((j-1)<<2)+(i-1)];
}

real * algGetOGLMatrix(Matrix *m) 
{
   return m->m;
}

Vector algTransf( Matrix matrix, Vector vector ) 
{
  Vector v = {
    matrix.m[0]*vector.x + matrix.m[4]*vector.y + matrix.m[8]*vector.z + matrix.m[12]*vector.w,
    matrix.m[1]*vector.x + matrix.m[5]*vector.y + matrix.m[9]*vector.z + matrix.m[13]*vector.w,
    matrix.m[2]*vector.x + matrix.m[6]*vector.y + matrix.m[10]*vector.z + matrix.m[14]*vector.w,
    matrix.m[3]*vector.x + matrix.m[7]*vector.y + matrix.m[11]*vector.z + matrix.m[15]*vector.w
  };
  return v;
}

Matrix algMatrixIdent(void) 
{
  Matrix m;
  m.m[0] = 1; m.m[4] = 0; m.m[8] = 0;  m.m[12] = 0;
  m.m[1] = 0; m.m[5] = 1; m.m[9] = 0;  m.m[13] = 0;
  m.m[2] = 0; m.m[6] = 0; m.m[10] = 1; m.m[14] = 0;
  m.m[3] = 0; m.m[7] = 0; m.m[11] = 0; m.m[15] = 1;
  return m;
}

Matrix algMatrixTransl(real tx, real ty, real tz) 
{
  Matrix m;
  m.m[0] = 1; m.m[4] = 0; m.m[8] = 0;  m.m[12] = tx;
  m.m[1] = 0; m.m[5] = 1; m.m[9] = 0;  m.m[13] = ty;
  m.m[2] = 0; m.m[6] = 0; m.m[10] = 1; m.m[14] = tz;
  m.m[3] = 0; m.m[7] = 0; m.m[11] = 0; m.m[15] = 1;
  return m;
}

Matrix algMatrixScale(real scalex, real scaley, real scalez) 
{
  Matrix m;
  m.m[0] = scalex; m.m[4] = 0;   m.m[8] = 0;    m.m[12] = 0;
  m.m[1] = 0;   m.m[5] = scaley; m.m[9] = 0;    m.m[13] = 0;
  m.m[2] = 0;   m.m[6] = 0;   m.m[10] = scalez; m.m[14] = 0;
  m.m[3] = 0;   m.m[7] = 0;   m.m[11] = 0;   m.m[15] = 1;
  return m;
}

Matrix algMatrixRotate(real angle, real ex, real ey,  real ez) 
{
  Matrix m;
  real norm = (real) sqrt(ex*ex + ey*ey + ez*ez);

  /* Converte graus para radianos. */
  angle = (angle* (real) M_PI)/180;

  if ( norm >= 1e-9 ) {
    real cos_a, sin_a;

    cos_a = (real) cos(angle);
    sin_a = (real) sin(angle);

    ex /= norm; ey /= norm; ez /= norm;

    m.m[0]  = cos_a + (1 - cos_a)*ex*ex;    //Linha 1
    m.m[4]  = ex*ey*(1 - cos_a) - ez*sin_a;
    m.m[8]  = ez*ex*(1 - cos_a) + ey*sin_a;
    m.m[12] =  0;

    m.m[1]  = ex*ey*(1 - cos_a) + ez*sin_a;
    m.m[5]  = cos_a + (1 - cos_a)*ey*ey;
    m.m[9]  = ey*ez*(1 - cos_a) - ex*sin_a;
    m.m[13] = 0;

    m.m[2]  = ex*ez*(1 - cos_a) - ey*sin_a;
    m.m[6]  = ey*ez*(1 - cos_a) + ex*sin_a;
    m.m[10] = cos_a + (1 - cos_a)*ez*ez;
    m.m[14] = 0;

    m.m[3]  = 0;
    m.m[7]  = 0;
    m.m[11] = 0;
    m.m[15] = 1;
  } else {
    /* Matriz identidade. */
    m.m[0] = 1; m.m[4] = 0; m.m[8] = 0;  m.m[12] = 0;
    m.m[1] = 0; m.m[5] = 1; m.m[9] = 0;  m.m[13] = 0;
    m.m[2] = 0; m.m[6] = 0; m.m[10] = 1; m.m[14] = 0;
    m.m[3] = 0; m.m[7] = 0; m.m[11] = 0; m.m[15] = 1;    
  }
  return m;
}

Matrix algMatrixAdd( Matrix m1, Matrix m2 ) 
{
  Matrix m;
  m.m[0] = m1.m[0] + m2.m[0];  m.m[4] = m1.m[4] + m2.m[4];  m.m[8] = m1.m[8] + m2.m[8];  m.m[12] = m1.m[12] + m2.m[12];
  m.m[1] = m1.m[1] + m2.m[1];  m.m[5] = m1.m[5] + m2.m[5];  m.m[9] = m1.m[9] + m2.m[9];  m.m[13] = m1.m[13] + m2.m[13];
  m.m[2] = m1.m[2] + m2.m[2];  m.m[6] = m1.m[6] + m2.m[6];  m.m[10] = m1.m[10] + m2.m[10];  m.m[14] = m1.m[14] + m2.m[14];
  m.m[3] = m1.m[3] + m2.m[3];  m.m[7] = m1.m[7] + m2.m[7];  m.m[11] = m1.m[11] + m2.m[11];  m.m[15] = m1.m[15] + m2.m[15];
  return m;
}

Matrix algMatrixScalarProduct( real scale, Matrix m1 ) 
{
  Matrix m;
  m.m[0] = scale*m1.m[0];  m.m[4] = scale*m1.m[4];  m.m[8] = scale*m1.m[8];  m.m[12] = scale*m1.m[12];
  m.m[1] = scale*m1.m[1];  m.m[5] = scale*m1.m[5];  m.m[9] = scale*m1.m[9];  m.m[13] = scale*m1.m[13];
  m.m[2] = scale*m1.m[2];  m.m[6] = scale*m1.m[6];  m.m[10] = scale*m1.m[10];  m.m[14] = scale*m1.m[14];
  m.m[3] = scale*m1.m[3];  m.m[7] = scale*m1.m[7];  m.m[11] = scale*m1.m[11];  m.m[15] = scale*m1.m[15];
  return m;
}

Matrix algMult( Matrix m1, Matrix m2 ) 
{
  Matrix m;
  m.m[0] = m1.m[0]*m2.m[0] + m1.m[4]*m2.m[1] + m1.m[8]*m2.m[2]  + m1.m[12]*m2.m[3];
  m.m[1] = m1.m[1]*m2.m[0] + m1.m[5]*m2.m[1] + m1.m[9]*m2.m[2]  + m1.m[13]*m2.m[3];
  m.m[2] = m1.m[2]*m2.m[0] + m1.m[6]*m2.m[1] + m1.m[10]*m2.m[2] + m1.m[14]*m2.m[3];
  m.m[3] = m1.m[3]*m2.m[0] + m1.m[7]*m2.m[1] + m1.m[11]*m2.m[2] + m1.m[15]*m2.m[3];

  m.m[4] = m1.m[0]*m2.m[4] + m1.m[4]*m2.m[5] + m1.m[8]*m2.m[6]  + m1.m[12]*m2.m[7];
  m.m[5] = m1.m[1]*m2.m[4] + m1.m[5]*m2.m[5] + m1.m[9]*m2.m[6]  + m1.m[13]*m2.m[7];
  m.m[6] = m1.m[2]*m2.m[4] + m1.m[6]*m2.m[5] + m1.m[10]*m2.m[6] + m1.m[14]*m2.m[7];
  m.m[7] = m1.m[3]*m2.m[4] + m1.m[7]*m2.m[5] + m1.m[11]*m2.m[6] + m1.m[15]*m2.m[7];

  m.m[8]  = m1.m[0]*m2.m[8] + m1.m[4]*m2.m[9] + m1.m[8]*m2.m[10]  + m1.m[12]*m2.m[11];
  m.m[9]  = m1.m[1]*m2.m[8] + m1.m[5]*m2.m[9] + m1.m[9]*m2.m[10]  + m1.m[13]*m2.m[11];
  m.m[10] = m1.m[2]*m2.m[8] + m1.m[6]*m2.m[9] + m1.m[10]*m2.m[10] + m1.m[14]*m2.m[11];
  m.m[11] = m1.m[3]*m2.m[8] + m1.m[7]*m2.m[9] + m1.m[11]*m2.m[10] + m1.m[15]*m2.m[11];

  m.m[12]  = m1.m[0]*m2.m[12] + m1.m[4]*m2.m[13] + m1.m[8]*m2.m[14]   + m1.m[12]*m2.m[15];
  m.m[13]  = m1.m[1]*m2.m[12] + m1.m[5]*m2.m[13] + m1.m[9]*m2.m[14]   + m1.m[13]*m2.m[15];
  m.m[14]  = m1.m[2]*m2.m[12] + m1.m[6]*m2.m[13] + m1.m[10]*m2.m[14]  + m1.m[14]*m2.m[15];
  m.m[15]  = m1.m[3]*m2.m[12] + m1.m[7]*m2.m[13] + m1.m[11]*m2.m[14]  + m1.m[15]*m2.m[15];
  return m; 
}

real algDet( Matrix m ) 
{
  return
    m.m[0] *
      ( m.m[5] * (m.m[10]*m.m[15] - m.m[14]*m.m[11]) - 
        m.m[9] * (m.m[6]*m.m[15]  - m.m[14]*m.m[7])  + 
        m.m[13] * (m.m[6]*m.m[11]  - m.m[10]*m.m[7])   ) - 
    m.m[4] *
      ( m.m[1] * (m.m[10]*m.m[15] - m.m[14]*m.m[11]) - 
        m.m[9] * (m.m[2]*m.m[15] - m.m[14]*m.m[3])   +
        m.m[13] * (m.m[2]*m.m[11] - m.m[10]*m.m[3])    ) +
    m.m[8] * 
      ( m.m[1] * (m.m[6]*m.m[15] - m.m[14]*m.m[7]) -
        m.m[5] * (m.m[2]*m.m[15] - m.m[14]*m.m[3]) +
        m.m[13] * (m.m[2]*m.m[7] - m.m[6]*m.m[3])      ) -
    m.m[12] *
      ( m.m[1] * (m.m[6]*m.m[11] - m.m[10]*m.m[7]) -
        m.m[5] * (m.m[2]*m.m[11] - m.m[10]*m.m[3]) +
        m.m[9] * (m.m[2]*m.m[7] - m.m[6]*m.m[3])       );
}

Matrix algInv( Matrix m) 
{
  real  d;
  Matrix r;

  d = algDet(m);

  
  r.m[0] = ( m.m[5] * (m.m[10]*m.m[15] - m.m[14]*m.m[11]) + m.m[6] * (-m.m[9]*m.m[15] + m.m[13]*m.m[11]) + 
     m.m[7] * (m.m[9]*m.m[14] - m.m[13]*m.m[10]) ) / d;

  r.m[4] = - ( m.m[4] * (m.m[10]*m.m[15] - m.m[14]*m.m[11]) + m.m[6] * (-m.m[8]*m.m[15] + m.m[12]*m.m[11]) + 
       m.m[7] * (m.m[8]*m.m[14] - m.m[12]*m.m[10]) ) / d;

  r.m[8] = ( m.m[4] * (m.m[9]*m.m[15] - m.m[13]*m.m[11]) + m.m[5] * (-m.m[8]*m.m[15] + m.m[12]*m.m[11]) + 
     m.m[7] * (m.m[8]*m.m[13] - m.m[12]*m.m[9]) ) / d;

  r.m[12] = - ( m.m[4] * (m.m[9]*m.m[14] - m.m[13]*m.m[10]) + m.m[5] * (-m.m[8]*m.m[14] + m.m[12]*m.m[10]) + 
       m.m[6] * (m.m[8]*m.m[13] - m.m[12]*m.m[9]) ) / d;



  r.m[1] = - ( m.m[1] * (m.m[10]*m.m[15] - m.m[14]*m.m[11]) + m.m[2] * (-m.m[9]*m.m[15] + m.m[13]*m.m[11]) + 
       m.m[3] * (m.m[9]*m.m[14] - m.m[13]*m.m[10]) ) / d;

  r.m[5] = ( m.m[0] * (m.m[10]*m.m[15] - m.m[14]*m.m[11]) + m.m[2] * (-m.m[8]*m.m[15] + m.m[12]*m.m[11]) + 
     m.m[3] * (m.m[8]*m.m[14] - m.m[12]*m.m[10]) ) / d;

  r.m[9] = - ( m.m[0] * (m.m[9]*m.m[15] - m.m[13]*m.m[11]) + m.m[1] * (-m.m[8]*m.m[15] + m.m[12]*m.m[11]) + 
       m.m[3] * (m.m[8]*m.m[13] - m.m[12]*m.m[9]) ) / d;

  r.m[13] = ( m.m[0] * (m.m[9]*m.m[14] - m.m[13]*m.m[10]) + m.m[1] * (-m.m[8]*m.m[14] + m.m[12]*m.m[10]) + 
     m.m[2] * (m.m[8]*m.m[13] - m.m[12]*m.m[9]) ) / d;



  r.m[2] = ( m.m[1] * (m.m[6]*m.m[15] - m.m[14]*m.m[7]) + m.m[2] * (-m.m[5]*m.m[15] + m.m[13]*m.m[7]) + 
     m.m[3] * (m.m[5]*m.m[14] - m.m[13]*m.m[6]) ) / d;

  r.m[6] = - ( m.m[0] * (m.m[6]*m.m[15] - m.m[14]*m.m[7]) + m.m[2] * (-m.m[4]*m.m[15] + m.m[12]*m.m[7]) + 
       m.m[3] * (m.m[4]*m.m[14] - m.m[12]*m.m[6]) ) / d;

  r.m[10] = ( m.m[0] * (m.m[5]*m.m[15] - m.m[13]*m.m[7]) + m.m[1] * (-m.m[4]*m.m[15] + m.m[12]*m.m[7]) + 
     m.m[3] * (m.m[4]*m.m[13] - m.m[12]*m.m[5]) ) / d;

  r.m[14] = - ( m.m[0] * (m.m[5]*m.m[14] - m.m[13]*m.m[6]) + m.m[1] * (-m.m[4]*m.m[14] + m.m[12]*m.m[6]) + 
       m.m[2] * (m.m[4]*m.m[13] - m.m[12]*m.m[5]) ) / d;



  r.m[3] =  - ( m.m[1] * (m.m[6]*m.m[11] - m.m[10]*m.m[7]) + m.m[2] * (-m.m[5]*m.m[11] + m.m[9]*m.m[7]) + 
       m.m[3] * (m.m[5]*m.m[10] - m.m[9]*m.m[6]) )  / d;

  r.m[7] = ( m.m[0] * (m.m[6]*m.m[11] - m.m[10]*m.m[7]) + m.m[2] * (-m.m[4]*m.m[11] + m.m[8]*m.m[7]) + 
     m.m[3] * (m.m[4]*m.m[10] - m.m[8]*m.m[6]) ) / d;

  r.m[11] = - ( m.m[0] * (m.m[5]*m.m[11] - m.m[9]*m.m[7]) + m.m[1] * (-m.m[4]*m.m[11] + m.m[8]*m.m[7]) + 
       m.m[3] * (m.m[4]*m.m[9] - m.m[8]*m.m[5]) ) / d;

  r.m[15] = ( m.m[0] * (m.m[5]*m.m[10] - m.m[9]*m.m[6]) + m.m[1] * (-m.m[4]*m.m[10] + m.m[8]*m.m[6]) + 
     m.m[2] * (m.m[4]*m.m[9] - m.m[8]*m.m[5]) ) / d;
  return r;
}

Vector algSolve( Matrix m, Vector b ) 
{
  double s1,s2,s3,s4,s5;
  Vector result = {0, 0, 0, 0}; /* Vetor inválido */
 
  double det = algDet(m);
   
  if ( fabs(det) < 1e-8 ) {
    return result;
  }
     
  s2 =  ( m.m[5] * (m.m[10]*m.m[15] - m.m[14]*m.m[11]) + m.m[6] * (-m.m[9]*m.m[15] + m.m[13]*m.m[11]) + 
          m.m[7] * (m.m[9]*m.m[14] - m.m[13]*m.m[10]) ) * b.x / det;
  s3 =  - ( m.m[4] * (m.m[10]*m.m[15] - m.m[14]*m.m[11]) + m.m[6] * (-m.m[8]*m.m[15] + m.m[12]*m.m[11]) + 
            m.m[7] * (m.m[8]*m.m[14] - m.m[12]*m.m[10]) ) * b.y / det;
  s1 =  s2 + s3;
  s2 =  s1;
  s4 =  ( m.m[4] * (m.m[9]*m.m[15] - m.m[13]*m.m[11]) + m.m[5] * (-m.m[8]*m.m[15] + m.m[12]*m.m[11]) + 
          m.m[7] * (m.m[8]*m.m[13] - m.m[12]*m.m[9]) ) * b.z / det;
  s5 =  - ( m.m[4] * (m.m[9]*m.m[14] - m.m[13]*m.m[10]) + m.m[5] * (-m.m[8]*m.m[14] + m.m[12]*m.m[10]) + 
            m.m[6] * (m.m[8]*m.m[13] - m.m[12]*m.m[9]) ) * b.w / det;
  s3 =  s4 + s5;
  result.x = (real) (s2 + s3);

  s2 =  - ( m.m[1] * (m.m[10]*m.m[15] - m.m[14]*m.m[11]) + m.m[2] * (-m.m[9]*m.m[15] + m.m[13]*m.m[11]) + 
            m.m[3] * (m.m[9]*m.m[14] - m.m[13]*m.m[10]) ) * b.x / det;
  s3 =  ( m.m[0] * (m.m[10]*m.m[15] - m.m[14]*m.m[11]) + m.m[2] * (-m.m[8]*m.m[15] + m.m[12]*m.m[11]) + 
          m.m[3] * (m.m[8]*m.m[14] - m.m[12]*m.m[10]) ) * b.y / det;
  s1 =  s2 + s3;
  s2 =  s1;
  s4 =  - ( m.m[0] * (m.m[9]*m.m[15] - m.m[13]*m.m[11]) + m.m[1] * (-m.m[8]*m.m[15] + m.m[12]*m.m[11]) + 
            m.m[3] * (m.m[8]*m.m[13] - m.m[12]*m.m[9]) ) * b.z / det;
  s5 =  ( m.m[0] * (m.m[9]*m.m[14] - m.m[13]*m.m[10]) + m.m[1] * (-m.m[8]*m.m[14] + m.m[12]*m.m[10]) + 
          m.m[2] * (m.m[8]*m.m[13] - m.m[12]*m.m[9]) ) * b.w / det;
  s3 =  s4 + s5;
  result.y = (real) (s2 + s3);

  s2 =  ( m.m[1] * (m.m[6]*m.m[15] - m.m[14]*m.m[7]) + m.m[2] * (-m.m[5]*m.m[15] + m.m[13]*m.m[7]) + 
          m.m[3] * (m.m[5]*m.m[14] - m.m[13]*m.m[6]) ) * b.x / det;
  s3 =  - ( m.m[0] * (m.m[6]*m.m[15] - m.m[14]*m.m[7]) + m.m[2] * (-m.m[4]*m.m[15] + m.m[12]*m.m[7]) + 
            m.m[3] * (m.m[4]*m.m[14] - m.m[12]*m.m[6]) ) * b.y / det;
  s1 =  s2 + s3;
  s2 =  s1;
  s4 =  ( m.m[0] * (m.m[5]*m.m[15] - m.m[13]*m.m[7]) + m.m[1] * (-m.m[4]*m.m[15] + m.m[12]*m.m[7]) + 
          m.m[3] * (m.m[4]*m.m[13] - m.m[12]*m.m[5]) ) * b.z / det;
  s5 =  - ( m.m[0] * (m.m[5]*m.m[14] - m.m[13]*m.m[6]) + m.m[1] * (-m.m[4]*m.m[14] + m.m[12]*m.m[6]) + 
            m.m[2] * (m.m[4]*m.m[13] - m.m[12]*m.m[5]) ) * b.w / det;
  s3 =  s4 + s5;
  result.z = (real) (s2 + s3);

  s2 =  - ( m.m[1] * (m.m[6]*m.m[11] - m.m[10]*m.m[7]) + m.m[2] * (-m.m[5]*m.m[11] + m.m[9]*m.m[7]) + 
            m.m[3] * (m.m[5]*m.m[10] - m.m[9]*m.m[6]) ) * b.x / det;
  s3 =  ( m.m[0] * (m.m[6]*m.m[11] - m.m[10]*m.m[7]) + m.m[2] * (-m.m[4]*m.m[11] + m.m[8]*m.m[7]) + 
          m.m[3] * (m.m[4]*m.m[10] - m.m[8]*m.m[6]) ) * b.y / det;
  s1 =   s2 + s3;
  s2 =   s1;
  s4 =  - ( m.m[0] * (m.m[5]*m.m[11] - m.m[9]*m.m[7]) + m.m[1] * (-m.m[4]*m.m[11] + m.m[8]*m.m[7]) + 
            m.m[3] * (m.m[4]*m.m[9] - m.m[8]*m.m[5]) ) * b.z / det;
  s5 =  ( m.m[0] * (m.m[5]*m.m[10] - m.m[9]*m.m[6]) + m.m[1] * (-m.m[4]*m.m[10] + m.m[8]*m.m[6]) + 
          m.m[2] * (m.m[4]*m.m[9] - m.m[8]*m.m[5]) ) * b.w / det;
  s3 =   s4 + s5;
  result.w = (real) (s2 + s3);

  return result;
}

Matrix algTransp( Matrix m ) 
{
  Matrix r;
  r.m[0] = m.m[0];  r.m[4] = m.m[1];  r.m[8] = m.m[2];   r.m[12] = m.m[3];
  r.m[1] = m.m[4];  r.m[5] = m.m[5];  r.m[9] = m.m[6];   r.m[13] = m.m[7];
  r.m[2] = m.m[8];  r.m[6] = m.m[9];  r.m[10] = m.m[10]; r.m[14] = m.m[11];
  r.m[3] = m.m[12]; r.m[7] = m.m[13]; r.m[11] = m.m[14]; r.m[15] = m.m[15];
  return r;
}

Vector algSnell(Vector in, Vector normal, double n1, double n2)
{
	double c_out;
	Vector t, r;
	Vector v  = algUnit(algMinus(in));
	Vector vn = algProj(algMinus(in), normal);
	Vector vt = algSub(vn, v);
	double s_in  = algNorm(vt);
	double s_out = n1 * s_in / n2;
	t = algUnit(vt);
	if(s_out > 1.0) return t;
	c_out = sqrt(1 - s_out * s_out);
	r = algLinComb(2, s_out, t, 0 - (c_out), normal);
	return r;
}

Quat algQuat(real w, real x, real y, real z)
{
  Quat q = {w, x, y, z};
  return q;
}

Quat  algQuatAdd(Quat q1, Quat q2) 
{
  Quat q = {q1.w + q2.w, q1.x + q2.x, q1.y + q2.y, q1.z + q2.z};
  return q;
}

Quat  algQuatSub(Quat q1, Quat q2) 
{
  Quat q = {q1.w - q2.w, q1.x - q2.x, q1.y - q2.y, q1.z - q2.z};
  return q;
}

Quat  algQuatScale(real s, Quat q1) 
{
  Quat q = {s*q1.w, s*q1.x, s*q1.y, s*q1.z};
  return q;
}

Quat  algQuatMult(Quat q1, Quat q2) 
{
  Quat q = {
    q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z,
    q1.w*q2.x + q2.w*q1.x + q1.y*q2.z - q1.z*q2.y,
    q1.w*q2.y + q2.w*q1.y + q1.z*q2.x - q1.x*q2.z,
    q1.w*q2.z + q2.w*q1.z + q1.x*q2.y - q1.y*q2.x
  };
  return q;
}

real algQuatDot(Quat q1, Quat q2) 
{
  return q1.w*q2.w + q1.x*q2.x + q1.y*q2.y + q1.z*q2.z;
}

real algQuatNorm(Quat q1) 
{
  return (real) (q1.w*q1.w + q1.x*q1.x + q1.y*q1.y + q1.z*q1.z);
}

real algQuatEuclidianNorm(Quat q1) 
{
  return (real) sqrt(q1.w*q1.w + q1.x*q1.x + q1.y*q1.y + q1.z*q1.z);
}

Quat  algQuatUnit(Quat q1) 
{
  Quat q=q1;
  real n = (real) sqrt(q1.w*q1.w + q1.x*q1.x + q1.y*q1.y + q1.z*q1.z);
  if (n>1e-9) {
      q.w = q1.w/n;
      q.x = q1.x/n;
      q.y = q1.y/n;
      q.z = q1.z/n;
  }
  return q;
}

Quat  algQuatConj(Quat q1) 
{
  Quat q = {q1.w, -q1.x, -q1.y, -q1.z};
  return q;
}

Quat  algQuatInv(Quat q1) 
{
  real n = q1.w*q1.w + q1.x*q1.x + q1.y*q1.y + q1.z*q1.z;
  Quat  q = {q1.w/n, -q1.x/n, -q1.y/n, -q1.z/n}; 
  return q;
}

Quat  algQuatSLERP(Quat q1, Quat q2, real u) 
{
  real cos_theta, theta, sin_theta;

  q1 = algQuatUnit(q1);
  q2 = algQuatUnit(q2);
  
  cos_theta  = q1.w*q2.w + q1.x*q2.x + q1.y*q2.y + q1.z*q2.z; 

  if ( cos_theta < 0 )
  {
    cos_theta = -cos_theta;
    q2 = algQuatScale(-1, q2);
  }

  theta      = (real) acos(cos_theta);
  sin_theta  = (real) sin(theta);

  if ( (1 - cos_theta) > 1e-6 ) {
    return algQuatAdd(
              algQuatScale((real) sin((1 - u) * theta) / sin_theta,  q1),
              algQuatScale((real) sin(u * theta) / sin_theta,  q2));
  } else { /* theta é quase 0 */
    /* Evita divisão por valor muito pequeno, aproveitando-se de 
       que sin(theta) é aprox. igual a theta, com theta muito pequeno.. */
    return algQuatAdd(algQuatScale(1 - u, q1), algQuatScale(u, q2));    
  }
}

Quat algQuatFromRot(real angle, Vector axis) 
{
  Quat  q;
  real t, s, n;

  t   = (angle * (real) M_PI)/360;
  s   = (real) sin(t);
  n   = (real) sqrt(axis.x*axis.x + axis.y*axis.y + axis.z*axis.z);

  q.w = (real) cos(t);
  q.x = s*(axis.x/n);
  q.y = s*(axis.y/n);
  q.z = s*(axis.z/n);
  return q;
}

real algQuatRotAngle(Quat q) 
{
  return (real) acos(q.w)*360 / (real) M_PI;
}

Vector algQuatRotAxis(Quat q) 
{
  Vector v = {q.x, q.y, q.z};
  return v;
}

Quat algQuatFromMatrix(Matrix m) 
{
  Quat  q;
  real tr = m.m[0] + m.m[5] + m.m[10]; /* Traço da matriz */
  real s;

  if ( tr > 0 ) 
  {    
    s = (real) sqrt(tr + 1);

    q.w = 0.5f * s; 
    s = 0.5f / s;
    q.x = (m.m[6] - m.m[9]) * s; 
    q.y = (m.m[8] - m.m[2]) * s;
    q.z = (m.m[1] - m.m[4]) * s; 
  } else 
  {
    /* Descobre qual elemento da diagonal tem maior valor */
    real max = m.m[0];
    max = (m.m[5] > max ? m.m[5] : max);
    max = (m.m[10] > max ? m.m[10] : max);

    if ( m.m[0] == max ) 
    {
      /* Coluna 0 */
      s  = (real) sqrt(m.m[0] - m.m[5] - m.m[10] + 1);

      q.x = 0.5f * s;
      if ( s != 0 ) s = 0.5f / s; 

      q.w = (m.m[6] - m.m[9] ) * s;
      q.y = (m.m[1] + m.m[4] ) * s;
      q.z = (m.m[2] + m.m[8] ) * s;
    } else if ( m.m[5] == max ) 
    {
      /* Coluna 1 */
      s  = (real) sqrt(m.m[5] - m.m[0] - m.m[10] + 1);

      q.y = 0.5f * s;
      if ( s != 0 ) s = 0.5f / s;
      
      q.w = (m.m[8] - m.m[2]) * s;
      q.x = (m.m[1] + m.m[4]) * s;
      q.z = (m.m[6] + m.m[9]) * s;
    } else 
    {
      /* Coluna 2: m[10] == max */
      s = (real) sqrt(m.m[10] - m.m[0] - m.m[5] + 1);

      q.z = 0.5f * s;
      if ( s != 0 ) s = 0.5f / s;

      q.w = (m.m[1] - m.m[4]) * s;
      q.x = (m.m[2] + m.m[8]) * s;
      q.y = (m.m[6] + m.m[9]) * s;
    }
  }
  return q;
}

Matrix algQuatToMatrix(Quat q) 
{
  Matrix m;
  m.m[0] = 1 - 2*(q.y*q.y + q.z*q.z);
  m.m[1] = 2*(q.x*q.y + q.z*q.w);
  m.m[2] = 2*(q.x*q.z - q.y*q.w);
  m.m[3] = 0;

  m.m[4] = 2*(q.x*q.y - q.z*q.w);
  m.m[5] = 1 - 2*(q.x*q.x + q.z*q.z);
  m.m[6] = 2*(q.y*q.z + q.x*q.w);
  m.m[7] = 0;

  m.m[8] = 2*(q.x*q.z + q.y*q.w);
  m.m[9] = 2*(q.y*q.z - q.x*q.w);
  m.m[10] = 1 - 2*(q.x*q.x + q.y*q.y);
  m.m[11] = 0;

  m.m[12] = 0;
  m.m[13] = 0;
  m.m[14] = 0;
  m.m[15] = 1;
  return m;
}

void algVectorPrint(char *text, Vector v)
{
   printf("%s, [%g,%g,%g,%g]\n",text,v.x,v.y,v.z,v.w);
}

void algMatrixPrint(char *text, Matrix m)
{
   printf("%s\n",text);
   printf(" |%g %g %g %g|\n",m.m[0],m.m[4],m.m[ 8],m.m[12]);
   printf(" |%g %g %g %g|\n",m.m[1],m.m[5],m.m[ 9],m.m[13]);
   printf(" |%g %g %g %g|\n",m.m[2],m.m[6],m.m[10],m.m[14]);
   printf(" |%g %g %g %g|\n",m.m[3],m.m[7],m.m[11],m.m[15]);
}

void algQuatPrint(char *text, Quat v)
{
   printf("%s, [%g,%g,%g,%g]\n",text,v.x,v.y,v.z,v.w);
}

#undef real
