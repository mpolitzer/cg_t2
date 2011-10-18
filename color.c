/**
 *	@file color.c Color: manipulação de cores.
 *
 *	@author 
 *			- Maira Noronha
 *			- Thiago Bastos
 *
 *	@date
 *			Criado em:			1 de Dezembro de 2002
 *			Última Modificação:	22 de Janeiro de 2003
 *
 *	@version 2.0
 */

#include "color.h"

#include <stdio.h>


/************************************************************************/
/* Constantes Privadas                                                  */
/************************************************************************/
#define MAX( a, b ) ( ( a > b ) ? a : b )
#define MIN( a, b ) ( ( a < b ) ? a : b )

/************************************************************************/
/* Definições das Funções Exportadas                                    */
/************************************************************************/
Color colorCreate3b( unsigned char red, unsigned char green, unsigned char blue )
{
	Color color= { (float)( red / 255.0 ), (float)( green / 255.0 ), (float)( blue / 255.0 ) };

	return color;
}

Color colorAddition( Color c1, Color c2 )
{
	Color color = { (float)( c1.red + c2.red ), (float)( c1.green + c2.green ), (float)( c1.blue + c2.blue ) };

	return color;
}

Color colorScale( double s, Color c )
{
	Color color = { (float)( s * c.red ), (float)( s * c.green ), (float)( s * c.blue ) };

	return color;
}

Color colorMultiplication( Color c1, Color c2 )
{
	Color color = { (float)( c1.red * c2.red ), (float)( c1.green * c2.green ), (float)( c1.blue * c2.blue ) };

	return color;
}

Color colorReflection( double s, Color l, Color m )
{
	Color color = { (float)( s * ( l.red * m.red ) ), (float)( s * ( l.green * m.green ) ), (float)( s * ( l.blue * m.blue ) ) };
	
	return color;
}

Color colorNormalize( Color c1 )
{
	Color color= { (float)(c1.red/255.0), (float)(c1.green/255.0), (float)(c1.blue/255.0) };

	return color;
}

