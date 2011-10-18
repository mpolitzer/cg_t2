/**
 *	@file color.h Color: manipulação de cores.
 *
 *	@author 
 *			- Maira Noronha
 *			- Thiago Bastos
 *          
 *
 *	@date
 *			Criado em:			01 de Dezembro de 2002
 *			Última Modificação:	22 de Janeiro de 2003
 *
 *	@version 2.0
 */

#ifndef	_COLOR_H_
#define	_COLOR_H_

/************************************************************************/
/* Tipos Exportados                                                     */
/************************************************************************/
/**
 *   Cor rgb.
 */
typedef struct color_impl
{
   /**
    * Componente vermelha.
    */
	float red;
   /**
    * Componente verde.
    */
	float green;
   /**
    * Componente azul.
    */
	float blue;
}
Color;

/************************************************************************/
/* Funções Exportadas                                                   */
/************************************************************************/
/**
 *	Cria uma cor a partir de 3 bytes RGB.
 */
Color colorCreate3b( unsigned char red, unsigned char green, unsigned char blue );

/**
 *	Soma duas cores.
 */
Color colorAddition( Color c1, Color c2 );

/**
 *	Amplia ou reduz o brilho de uma cor, multiplicando-o por um fator.
 */
Color colorScale( double scale, Color color );

/**
 *	Multiplica duas cores.
 *	Útil para obter a cor de um material iluminado por uma luz ambiente.
 */
Color colorMultiplication( Color c1, Color c2 );

/**
 *	Cor refletida por um material iluminado por uma certa luz, usando um
 *	coeficiente especificado (scale). Útil nos cálculos de iluminação.
 */
Color colorReflection( double scale, Color light, Color material );

/**
 *	Torna a faixa de valores de r, g e b entre 0 e 1.
 */
Color colorNormalize( Color color );

#endif

