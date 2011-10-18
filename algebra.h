/**
 *   @file algebra.h Algebra: operações com vetores e matrizes.
 *
 *   @date
 *         Criado em:         Mar2003
 *         Última Modificação:   Mai2003
 *
 */

#ifndef   _ALGEBRA_H_
#define   _ALGEBRA_H_

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/* Tipos Exportados                                                     */
/************************************************************************/

#define real double

/**
 *   Vetor no espaço 3D homogêneo.
 *   Para representar vetores 2D, use z = 0 e w = 1.
 *   Para representar vetores 3D, use w = 1.
 */
typedef struct
{
/**
 * Valor na direcao x.
 */   
   real x;
/**
 * Valor na direcao y.
 */
   real y;
/**
 * Valor na direcao z (z = 0 para vetores 2D).
 */
   real z;
/**
 * Valor de w (w = 1 para vetores 2D e 3D).
 */
   real w;
}
Vector;

/**
 * Quaternio (w, (x,y,z)).
 */
typedef struct 
{
/**
 * Valor de w.
 */ 
   real w;
/**
 * Valor na direcao x.
 */ 
   real x;
/**
 * Valor na direcao y.
 */ 
   real y;
/**
 * Valor na direcao z.
 */ 
   real z;
}
Quat;

/**
 *   Matriz homogenea no mesmo formato do OpenGL.
 */
typedef struct 
{
/**
 *   Matriz homogenea no mesmo formato do OpenGL (vetor de 16 elementos).
 *
 *   Ordem dos elementos:
 *   M = ( m0,   m4,   m8,   m12 )
 *       ( m1,   m5,   m9,   m13 )
 *       ( m2,   m6,   m10,  m14 )
 *       ( m3,   m7,   m11,  m15 )
 */
   real m[16];
} Matrix;


/************************************************************************/
/* Funções Exportadas                                                   */
/************************************************************************/

/**
 *   Cria um vetor no espaço 3D homogêneo.
 */
Vector algVector( real x, real y, real z, real w );

/**
 * Obtem a coordenada x de um vetor.
 */
real algGetX(Vector vector);

/**
 * Obtem a coordenada y de um vetor.
 */
real algGetY(Vector vector);

/**
 * Obtem a coordenada z de um vetor.
 */
real algGetZ(Vector vector);

/**
 * Obtem a coordenada w de um vetor.
 */
real algGetW(Vector vector);

/**
 *   Projeta o vetor do espaco homogeneo (w!=1)
 * no espaco cartesiano (w=1).
 */
Vector algCartesian( Vector vector );

/**
 *   Soma dois vetores do R3 (ignorando a componente w).
 */
Vector algAdd( Vector v1, Vector v2 );

/**
 *   Multiplica um vetor do R3 por um escalar(ignorando a componente w).
 */
Vector algScale( real scalar, Vector vector );

/**
 *   Subtrai dois vetores (ignorando a componente w).
 */
Vector algSub( Vector v1, Vector v2 );

/**
 *   Negativo de um vetor do R3 (ignorando a componente w).
 */
Vector algMinus( Vector vector );

/**
 *   Norma de um vetor do R3 (ignorando a componente w).
 */
real algNorm( Vector vector );

/**
 *   Vetor unitario direção de um vetor do R3 (ignorando a componente w).
 */
Vector algUnit( Vector vector );

/**
 *   Produto interno entre dois vetores do R3 (ignorando a componente w).
 */
real algDot( Vector v1, Vector v2 );

/**
 *   Produto interno entre dois vetores homogeneos, ou seja w e' uma componente.
 */
real algDot4( Vector v1, Vector v2 );

/**
 *   Projeta um vetor do R3 em outro, e retorna o vetor resultante (ignorando a componente w).
 */
Vector algProj( Vector ofVector, Vector ontoVector );

/**
 *   Produto vetorial entre dois vetores do R3 (ignorando a componente w).
 */
Vector algCross( Vector v1, Vector v2 );

/**
 *   Reflete um vetor do R3 em torno de outro (ignorando a componente w).
 */
Vector algReflect( Vector ofVector, Vector aroundVector );

/**
 *   Combinação linear de N vetores.
 *
 *   O primeiro parâmetro indica N, o número de vetores sendo combinados. Os parâmetros
 *   seguintes são sempre pares [real scalar, Vector vector]. O vetor resultante é a
 *   soma de todos os vetores (scalar * vector) fornecidos, e normalizado (w = 1).
 *
 *   Exemplo: para obter o vetor 2 * v1 + 4 * v2 + 8 * v3, use:
 *      vector = algLinComb( 3, 2.0, v1, 4.0, v2, 8.0, v3 );
 *
 *   @warning Sempre passe os escalares como real explicitamente (1.0 ao invés de 1).
 */
Vector algLinComb( int count, ... );

/**
 *  Cria uma matriz 4x4.
 */
Matrix algMatrix4x4( real a11, real a12, real a13, real a14, 
                     real a21, real a22, real a23, real a24,
                     real a31, real a32, real a33, real a34,
                     real a41, real a42, real a43, real a44);

/**
 * Retorna o elemento i,j da matriz 4x4.
 * Os valores validos de i e j sao 1,2,3 e 4.
 */
real algGetElem(Matrix m, int i, int j);
 
/**
 * Matriz para o OpenGL.
 * Retorna um ponteiro do tipo real * para ser passado para o 
 * OpenGL como sendo a variavel que contem a matriz de 
 * transformacao.
 */
real * algGetOGLMatrix(Matrix *m);

/**
 *   Multiplica um vetor por uma matriz.
 */
Vector algTransf( Matrix matrix, Vector vector );

/**
 * Matriz identidade.
 */
Matrix algMatrixIdent(void);

/**
 * Matriz de translacao tx, ty, tz.
 */
Matrix algMatrixTransl(real tx, real ty, real tz);

/**
 * Matriz de escala.
 */
Matrix algMatrixScale(real scalex, real scaley, real scalez);

/**
 * Matriz de rotacao em torno de um eixo.
 */
Matrix algMatrixRotate(real angle, real ex, real ey,  real ez);

/**
 * Soma duas matrizes colocando o resultando na terceira matriz.
 */
Matrix algMatrixAdd( Matrix m1, Matrix m2 );

/**
 * Multiplica uma matriz por uma escalar retornando o a matriz resultante.
 */
Matrix algMatrixScalarProduct( real scale, Matrix m );

/**
 *   Multiplica duas matrizes colocando o resultando na terceira matriz.
 */
Matrix algMult( Matrix m1, Matrix m2 );

/**
 * Calcula o determinante de uma matriz.
 */
real algDet( Matrix matrix );

/**
 * Calcula a matriz inversa, retornando 0 caso nao consiga
 */
Matrix algInv( Matrix matrix);

/**
 * Resolve o sistema Mx=b.
 */
Vector algSolve( Matrix matrix, Vector b );

/**
 * Transpõe uma Matriz
 */
Matrix algTransp( Matrix m );

Vector algSnell(Vector in, Vector normal, double n1, double n2);

/**
 * Cria um quaternio 
 */
Quat algQuat(real w, real x,  real y,  real z); 

/**
 * Soma de quaternios.
 */
Quat  algQuatAdd(Quat q1, Quat q2);

/**
 * Subtracao de quaternios.
 */
Quat  algQuatSub(Quat q1, Quat q2);

/**
 * Multiplicacao de quaternio por escalar.
 */
Quat  algQuatScale(real s, Quat q1);

/**
 * Multiplicacao de quaternios.
 */
Quat  algQuatMult(Quat q1, Quat q2);

/**
 * Produto interno de quaternios.
 */
real algQuatDot(Quat q1, Quat q2);

/**
 * Norma de quaternios.
 */
real algQuatNorm(Quat q1);

/**
 * Norma euclidiana de quaternios.
 */
real algQuatEuclidianNorm(Quat q1);

/**
 * Soma de quaternios.
 */
Quat  algQuatUnit(Quat q1);

/**
 * Conjugado de um quaternio.
 */
Quat  algQuatConj(Quat q1);

/**
 * Inverso de um quaternio.
 */
Quat  algQuatInv(Quat q1);

/**
 * Interpolacao linear esferiaca de quaternios, SLERP (Spherical Linear Interpolation). 
 */
Quat  algQuatSLERP(Quat q1, Quat q2, real u);

/**
 *  Quaternio que representa uma rotacao de "angle" (em radianos) em torno do eixo "axis"
 *  que e' definido por um vetor que pode nao ser unitario.
 */
Quat algQuatFromRot(real angle, Vector axis);

/**
 *  Angulo de rotacao representado em um quaternio.
 */
real algQuatRotAngle(Quat q);

/**
 *  Eixo de rotacao representado em um quaternio.
 */
Vector algQuatRotAxis(Quat q);

/**
 *  Quaternio correspondente a uma matriz de rotacao.
 */
Quat algQuatFromMatrix(Matrix m);

/**
 *  Matriz de rotacao representada por um quaternio.
 */
Matrix algQuatToMatrix(Quat q);

/**
 * Imprime um vetor.
 */
void algVectorPrint(char *text, Vector v);

/**
 * Imprime uma matriz.
 */
void algMatrixPrint(char *text, Matrix m);

/**
 * Imprime um quaternio.
 */
void algQuatPrint(char *text, Quat v);

#undef real

#ifdef __cplusplus
}
#endif

#endif
