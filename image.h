/**
 *	@file image.h Image: operacoes imagens em formato TGA.
 *
 *	@author 
 *          - Maira Noronha
 *			- Fabiola Maffra
 *          - Marcelo Gattass
 *
 *	@date
 *			Ultima Modificacao:		23 de Abril de 2003
 *
 *	@version 2.0
 */

#ifndef IMAGE_H
#define IMAGE_H

#include "color.h"

typedef struct Image_imp Image;

/************************************************************************/
/* Funcoes Exportadas                                                   */
/************************************************************************/

/**
 *	Cria uma nova imagem com as dimensoes especificadas.
 *
 *	@param w Largura da imagem.
 *	@param h Altura da imagem.
 *
 *	@return Handle da imagem criada.
 */
Image * imgCreate (int w, int h);

/**
 *	Destroi a imagem.
 *
 *	@param image imagem a ser destruida.
 */
void    imgDestroy (Image *image);

/**
*	Cria uma nova nova copia imagem dada.
*
*	@param image imagem a ser copiada.
*
*	@return Handle da imagem criada.
*/
Image * imgCopy(Image * image);

/**
*	Cria uma nova nova copia imagem dada em tons de cinza.
*
*	@param image imagem a ser copiada em tons de cinza.
*
*	@return Handle da imagem criada.
*/
Image * imgGrey(Image * image);

/**
*	Redimensiona a imagem especificada.
*
*	@param image Handle para uma imagem.
*	@param w1 Nova largura da imagem.
*	@param h1 Nova altura da imagem.
*  @return imagem criada.
*/
Image * imgResize(Image *img0, int w1, int h1);

/**
*	Ajusta a largura da imagem para uma potencia de 2.
*
*	@param img0 imagem a ser ajustada.
*/
Image * imgAdjust2eN(Image *img0);

/**
*	Obtem a largura (width) de uma imagem.
*
*	@param image Handle para uma imagem.
*	@return  a largura em pixels (width) da imagem.
*/
int imgGetWidth(Image * image);

/**
*	Obtem a altura (heigth) de uma imagem.
*
*	@param image Handle para uma imagem.
*	@return  a altura em pixels (height) da imagem.
*/
int imgGetHeight(Image * image);

/**
*	Obtem as dimensoes de uma imagem.
*
*	@param image Handle para uma imagem.
*	@param w [out]Retorna a largura da imagem.
*	@param h [out]Retorna a altura da imagem.
*/
float * imgGetRGBData(Image * image);

/**
*	Ajusta o pixel de uma imagem com a cor especificada.
*
*	@param image Handle para uma imagem.
*	@param x Posicao x na imagem.
*	@param y Posicao y na imagem.
*	@param color Cor do pixel(valor em float [0,1]).
*/
void imgSetPixel3fv(Image * image, int x, int y, float * color);

/**
*	Obtem o pixel de uma imagem na posicao especificada.
*
*	@param image Handle para uma imagem.
*	@param x Posicao x na imagem.
*	@param y Posicao y na imagem.
*	@param color [out] Pixel da posicao especificada(valor em float [0,1]).
*/
void imgGetPixel3fv(Image * image, int x, int y, float *color);

/**
*	Ajusta o pixel de uma imagem com a cor especificada.
*
*	@param image Handle para uma imagem.
*	@param x Posicao x na imagem.
*	@param y Posicao y na imagem.
*	@param color Cor do pixel (valor em unsigend char[0,255]).
*/
void imgSetPixel3ubv(Image * image, int x, int y, unsigned char * color);

/**
*	Obtem o pixel de uma imagem na posicao especificada.
*
*	@param image Handle para uma imagem.
*	@param x Posicao x na imagem.
*	@param y Posicao y na imagem.
*	@param color [out] Pixel da posicao especificada (valor em unsigend char[0,255]).
*/
void imgGetPixel3ubv(Image * image, int x, int y, unsigned char *color);

/**
 *	Ajusta o pixel de uma imagem com a cor especificada.
 *
 *	@param image Handle para uma imagem.
 *	@param x Posicao x na imagem.
 *	@param y Posicao y na imagem.
 *	@param color Cor do pixel.
 */
void imageSetPixel(Image *image, int x, int y, Color color);

/**
 *	Obtem o pixel de uma imagem na posicao especificada.
 *
 *	@param image Handle para uma imagem.
 *	@param x Posicao x na imagem.
 *	@param y Posicao y na imagem.
 *
 *	@return Pixel da posicao especificada.
 */
Color imageGetPixel(Image *image, int x, int y);



/**
 *	Le a imagem a partir do arquivo especificado.
 *
 *	@param filename Nome do arquivo de imagem.
 *
 *	@return imagem criada.
 */
Image * imageLoad(char *filename);

/**
 *	Salva a imagem no arquivo especificado em formato TGA.
 *
 *	@param filename Nome do arquivo de imagem.
 *	@param image Handle para uma imagem.
 *
 *	@return retorna 1 caso nao haja erros.
 */
int imageWriteTGA(char *filename, Image *image);

/**
*	Le a imagem a partir do arquivo especificado.
*
*	@param filename Nome do arquivo de imagem.
*
*	@return imagem criada.
*/
Image * imgReadBMP (char *filename);

/**
*	Salva a imagem no arquivo especificado em formato BMP.
*
*	@param filename Nome do arquivo de imagem.
*	@param bmp Handle para uma imagem.
*
*	@return retorna 1 caso nao haja erros.
*/
int imgWriteBMP(char *filename, Image * bmp);

/*** FUNCOES QUE DEVEM SER IMPLEMENTADAS NO TRABALHO 1 ***/

/**
*	Conta o numero de cores diferentes na imagem
*
*	@param image Handle para uma imagem.
*	@param w Nova largura da imagem.
*	@param h Nova altura da imagem.
*/
unsigned int imgCountColors(Image * image);

/**
*	Normaliza as cores da imagem (eliminando o 
*  efeito da iluminacao.
*
*	@param image Handle para uma imagem.
*
*  @return Handle para nova imagem normalizada.
*/
Image * imgNormalizeColors(Image * image);

/**
*	Reduz o numero de cores distintas
*
*	@param image Handle para uma imagem.
*	@param maxCores numero de cores que a nova imagem deve ter.
*
*  @return Handle para a nova imagem.
*/
Image * imgReduceColors(Image * image, int maxCores);

/**
*	Subtrai uma imagem de outra.
*
*	@param img0 img0 =  |img0-img1|.
*	@param img1.
*/
void imgSub(Image *img0, Image *img1);

/**
*	Passa um filtro gaussiano na imagem.
*
*	@param img0 imagem a ser filtrada.
*
*	@return imagem filtrada.
*/
Image * imgGaussFilter(Image * img0);

/**
*	Passa um filtro laplaciano na imagem.
*
*	@param img0 imagem a ser filtrada.
*
*	@return imagem filtrada.
*/
Image * imgLaplcFilter(Image * img0);

/**
*	Passa um filtro log.
*
*	@param img0 imagem a ser filtrada.
*
*	@return imagem filtrada.
*/
Image * imgEdges(Image * img0);

/**
*	Passa um filtro de mediana na imagem.
*
*	@param img0 imagem a ser filtrada.
*
*	@return imagem filtrada.
*/
Image * imgFiltroDeMediana(Image * img0);

/**
*	transforma a imagem em uma imagem binaria(preta e branca).
*
*	@param img0 imagem a ser binarizada.
*
*	@return imagem binarizada.
*/
Image * imgBinarizacao(Image * img0);

#endif
