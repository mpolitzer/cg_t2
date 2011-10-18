/**
 *	@file image.c Image: operações imagens em formato TGA.
 *
 *	@author 
 *          - Maira Noronha
 *			- Fabiola Maffra
 *          - Marcelo Gattass
 *
 *	@date
 *			Última Modificação:		23 de Abril de 2003
 *
 *	@version 2.0
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include "color.h"
#include "image.h"

/* Compiler dependent definitions */
typedef unsigned char BYTE;       
typedef unsigned short int USHORT;  
typedef unsigned short int WORD;  
typedef long int LONG;            
typedef unsigned long int DWORD;

#define ROUND(_) (int)floor( (_) + 0.5 )

struct Image_imp {

	int width;		/* numero de pixels na direcao horizontal da imagem */
	int height;		/* numero de pixels na direcao vertical da imagem   */
	float *buf;		/* vetor de dimensao 3*width*height que armazena consecutivamente as 3 componentes de cor de cada pixel a */
					/* partir do canto inferior esquerdo da imagem.  A posicao das 3 componentes de cor do pixel (x,y)        */
					/* fica armazenada a partir da posicao (y*width*3) + (x*3)                                                */
};

typedef struct
{
	Color max;     /* maximos R's, G's e B's */
	Color min;	   /* minimos R's, G's e B's */
	float var;	   /* variacao em cada cubo  */
	int ini;	   /* final do cubo  */
	int fim;       /* inicio do cubo */
	int maiorDim;  /* R=0, G=1, B=2  */
	int checked;
} colorCube;

/************************************************************************/
/* Definicao das Funcoes Privadas                                       */
/************************************************************************/

/*  getuint e putuint:
* Funcoes auxiliares para ler e escrever inteiros na ordem (lo-hi)
* Note  que  no Windows as variaveis tipo "unsigned short int" sao 
* armazenadas  no disco em dois bytes na ordem inversa. Ou seja, o 
* numero  400, por exemplo, que  pode ser escrito como 0x190, fica 
* armazenado  em dois bytes consecutivos 0x90 e 0x01. Nos sistemas 
* UNIX e Mac este mesmo inteiro seria armazenado  na  ordem 0x01 e 
* 0x90. O armazenamento do Windows e' chamado  de  "little endian"  
* (i.e., lowest-order byte stored first), e  no  sitemas  Unix sao
* "big-endian" (i.e., highest-order byte stored first). 
*/

/***************************************************************************
* Reads an unsigned integer from input                                     *
***************************************************************************/
static int getuint(unsigned short *_uint, FILE *input)
{
	int got;
	unsigned char temp[2];
	unsigned short tempuint;

	got = (int) fread(&temp, 1, 2, input);
	if (got != 2) return 0;

	tempuint = ((unsigned short)(temp[1])<<8) | ((unsigned short)(temp[0]));

	*_uint = tempuint;

	return 1;
}

/***************************************************************************
* Writes an unsigned integer in output                                     *
***************************************************************************/
static int putuint(unsigned short _uint, FILE *output)
{
	int put;
	unsigned char temp[2];

	temp[0] = _uint & 0xff;
	temp[1] = (_uint >> 8) & 0xff;

	put = (int) fwrite(&temp, 1, 2, output);
	if (put != 2) return 0;

	return 1;
}

/***************************************************************************
* Reads a long integer from input                                          *
***************************************************************************/ 
static int getlong(FILE *input, long int *longint)
{
	int got;
	unsigned char temp[4];
	long int templongint;

	got = (int)fread(&temp, 1, 4, input);
	if (got != 4) return 0;

	templongint = ((long int)(temp[3])<<24) | ((long int)(temp[2])<<16) 
		| ((long int)(temp[1])<<8) | ((long int)(temp[0]));

	*longint = templongint;

	return 1;
}

/***************************************************************************
* Writes a long integer in output                                          *
***************************************************************************/ 
static int putlong(FILE *output, long int longint)
{
	int put;
	unsigned char temp[4];

	temp[0] = (unsigned char)longint & 0xff;
	temp[1] = (unsigned char)(longint >> 8) & 0xff;
	temp[2] = (unsigned char)(longint >> 16) & 0xff;
	temp[3] = (unsigned char)(longint >> 24) & 0xff;

	put = (int)fwrite(&temp, 1, 4, output);

	if (put != 4) return 0;

	return 1;
}

/***************************************************************************
* Reads a word from input                                                  *
***************************************************************************/ 
static int getword(FILE *input, unsigned short int *word)
{
	int got;
	unsigned char temp[2];
	unsigned short int tempword;

	got = (int)fread(&temp, 1, 2, input);
	if (got != 2) return 0;

	tempword = ((unsigned short int)(temp[1])<<8) | ((unsigned short int)(temp[0]));

	*word = tempword;

	return 1;
}

/***************************************************************************
* Writes a word in output                                                  *
***************************************************************************/ 
static int putword(FILE *output, unsigned short int word)
{
	int put;
	unsigned char temp[2];

	temp[0] = word & 0xff;
	temp[1] = (word >> 8) & 0xff;

	put = (int)fwrite(&temp, 1, 2, output);
	if (put != 2) return 0;

	return 1;
}

/***************************************************************************
* Reads a double word from input                                           *
***************************************************************************/ 
static int getdword(FILE *input, unsigned long int *dword)
{
	int got;
	unsigned char temp[4];
	unsigned long int tempdword;

	got = (int)fread(&temp, 1, 4, input);
	if (got != 4) return 0;

	tempdword = ((unsigned long int)(temp[3])<<24) | ((unsigned long int)(temp[2])<<16) 
		| ((unsigned long int)(temp[1])<<8) | ((unsigned long int)(temp[0]));

	*dword = tempdword;

	return 1;
}

/***************************************************************************
* Writes a double word in output                                           *
***************************************************************************/ 
static int putdword(FILE *output, unsigned long int dword)
{
	int put;
	unsigned char temp[4];

	temp[0] = (unsigned char) (dword & 0xff);
	temp[1] = (unsigned char) ((dword >>  8) & 0xff);
	temp[2] = (unsigned char) ((dword >> 16) & 0xff);
	temp[3] = (unsigned char) ((dword >> 24) & 0xff);

	put = (int)fwrite(&temp, 1, 4, output);

	if (put != 4) return 0;

	return 1;
}

/************************************************************************/
/* Definicao das Funcoes Exportadas                                     */
/************************************************************************/

Image * imgCreate(int w, int h)
{
	Image * image = (Image*) malloc (sizeof(Image));
	assert(image);
	image->width  =(unsigned int) w;
	image->height =(unsigned int) h;
	image->buf = (float *) malloc (w * h * 3*sizeof(float));
	assert(image->buf);
	return image;
}

void imgDestroy (Image *image)
{
   if (image)
   {
      if (image->buf) free (image->buf);
      free(image);
   }
}

Image * imgCopy(Image * image)
{
	int w = imgGetWidth(image);
	int h = imgGetHeight(image);
	Image * img1=imgCreate(w,h);
	int x,y;
	float rgb[3];

	for (y=0;y<h;y++){
		for (x=0;x<w;x++) {
			imgGetPixel3fv(image,x,y,rgb);
			imgSetPixel3fv(img1,x,y,rgb);
		}
	}

	return img1;
}

Image * imgGrey(Image * image)
{
	int w = imgGetWidth(image);
	int h = imgGetHeight(image);
	Image * img1=imgCreate(w,h);
	int x,y;
	float rgb[3],grey[3];

	for (y=0;y<h;y++){
		for (x=0;x<w;x++) {
			imgGetPixel3fv(image,x,y,rgb);
			grey[0]=0.2126f*rgb[0]+0.7152f*rgb[1]+0.0722f*rgb[2];
			grey[1]=grey[0];
			grey[2]=grey[0];
			imgSetPixel3fv(img1,x,y,grey);
		}
	}

	return img1;
}

Image * imgResize(Image * img0, int w1, int h1)
{
	Image *img1 = imgCreate(w1,h1);
	int w0, h0;
	int x0,y0,x1,y1;
	Color color;

	w0 = imgGetWidth(img0);
	h0 = imgGetHeight(img0);

	for (y1=0;y1<h1;y1++)
		for (x1=0;x1<w1;x1++)
		{
			x0=ROUND(w0*x1/w1);   /* pega a cor do pixel mais proxima */
			y0=ROUND(h0*y1/h1);
			color = imageGetPixel(img0,x0,y0);
			imageSetPixel(img1,x1,y1,color);
		}

		return img1;
}

Image * imgAdjust2eN(Image *img0)
{
	Image * img1;
	int i=0,valid[14]={1,2,4,8,16,32,64,128,512,1024,2048,4096,8192,16384};
	int w0=img0->width;
	int h0=img0->height;
	int w1,h1;
	int x,y;
	float rgb[3],black[3]={0.f,0.f,0.f};

	for (i=0;i<14&&valid[i]<w0;i++);
	w1=valid[i];

	for (i=0;i<14&&valid[i]<h0;i++);
	h1=valid[i];

	img1=imgCreate(w1,h1);

	for (y=0;y<h1;y++){
		for (x=0;x<w1;x++) {
			if (x<w0&&y<h0) {
				imgGetPixel3fv(img0,x,y,rgb);
				imgSetPixel3fv(img1,x,y,rgb);
			}
			else
				imgSetPixel3fv(img1,x,y,black);
		}
	}

	return img1;
}

int imgGetWidth(Image * image)
{
	return image->width;
}

int imgGetHeight(Image * image)
{
	return image->height;
}

float *imgGetRGBData(Image * image)
{
	return image->buf;
}

void imgSetPixel3fv(Image * image, int x, int y, float * color)
{
	int pos = (y*image->width*3) + (x*3);
	image->buf[pos  ] = color[0];
	image->buf[pos+1] = color[1];
	image->buf[pos+2] = color[2];
}

void imgGetPixel3fv(Image * image, int x, int y, float *color)
{
	int pos = (y*image->width*3) + (x*3);
	color[0] = image->buf[pos  ];
	color[1] = image->buf[pos+1];
	color[2] = image->buf[pos+2];
}

void imgSetPixel3ubv(Image * image, int x, int y, unsigned char * color)
{
	int pos = (y*image->width*3) + (x*3);

	image->buf[pos  ] = (float)(color[0]/255.);
	image->buf[pos+1] = (float)(color[1]/255.);
	image->buf[pos+2] = (float)(color[2]/255.);
}

void imgGetPixel3ubv(Image * image, int x, int y, unsigned char *color)
{
	int pos = (y*image->width*3) + (x*3);
	int r= ROUND(255*image->buf[pos]);
	int g= ROUND (255*image->buf[pos+1]);
	int b= ROUND (255*image->buf[pos+2]);
	color[0] = (unsigned char)(r<256) ? r : 255 ;
	color[1] = (unsigned char)(g<256) ? g : 255 ;
	color[2] = (unsigned char)(b<256) ? b : 255 ;
}

void imageSetPixel(Image *image, int x, int y, Color color)
{
	int pos = (y*image->width*3) + (x*3);

	image->buf[pos  ] = (float)(color.red);
	image->buf[pos+1] = (float)(color.green);
	image->buf[pos+2] = (float)(color.blue);
}

Color imageGetPixel(Image *image, int x, int y)
{
	Color color;
	int pos = (y*image->width*3) + (x*3);

	color.red   = (image->buf[pos]);
	color.green = (image->buf[pos+1]);
	color.blue  = (image->buf[pos+2]);

	return color;
}

Image * imageLoad(char *filename) 
{
	FILE        *filePtr;

	Image       *image;                /* imagem a ser criada */
	unsigned char *buffer;             /* buffer para ler o vetor de rgb da imagem  */

	unsigned char imageType;           /* 2 para imagens RGB */ 
	unsigned short int imageWidth;     /* largura da imagem  */
	unsigned short int imageHeight;    /* altura da imagem   */
	unsigned char bitCount;            /* numero de bits por pixel */

	int x,y;     /* posicao de um pixel  */

	unsigned char ucharSkip;      /* dado lixo unsigned char */
	short int     sintSkip;       /* dado lixo short int */

	/* abre o arquivo com a imagem TGA */
	filePtr = fopen(filename, "rb");
	assert(filePtr);

	/* pula os primeiros dois bytes que devem ter valor zero */
	ucharSkip = getc(filePtr); /* tamanho do descritor da imagem (0) */
	if (ucharSkip != 0) printf("erro na leitura de %s: imagem com descritor\n", filename);

	ucharSkip = getc(filePtr); 
	if (ucharSkip != 0) printf("erro na leitura de %s: imagem com tabela de cores\n", filename);

	/* le o tipo de imagem (que deve ser obrigatoriamente 2).  
	nao estamos tratando dos outros tipos */
	imageType=getc(filePtr);
	assert(imageType == 2);

	/* pula 9 bytes relacionados com a tabela de cores 
	(que nao existe quando a imagem e' RGB, imageType=2) */
	getuint((short unsigned int *)&sintSkip,filePtr);
	getuint((short unsigned int *)&sintSkip,filePtr);
	ucharSkip = getc(filePtr); 

	/* especificacao da imagem */
	getuint((short unsigned int *)&sintSkip,filePtr);      /* origem em x (por default = 0) */
	getuint((short unsigned int *)&sintSkip,filePtr);      /* origem em y (por default = 0) */ 
	getuint(&imageWidth,filePtr);    /* largura */
	getuint(&imageHeight,filePtr);   /* altura  */

	/* read image bit depth */
	bitCount=getc(filePtr);
	assert(bitCount == 24);  /* colorMode -> 3 = BGR (24 bits) */

	/* read 1 byte of garbage data */
	ucharSkip = getc(filePtr); 

	/* cria uma instancia do tipo Imagem */
	image = imgCreate(imageWidth,imageHeight);
	buffer = (unsigned char *) malloc(3*imageWidth*imageHeight*sizeof(unsigned char));
	assert(image);
	assert(buffer);

	/* read in image data */
	fread(buffer, sizeof(unsigned char), 3*imageWidth*imageHeight, filePtr);

	/* copia e troca as compontes de BGR para RGB */
	for (y=0;y<imageHeight;y++) {
		for (x=0;x<imageWidth;x++) {
			int pos = (y*imageWidth*3) + (x*3);
			image->buf[pos  ] = (float)(buffer[pos+2]/255.);
			image->buf[pos+1] = (float)(buffer[pos+1]/255.);
			image->buf[pos+2] = (float)(buffer[pos  ]/255.);
		}
	}

	free(buffer);
	fclose(filePtr);
	return image;
}

int imageWriteTGA(char *filename, Image * image)
{
	unsigned char imageType=2;      /* RGB(A) sem compressão */
	unsigned char bitDepth=24;      /* 24 bits por pixel     */

	FILE          *filePtr;        /* ponteiro do arquivo    */
	unsigned char * buffer;        /* buffer de bytes        */
	int  x,y;   

	unsigned char byteZero=0;      /* usado para escrever um byte zero no arquivo      */
	short int     shortZero=0;     /* usado para escrever um short int zero no arquivo */

	if (!image) return 0;

	/* cria um arquivo binario novo */
	filePtr = fopen(filename, "wb");
	assert(filePtr);

	/* cria o buffer */
	buffer = (unsigned char *) malloc(3*image->width*image->height*sizeof(unsigned char));
	assert(buffer);

	/* copia e troca as compontes de BGR para RGB */
	for (y=0;y<image->height;y++) {
		for (x=0;x<image->width;x++) {
			int pos = (y*image->width*3) + (x*3);
			int r= ROUND (255*image->buf[pos  ]);
			int g= ROUND (255*image->buf[pos+1]);
			int b= ROUND (255*image->buf[pos+2]);
			buffer[pos+2] = (unsigned char)(r<256) ? r : 255 ;
			buffer[pos+1] = (unsigned char)(g<256) ? g : 255 ;
			buffer[pos  ] = (unsigned char)(b<256) ? b : 255 ;
		}
	}

	/* escreve o cabecalho */
	putc(byteZero,filePtr);          /* 0, no. de caracteres no campo de id da imagem     */
	putc(byteZero,filePtr);          /* = 0, imagem nao tem palheta de cores              */
	putc(imageType,filePtr);         /* = 2 -> imagem "true color" (RGB)                  */
	putuint(shortZero,filePtr);      /* info sobre a tabela de cores (inexistente)        */
	putuint(shortZero,filePtr);      /* idem                                              */
	putc(byteZero,filePtr);          /* idem                                              */
	putuint(shortZero,filePtr);      /* =0 origem em x                                    */
	putuint(shortZero,filePtr);      /* =0 origem em y                                    */
	putuint(image->width,filePtr);   /* largura da imagem em pixels                       */
	putuint(image->height,filePtr);  /* altura da imagem em pixels                        */
	putc(bitDepth,filePtr);          /* numero de bits de um pixel                        */
	putc(byteZero, filePtr);         /* =0 origem no canto inf esquedo sem entrelacamento */

	/* escreve o buf de cores da imagem */
	fwrite(buffer, sizeof(unsigned char), 3*image->width*image->height, filePtr);

	free(buffer);
	fclose(filePtr);
	return 1;
}

Image * imgReadBMP(char *filename)
{
	FILE  *filePtr;				/* ponteiro do arquivo */
	Image *image;				/* imagem a ser criada */
	BYTE *linedata;

	USHORT  bfType;             /* "BM" = 19788           */
	LONG    biWidth;            /* image width in pixels  */
	LONG    biHeight;           /* image height in pixels */
	WORD    biBitCount;         /* bitmap color depth     */
	DWORD   bfSize;

	USHORT  ushortSkip;			/* dado lixo USHORT */
	DWORD   dwordSkip;			/* dado lixo DWORD  */
	LONG    longSkip;			/* dado lixo LONG   */
	WORD    wordSkip;			/* dado lixo WORD   */

	LONG i, j, k, l, linesize, got;

	/* abre o arquivo com a imagem BMP */
	filePtr = fopen(filename, "rb");
	assert(filePtr);

	/* verifica se eh uma imagem bmp */
	getuint(&bfType, filePtr);
	assert(bfType == 19778);

	/* pula os 12 bytes correspondentes a bfSize, Reserved1 e Reserved2 */
	getdword(filePtr, &bfSize);
	getuint(&ushortSkip, filePtr);  /* Reserved1, deve ter valor 0 */
	assert(ushortSkip == 0);
	getuint(&ushortSkip, filePtr);  /* Reserved2, deve ter valor 0 */
	assert(ushortSkip == 0);

	/* pula os 4 bytes correspondentes a bfOffBits, que deve ter valor 54 */
	getdword(filePtr, &dwordSkip);
	assert(dwordSkip == 54);

	/* pula os 4 bytes correspondentes a biSize, que deve ter valor 40 */
	getdword(filePtr, &dwordSkip);
	assert(dwordSkip == 40);

	/* pega largura e altura da imagem */
	getlong(filePtr, &biWidth);
	getlong(filePtr, &biHeight);

	/* verifica que o numero de quadros eh igual a 1 */
	getword(filePtr, &wordSkip);
	assert(wordSkip == 1);

	/* Verifica se a imagem eh de 24 bits */
	getword(filePtr, &biBitCount);
	if(biBitCount != 24)
	{
		fprintf(stderr, "imgReadBMP: Not a bitmap 24 bits file.\n");		
		fclose(filePtr);
		return (NULL);
	}

	/* pula os demais bytes do infoheader */
	getdword(filePtr, &dwordSkip);
	assert(dwordSkip == 0);
	getdword(filePtr, &dwordSkip);
	getlong(filePtr, &longSkip);
	getlong(filePtr, &longSkip);
	getdword(filePtr, &dwordSkip);
	getdword(filePtr, &dwordSkip);

	image = imgCreate(biWidth, biHeight);

	/* a linha deve terminar em uma fronteira de dword */
	linesize = 3*image->width;
	if (linesize & 3) {
		linesize |= 3;
		linesize++;
	}

	/* aloca espaco para a area de trabalho */ 
	linedata = (BYTE *) malloc(linesize);
	if (linedata == NULL) {
		fprintf(stderr, "get24bits: Not enough memory.\n");
		return 0;
	}

	/* pega as componentes de cada pixel */
	for (k=0, i=0; i<image->height; i++) {
		got = (unsigned long int)fread(linedata, linesize, 1, filePtr);
		if (got != 1) {
			free(linedata);
			fprintf(stderr, "get24bits: Unexpected end of file.\n");
		}
		for (l=1, j=0; j<image->width; j++, l=l+3) {
			image->buf[k++] = (float)(linedata[l+1]/255.);
			image->buf[k++] = (float)(linedata[l  ]/255.); 
			image->buf[k++] = (float)(linedata[l-1]/255.);
		}
	}

	free(linedata);
	fclose(filePtr);
	return image;
}

int imgWriteBMP(char *filename, Image * bmp)
{
	FILE          *filePtr;		   /* ponteiro do arquivo */
	unsigned char *filedata;
	DWORD bfSize;
	int i, j, k, l;

	int linesize, put;

	if (!bmp) return 0;

	/* cria um novo arquivo binario */
	filePtr = fopen(filename, "wb");
	assert(filePtr);

	/* a linha deve terminar em uma double word boundary */
	linesize = bmp->width * 3;
	if (linesize & 3) {
		linesize |= 3;
		linesize ++;
	}

	/* calcula o tamanho do arquivo em bytes */
	bfSize = 14 +							/* file header size */
		40 +							/* info header size */
		bmp->height * linesize;	    /* image data  size */

	/* Preenche o cabeçalho -> FileHeader e InfoHeader */
	putuint(19778, filePtr);					/* type = "BM" = 19788								 */
	putdword(filePtr, bfSize);					/* bfSize -> file size in bytes						 */
	putuint(0, filePtr);						/* bfReserved1, must be zero						 */
	putuint(0, filePtr);						/* bfReserved2, must be zero						 */
	putdword(filePtr, 54);						/* bfOffBits -> offset in bits to data				 */

	putdword(filePtr, 40);						/* biSize -> structure size in bytes				 */
	putlong(filePtr, bmp->width);				/* biWidth -> image width in pixels					 */			
	putlong(filePtr, bmp->height);				/* biHeight -> image height in pixels				 */
	putword(filePtr, 1);						/* biPlanes, must be 1								 */
	putword(filePtr, 24);						/* biBitCount, 24 para 24 bits -> bitmap color depth */
	putdword(filePtr, 0);						/* biCompression, compression type -> no compression */
	putdword(filePtr, 0);						/* biSizeImage, nao eh usado sem compressao			 */
	putlong(filePtr, 0);						/* biXPelsPerMeter									 */
	putlong(filePtr, 0);						/* biYPelsPerMeter									 */
	putdword(filePtr, 0);						/* biClrUsed, numero de cores na palheta			 */
	putdword(filePtr, 0);						/* biClrImportant, 0 pq todas sao importantes		 */

	/* aloca espacco para a area de trabalho */
	filedata = (unsigned char *) malloc(linesize);
	assert(filedata);

	/* a linha deve ser zero padded */
	for (i=0; i<(linesize-(3*bmp->width)); i++) 
		filedata[linesize-1-i] = 0;
	j = 1;
	for (k=0; k<bmp->height;k++)
	{
		l = 1;
		/* coloca as componentes BGR no buffer */    
		for (i=0; i<bmp->width; i++) {
			int b= ROUND (255*bmp->buf[j+1]);
			int g= ROUND (255*bmp->buf[j  ]);
			int r= ROUND (255*bmp->buf[j-1]);
			filedata[l-1] = (unsigned char)(b<256) ? b : 255 ;
			filedata[l  ] = (unsigned char)(g<256) ? g : 255 ;
			filedata[l+1] = (unsigned char)(r<256) ? r : 255 ;
			j+=3; l+=3;
		}

		/* joga para o arquivo */
		put = (int)fwrite(filedata, linesize, 1, filePtr);
		if (put != 1) {
			fprintf(stderr, "put24bits: Disk full.");
			free(filedata);
			return 0;
		}
	}

	/* operacao executada com sucesso */
	fprintf(stdout,"imgWriteBMP: %s successfuly generated\n",filename);
	free(filedata);
	fclose(filePtr);
	return 1;
}

static int comparaCor(const void * p1, const void * p2)
{
	float *c1 = (float *) p1;  /* aponta para o byte red da cor 1 */
	float *c2 = (float *) p2;  /* aponta para o byte red da cor 2 */

	/* compara o canal vermelho */
	if (*c1 < *c2) return -1;
	if (*c1 > *c2) return  1;

	/* compara  o canal verde, uma vez que o vermelho e' igual */
	c1++; c2++;
	if (*c1 < *c2) return -1;
	if (*c1 > *c2) return  1;

	/* compara o canal azul, uma vez que o vermelho e o azul sao iguais */
	c1++; c2++;
	if (*c1 < *c2) return -1;
	if (*c1 > *c2) return  1;

	/* sao iguais */
	return 0;
}

unsigned int imgCountColors(Image * img)
{
	unsigned int numCor = 1;
	int w = imgGetWidth(img);
	int h = imgGetHeight(img);
	float* buf=imgGetRGBData(img);
	float *vet=(float*) malloc(3*w*h*sizeof(float));
	int i;

	/* copia o buffer da imagem no vetor de floats fazendo uma quantizacao para 256 tons de cada componente de cor */
	for (i=0;i<3*w*h;i++) {
		float val = (float)(255*buf[i]);
		vet[i]  = (float)(val<256) ? val : 255;
	}

	/* ordena o vetor */
	qsort(vet,w*h,3*sizeof(float),comparaCor);

	/* conta o numero de cores diferentes */
	for (i=3; i<3*w*h; i+=3)
		if (comparaCor(&vet[i-3],&vet[i])!=0) numCor++;

	free(vet);
	return numCor;
}

Image * imgNormalizeColors(Image * img0)
{
	int w = imgGetWidth(img0);
	int h = imgGetHeight(img0);
	Image* img1 = imgCreate(w,h);
	int x,y;
	float rgb[3];


	for (y=0;y<h;y++){
		for (x=0;x<w;x++) {
			imgGetPixel3fv(img0,x,y,rgb);
			imgSetPixel3fv(img1,x,y,rgb);
		}
	}

	return img1;
}

static int comparaR(const void * p1, const void * p2)
{
	Color *color1 = (Color *) p1;
	Color *color2 = (Color *) p2;

	Color c1 = *color1;
	Color c2 = *color2;

	if (c1.red < c2.red) return -1;
	if (c1.red > c2.red) return  1;

	return 0;
}

static int comparaG(const void * p1, const void * p2)
{
	Color *color1 = (Color *) p1;
	Color *color2 = (Color *) p2;

	Color c1 = *color1;
	Color c2 = *color2;

	if (c1.green < c2.green) return -1;
	if (c1.green > c2.green) return  1;

	return 0;
}

static int comparaB(const void * p1, const void * p2)
{
	Color *color1 = (Color *) p1;
	Color *color2 = (Color *) p2;

	Color c1 = *color1;
	Color c2 = *color2;

	if (c1.blue < c2.blue) return -1;
	if (c1.blue > c2.blue) return  1;

	return 0;
}

void caixaEnvolvente(colorCube* cube, Color* colorVec)
{
	float r,g,b;
	int i; 

	/* inicializa com o pior caso */
	cube->min.red = cube->min.green = cube->min.blue = 1.0; 
	cube->max.red = cube->max.green = cube->max.blue = 0.0;

	/* percorre o cubo ajustando o domínio das cores */
	for (i=cube->ini;i<=cube->fim;i++){
		r = colorVec[i].red;
		if (r > cube->max.red) cube->max.red = r;
		if (r < cube->min.red) cube->min.red = r;
		g = colorVec[i].green;
		if (g > cube->max.green) cube->max.green = g;
		if (g < cube->min.green) cube->min.green = g;
		b = colorVec[i].blue;
		if (b > cube->max.blue) cube->max.blue = b;
		if (b < cube->min.blue) cube->min.blue = b;
	}
}

Image* bestColor(Image *img0, Color* pal, Image* img1, int pal_size)
{
	int w = imgGetWidth(img0);
	int h = imgGetHeight(img0);
	float rgb[3];
	int x,y,i;

	for (y=0;y<h;y++){
		for (x=0;x<w;x++){
			int i_menor;
			float m_menor;
			float rgb1[3];

			imgGetPixel3fv(img0,x,y,rgb);

			m_menor = FLT_MAX;
			i_menor = 0;

			for(i=0;i<pal_size;i++)
			{
				float dif[3];
				float m;

				dif[0] = rgb[0] - pal[i].red;
				dif[1] = rgb[1] - pal[i].green;
				dif[2] = rgb[2] - pal[i].blue;

				m = (float)((dif[0]*dif[0]) + (dif[1]*dif[1]) + (dif[2]*dif[2]));

				if (m < m_menor)
				{
					i_menor = i;
					m_menor = m;

					rgb1[0]=pal[i_menor].red;
					rgb1[1]=pal[i_menor].green;
					rgb1[2]=pal[i_menor].blue;
				}
			}

			imgSetPixel3fv(img1,x,y,rgb1);
		}
	}
	return img1;
}

void paleta(Color* pal, colorCube* cubeVec, Color* colorVec, int numCubos)
{
	float sumvar[3];
	int count;
	int i,j;

	for(i=0;i<numCubos;i++)
	{
		sumvar[0] = sumvar[1] = sumvar[2] = 0.0;
		count = 0;

		for(j=cubeVec[i].ini;j<=cubeVec[i].fim;j++)
		{
			sumvar[0] += colorVec[j].red;
			sumvar[1] += colorVec[j].green;
			sumvar[2] += colorVec[j].blue;
			count++;
		}

		pal[i].red = sumvar[0]/count;
		pal[i].green = sumvar[1]/count;
		pal[i].blue = sumvar[2]/count;
	}
}

void ordenaMaiorDim(colorCube* cubeVec, Color* colorVec, int i)
{
	size_t numElem;
	size_t tamElem;
	float var[3];

	numElem = (size_t)(cubeVec[i].fim - cubeVec[i].ini + 1);
	tamElem = (size_t)(sizeof(Color));

	var[0] = cubeVec[i].max.red - cubeVec[i].min.red;
	var[1] = cubeVec[i].max.green - cubeVec[i].min.green;
	var[2] = cubeVec[i].max.blue - cubeVec[i].min.blue;

	/* procura a maior dimensao em um cubo */
	if((var[0]>=var[1])&&(var[0]>=var[2])) cubeVec[i].maiorDim = 0;
	else
		if((var[1]>=var[0])&&(var[1]>=var[2])) cubeVec[i].maiorDim = 1;
		else
			if((var[2]>var[0]) && (var[2]>var[1])) cubeVec[i].maiorDim = 2;

	/* ordena de acordo com a maior dimensao da caixa */
	if(cubeVec[i].maiorDim == 0)
	{
		qsort(&colorVec[cubeVec[i].ini],numElem,tamElem,comparaR);
		cubeVec[i].var = var[0];
	}
	if(cubeVec[i].maiorDim == 1)
	{
		qsort(&colorVec[cubeVec[i].ini],numElem,tamElem,comparaG);
		cubeVec[i].var = var[1];
	}
	if(cubeVec[i].maiorDim == 2)
	{
		qsort(&colorVec[cubeVec[i].ini],numElem,tamElem,comparaB);
		cubeVec[i].var = var[2];
	}
}

void cortaCubo(colorCube* cubeVec, int posCorte, int numCubos)
{
	/* divide o cubo */
	int ini = cubeVec[posCorte].ini;
	int fim = cubeVec[posCorte].fim;

	if((fim - ini)%2 != 0) /* numero par de elementos */
	{
		cubeVec[numCubos].ini = ini + (fim - ini + 1)/2;
		cubeVec[numCubos].fim = fim;
		cubeVec[posCorte].fim = cubeVec[numCubos].ini - 1;
	}
	else /* numero impar de elementos */
	{
		cubeVec[numCubos].ini = ini + (fim - ini)/2;
		cubeVec[numCubos].fim = fim;
		cubeVec[posCorte].fim = cubeVec[numCubos].ini - 1;
	}
}

int cuboCorte(colorCube* cubeVec, int numCubos)
{
	/* escolhe o cubo a ser cortado */
	float maiorVar = -1;
	int posCorte = -1;
	int k;
	for(k=0;k<numCubos;k++)
	{
		/* cubo com uma unica cor */
		if((cubeVec[k].max.red == cubeVec[k].min.red)&&(cubeVec[k].max.green == cubeVec[k].min.green)&&(cubeVec[k].max.blue == cubeVec[k].min.blue))
			continue;

		if(cubeVec[k].var>maiorVar)
		{
			maiorVar = cubeVec[k].var;
			posCorte = k;
		}
	}

	return posCorte;
}

Image* imgReduceColors(Image * img0, int maxCores)
{
	int w = imgGetWidth(img0);
	int h = imgGetHeight(img0);
	Image* img1 = imgCreate(w,h);

	int x,y,i,j,numCubos = 0;
	int posCorte = -1;
	float rgb[3];

	colorCube* cubeVec = (colorCube*)malloc(maxCores*sizeof(colorCube)); /* vetor de cubos */
	Color* colorVec = (Color*)malloc(w*h*sizeof(Color)); /* vetor  de cores */
	Color* pal = (Color*)malloc(maxCores*sizeof(Color)); /* paleta de cores */

	/* guarda as cores nos vetores (com repeticao) */
	i = 0;
	for (y=0;y<h;y++){
		for (x=0;x<w;x++){
			imgGetPixel3fv(img0,x,y,rgb);
			colorVec[i].red = rgb[0];
			colorVec[i].green = rgb[1];
			colorVec[i].blue = rgb[2];
			i++;
		}
	}

	/* cria o cubo inicial */
	cubeVec[0].checked = 0;
	cubeVec[0].fim = w*h-1;
	cubeVec[0].ini = 0;

	numCubos = 1;

	for(j=0;j<maxCores-1;j++)
	{
		for(i=0;i<numCubos;i++)
		{
			if(cubeVec[i].checked) continue;

			/* calcula os maximos e os minimos */
			caixaEnvolvente(&cubeVec[i], colorVec);

			/* ordena de acordo com a maior dimensao */
			ordenaMaiorDim(cubeVec, colorVec, i);

			cubeVec[i].checked = 1;
		}

		/* escolhe o cubo a ser cortado */
		posCorte = cuboCorte(cubeVec, numCubos);

		if(posCorte == -1) break;

		/* divide o cubo */
		cortaCubo(cubeVec, posCorte, numCubos);

		cubeVec[numCubos].checked = 0;
		cubeVec[posCorte].checked = 0;

		numCubos++;
	}

	/* cria a paleta de cores */
	paleta(pal, cubeVec, colorVec, numCubos);

	/* preenche a imagem com as cores da paleta */
	bestColor(img0,&pal[0],img1,maxCores);

	free(colorVec);
	free(cubeVec);
	free(pal);

	printf("fim\n");

	return img1;
}

void imgSub(Image *img0, Image *img1)
{
	int w = imgGetWidth(img0);
	int h = imgGetHeight(img0);
	int x,y;
	float rgb0[3],rgb1[3];

	for (y=0;y<h;y++){
		for (x=0;x<w;x++) {
			imgGetPixel3fv(img0,x,y,rgb0);
			imgGetPixel3fv(img1,x,y,rgb1);
			rgb0[0]=(rgb1[0]>rgb0[0])? rgb1[0]-rgb0[0] : rgb0[0]-rgb1[0] ;
			rgb0[1]=(rgb1[1]>rgb0[1])? rgb1[1]-rgb0[1] : rgb0[1]-rgb1[1] ;
			rgb0[2]=(rgb1[2]>rgb0[2])? rgb1[2]-rgb0[2] : rgb0[2]-rgb1[2] ;
			imgSetPixel3fv(img0,x,y,rgb0);
		}
	}
}

Image * imgGaussFilter(Image * img0)
{
	int w = imgGetWidth (img0);
	int h = imgGetHeight(img0);
	Image * img1 = imgCreate(w,h);

	float rgb00[3], rgb01[3], rgb02[3];
	float rgb10[3], rgb11[3], rgb12[3];
	float rgb20[3], rgb21[3], rgb22[3];
	float rgb[3];

	int Gauss[3][3];

	int x, y;

	/* Filtro Gaussiano */
	Gauss[0][0] = 1; Gauss[0][1] = 2; Gauss[0][2] = 1;
	Gauss[1][0] = 2; Gauss[1][1] = 4; Gauss[1][2] = 2;
	Gauss[2][0] = 1; Gauss[2][1] = 2; Gauss[2][2] = 1;

	for (y=0;y<h;y++){
		for (x=0;x<w;x++) {

			rgb[0] = rgb[1] = rgb[2] = 0.0f;

			imgGetPixel3fv(img0,x,y,rgb11);
			rgb[0] += rgb11[0];
			rgb[1] += rgb11[1];
			rgb[2] += rgb11[2];

			if((x-1)>=0 && (y-1)>=0) imgGetPixel3fv(img0,x-1,y-1,rgb00);
			if((y-1)>=0) imgGetPixel3fv(img0,  x,y-1,rgb01);
			if((x+1)<w && (y-1)>=0) imgGetPixel3fv(img0,x+1,y-1,rgb02);
			if((x-1)>=0) imgGetPixel3fv(img0,x-1,  y,rgb10);
			if((x+1)<w) imgGetPixel3fv(img0,x+1,  y,rgb12);
			if((x-1)>=0 && (y+1)<h) imgGetPixel3fv(img0,x-1,y+1,rgb20);
			if((y+1)<h) imgGetPixel3fv(img0,  x,y+1,rgb21);
			if((x+1)<w && (y+1)<h) imgGetPixel3fv(img0,x+1,y+1,rgb22);

			/* tratando as bordas */
			if(((y == 0) && (x == 0))){

				rgb[0] = (rgb11[0]*Gauss[1][1] + rgb12[0]*Gauss[1][2] + rgb21[0]*Gauss[2][1] + rgb22[0]*Gauss[2][2])/16;
				rgb[1] = (rgb11[1]*Gauss[1][1] + rgb12[1]*Gauss[1][2] + rgb21[1]*Gauss[2][1] + rgb22[1]*Gauss[2][2])/16;
				rgb[2] = (rgb11[2]*Gauss[1][1] + rgb12[2]*Gauss[1][2] + rgb21[2]*Gauss[2][1] + rgb22[2]*Gauss[2][2])/16;
			}

			if(((x == (w-1)) && (y == 0))){

				rgb[0] = (rgb11[0]*Gauss[1][1] + rgb10[0]*Gauss[1][0] + rgb20[0]*Gauss[2][0] + rgb21[0]*Gauss[2][1])/16;
				rgb[1] = (rgb11[1]*Gauss[1][1] + rgb10[1]*Gauss[1][0] + rgb20[1]*Gauss[2][0] + rgb21[1]*Gauss[2][1])/16;
				rgb[2] = (rgb11[2]*Gauss[1][1] + rgb10[2]*Gauss[1][0] + rgb20[2]*Gauss[2][0] + rgb21[2]*Gauss[2][1])/16;
			}

			if(((x == (w-1)) && (y == (h-1)))){

				rgb[0] = (rgb11[0]*Gauss[1][1] + rgb00[0]*Gauss[0][0] + rgb01[0]*Gauss[0][1] + rgb10[0]*Gauss[1][0])/16;
				rgb[1] = (rgb11[1]*Gauss[1][1] + rgb00[1]*Gauss[0][0] + rgb01[1]*Gauss[0][1] + rgb10[1]*Gauss[1][0])/16;
				rgb[2] = (rgb11[2]*Gauss[1][1] + rgb00[2]*Gauss[0][0] + rgb01[2]*Gauss[0][1] + rgb10[2]*Gauss[1][0])/16;
			}

			if(((y == (h-1)) && (x == 0))){

				rgb[0] = (rgb11[0]*Gauss[1][1] + rgb12[0]*Gauss[1][2] + rgb01[0]*Gauss[0][1] + rgb02[0]*Gauss[0][2])/16;
				rgb[1] = (rgb11[1]*Gauss[1][1] + rgb12[1]*Gauss[1][2] + rgb01[1]*Gauss[0][1] + rgb02[1]*Gauss[0][2])/16;
				rgb[2] = (rgb11[2]*Gauss[1][1] + rgb12[2]*Gauss[1][2] + rgb01[2]*Gauss[0][1] + rgb02[2]*Gauss[0][2])/16;
			}

			if(((x == (w-1)) && (y>0) && (y<(h-1)))){

				rgb[0] = (rgb11[0]*Gauss[1][1] + rgb00[0]*Gauss[0][0] + rgb01[0]*Gauss[0][1] + rgb10[0]*Gauss[1][0] + rgb21[0]*Gauss[2][1] + rgb20[0]*Gauss[2][0])/16;
				rgb[1] = (rgb11[1]*Gauss[1][1] + rgb00[1]*Gauss[0][0] + rgb01[1]*Gauss[0][1] + rgb10[1]*Gauss[1][0] + rgb21[1]*Gauss[2][1] + rgb20[1]*Gauss[2][0])/16;
				rgb[2] = (rgb11[2]*Gauss[1][1] + rgb00[2]*Gauss[0][0] + rgb01[2]*Gauss[0][1] + rgb10[2]*Gauss[1][0] + rgb21[2]*Gauss[2][1] + rgb20[2]*Gauss[2][0])/16;
			}

			if(((x == 0) && (y > 0) && (y < (h-1)))){

				rgb[0] = (rgb11[0]*Gauss[1][1] + rgb01[0]*Gauss[0][1] + rgb02[0]*Gauss[0][2] + rgb12[0]*Gauss[1][2] + rgb21[0]*Gauss[2][1] + rgb22[0]*Gauss[2][2])/16;
				rgb[1] = (rgb11[1]*Gauss[1][1] + rgb01[1]*Gauss[0][1] + rgb02[1]*Gauss[0][2] + rgb12[1]*Gauss[1][2] + rgb21[1]*Gauss[2][1] + rgb22[1]*Gauss[2][2])/16;
				rgb[2] = (rgb11[2]*Gauss[1][1] + rgb01[2]*Gauss[0][1] + rgb02[2]*Gauss[0][2] + rgb12[2]*Gauss[1][2] + rgb21[2]*Gauss[2][1] + rgb22[2]*Gauss[2][2])/16;
			}

			if(((y == 0) && (x > 0) && (x < (w-1)))){

				rgb[0] = (rgb11[0]*Gauss[1][1] + rgb10[0]*Gauss[1][0] + rgb12[0]*Gauss[1][2] + rgb20[0]*Gauss[2][0] + rgb21[0]*Gauss[2][1] + rgb22[0]*Gauss[2][2])/16;
				rgb[1] = (rgb11[1]*Gauss[1][1] + rgb10[1]*Gauss[1][0] + rgb12[1]*Gauss[1][2] + rgb20[1]*Gauss[2][0] + rgb21[1]*Gauss[2][1] + rgb22[1]*Gauss[2][2])/16;
				rgb[2] = (rgb11[2]*Gauss[1][1] + rgb10[2]*Gauss[1][0] + rgb12[2]*Gauss[1][2] + rgb20[2]*Gauss[2][0] + rgb21[2]*Gauss[2][1] + rgb22[2]*Gauss[2][2])/16;
			}

			if(((y == (h-1)) && (x > 0) && (x < (w-1)))){

				rgb[0] = (rgb11[0]*Gauss[1][1] + rgb00[0]*Gauss[0][0] + rgb01[0]*Gauss[0][1] + rgb02[0]*Gauss[0][2] + rgb10[0]*Gauss[1][0] + rgb12[0]*Gauss[1][2])/16;
				rgb[1] = (rgb11[1]*Gauss[1][1] + rgb00[1]*Gauss[0][0] + rgb01[1]*Gauss[0][1] + rgb02[1]*Gauss[0][2] + rgb10[1]*Gauss[1][0] + rgb12[1]*Gauss[1][2])/16;
				rgb[2] = (rgb11[2]*Gauss[1][1] + rgb00[2]*Gauss[0][0] + rgb01[2]*Gauss[0][1] + rgb02[2]*Gauss[0][2] + rgb10[2]*Gauss[1][0] + rgb12[2]*Gauss[1][2])/16;
			}

			/* tratando os pixels centrais */
			if( ((x!=0) && (y!=0) && (y!=(h-1)) && (x!=(w-1)))){

				rgb[0] = (rgb00[0]*Gauss[0][0] + rgb01[0]*Gauss[0][1] + rgb02[0]*Gauss[0][2] + 
					rgb10[0]*Gauss[1][0] + rgb11[0]*Gauss[1][1] + rgb12[0]*Gauss[1][2] + 
					rgb20[0]*Gauss[2][0] + rgb21[0]*Gauss[2][1] + rgb22[0]*Gauss[2][2])/16;

				rgb[1] = (rgb00[1]*Gauss[0][0] + rgb01[1]*Gauss[0][1] + rgb02[1]*Gauss[0][2] + 
					rgb10[1]*Gauss[1][0] + rgb11[1]*Gauss[1][1] + rgb12[1]*Gauss[1][2] + 
					rgb20[1]*Gauss[2][0] + rgb21[1]*Gauss[2][1] + rgb22[1]*Gauss[2][2])/16;

				rgb[2] = (rgb00[2]*Gauss[0][0] + rgb01[2]*Gauss[0][1] + rgb02[2]*Gauss[0][2] + 
					rgb10[2]*Gauss[1][0] + rgb11[2]*Gauss[1][1] + rgb12[2]*Gauss[1][2] + 
					rgb20[2]*Gauss[2][0] + rgb21[2]*Gauss[2][1] + rgb22[2]*Gauss[2][2])/16;
			}

			if(rgb[0]>1) rgb[0] = 1;
			if(rgb[1]>1) rgb[1] = 1;
			if(rgb[2]>1) rgb[2] = 1;

			imgSetPixel3fv(img1,x,y,rgb);
		}
	}

	return img1;
}

Image * imgLaplcFilter(Image * img)
{
	int w = imgGetWidth (img);
	int h = imgGetHeight(img);
	Image * img1 = imgCreate(w,h);
	Image * img0 = imgCreate(w,h);

	float rgb00[3], rgb01[3], rgb02[3];
	float rgb10[3], rgb11[3], rgb12[3];
	float rgb20[3], rgb21[3], rgb22[3];
	float rgb[3];

	int Laplc[3][3];

	int x,y;

	img0 = imgGrey(img);

	/* Filtro Laplaciano */
	Laplc[0][0] = -1; Laplc[0][1] = -1; Laplc[0][2] = -1;
	Laplc[1][0] = -1; Laplc[1][1] =  8; Laplc[1][2] = -1;
	Laplc[2][0] = -1; Laplc[2][1] = -1; Laplc[2][2] = -1;

	for (y=0;y<h;y++){
		for (x=0;x<w;x++) {

			rgb[0] = rgb[1] = rgb[2] = 0.0f;

			imgGetPixel3fv(img0,x,y,rgb11);
			rgb[0] += rgb11[0]*Laplc[1][1];
			rgb[1] += rgb11[1]*Laplc[1][1];
			rgb[2] += rgb11[2]*Laplc[1][1];

			if((y-1)>=0)
			{
				imgGetPixel3fv(img0,x,y-1,rgb01);
				rgb[0] += rgb01[0]*Laplc[0][1];
				rgb[1] += rgb01[1]*Laplc[0][1];
				rgb[2] += rgb01[2]*Laplc[0][1];
			} 

			if((x-1)>=0)
			{
				imgGetPixel3fv(img0,x-1,y,rgb10);
				rgb[0] += rgb10[0]*Laplc[1][0];
				rgb[1] += rgb10[1]*Laplc[1][0];
				rgb[2] += rgb10[2]*Laplc[1][0];

				if((y-1)>=0)
				{
					imgGetPixel3fv(img0,x-1,y-1,rgb00);
					rgb[0] += rgb00[0]*Laplc[0][0];
					rgb[1] += rgb00[1]*Laplc[0][0];
					rgb[2] += rgb00[2]*Laplc[0][0];
				}

				if((y+1)<h)
				{
					imgGetPixel3fv(img0,x-1,y+1,rgb20);
					rgb[0] += rgb20[0]*Laplc[2][0];
					rgb[1] += rgb20[1]*Laplc[2][0];
					rgb[2] += rgb20[2]*Laplc[2][0];
				}
			}

			if((x+1)<w)
			{
				imgGetPixel3fv(img0,x+1,y,rgb12);
				rgb[0] += rgb12[0]*Laplc[1][2];
				rgb[1] += rgb12[1]*Laplc[1][2];
				rgb[2] += rgb12[2]*Laplc[1][2];

				if((y-1)>=0)
				{
					imgGetPixel3fv(img0,x+1,y-1,rgb02);
					rgb[0] += rgb02[0]*Laplc[0][2];
					rgb[1] += rgb02[1]*Laplc[0][2];
					rgb[2] += rgb02[2]*Laplc[0][2];
				} 

				if((y+1)<h)
				{
					imgGetPixel3fv(img0,x+1,y+1,rgb22);
					rgb[0] += rgb22[0]*Laplc[2][2];
					rgb[1] += rgb22[1]*Laplc[2][2];
					rgb[2] += rgb22[2]*Laplc[2][2];
				}
			}

			if((y+1)<h)
			{
				imgGetPixel3fv(img0,x,y+1,rgb21);
				rgb[0] += rgb21[0]*Laplc[2][1];
				rgb[1] += rgb21[1]*Laplc[2][1];
				rgb[2] += rgb21[2]*Laplc[2][1];
			}

			if(rgb[0]>1) rgb[0] = 1;
			if(rgb[1]>1) rgb[1] = 1;
			if(rgb[2]>1) rgb[2] = 1;

			if(rgb[0]<0) rgb[0] = 0;
			if(rgb[1]<0) rgb[1] = 0;
			if(rgb[2]<0) rgb[2] = 0;

			imgSetPixel3fv(img1,x,y,rgb);
		}
	}

	return img1;
}

Image * imgEdges(Image * img0)
{
	int w = imgGetWidth (img0);
	int h = imgGetHeight(img0);
	Image * imgLoG   = imgCreate(w,h);
	float rgb1[3];
	float rgb[3];

	int x, y;

	imgLoG = imgGaussFilter(img0);
	imgLoG   = imgLaplcFilter(imgLoG);

	for (y=0;y<h;y++){
		for (x=0;x<w;x++) {

			imgGetPixel3fv(imgLoG,x,y,rgb);

			rgb1[0] = 1 - rgb[0];
			rgb1[1] = 1 - rgb[1];
			rgb1[2] = 1 - rgb[2];

			imgSetPixel3fv(imgLoG,x,y,rgb1);
		}
	}

	return imgLoG;
}

Image * imgFiltroDeMediana(Image * img0)
{
	int w = imgGetWidth (img0);
	int h = imgGetHeight(img0);
	Image * img1 = imgCreate(w,h);

	int x,y,k;
	int mediana;

	float rgb00[3], rgb01[3], rgb02[3];
	float rgb10[3], rgb11[3], rgb12[3];
	float rgb20[3], rgb21[3], rgb22[3];
	float vet0[9], vet1[9], vet2[9];
	float rgb[3];

	for (y=0;y<h;y++){
		for (x=0;x<w;x++) {

			k = 0;

			imgGetPixel3fv(img0,x,y,rgb11);
			vet0[k] = rgb11[0];
			vet1[k] = rgb11[1];
			vet2[k] = rgb11[2];
			k++;

			if((y-1)>=0)
			{
				imgGetPixel3fv(img0,x,y-1,rgb01);
				vet0[k] = rgb01[0];
				vet1[k] = rgb01[1];
				vet2[k] = rgb01[2];	
				k++;
			} 

			if((x-1)>=0)
			{
				imgGetPixel3fv(img0,x-1,y,rgb10);
				vet0[k] = rgb10[0];
				vet1[k] = rgb10[1];
				vet2[k] = rgb10[2];
				k++;

				if((y-1)>=0)
				{
					imgGetPixel3fv(img0,x-1,y-1,rgb00);
					vet0[k] = rgb00[0];
					vet1[k] = rgb00[1];
					vet2[k] = rgb00[2];
					k++;
				}

				if((y+1)<h)
				{
					imgGetPixel3fv(img0,x-1,y+1,rgb20);
					vet0[k] = rgb20[0];
					vet1[k] = rgb20[1];
					vet2[k] = rgb20[2];
					k++;
				}
			}

			if((x+1)<w)
			{
				imgGetPixel3fv(img0,x+1,y,rgb12);
				vet0[k] = rgb12[0];
				vet1[k] = rgb12[1];
				vet2[k] = rgb12[2];
				k++;

				if((y-1)>=0)
				{
					imgGetPixel3fv(img0,x+1,y-1,rgb02);
					vet0[k] = rgb02[0];
					vet1[k] = rgb02[1];
					vet2[k] = rgb02[2];		
					k++;
				} 

				if((y+1)<h)
				{
					imgGetPixel3fv(img0,x+1,y+1,rgb22);
					vet0[k] = rgb22[0];
					vet1[k] = rgb22[1];
					vet2[k] = rgb22[2];
					k++;
				}
			}

			if((y+1)<h)
			{
				imgGetPixel3fv(img0,x,y+1,rgb21);
				vet0[k] = rgb21[0];
				vet1[k] = rgb21[1];
				vet2[k] = rgb21[2];
				k++;
			}

			qsort(vet0,k,sizeof(float),comparaCor);
			qsort(vet1,k,sizeof(float),comparaCor);
			qsort(vet2,k,sizeof(float),comparaCor);

			if(k%2==0)mediana = k/2;
			else mediana = (k+1)/2;

			rgb[0] = vet0[mediana];
			rgb[1] = vet1[mediana];
			rgb[2] = vet2[mediana];

			if(rgb[0]>1) rgb[0] = 1;
			if(rgb[1]>1) rgb[1] = 1;
			if(rgb[2]>1) rgb[2] = 1;

			imgSetPixel3fv(img1,x,y,rgb);
		}
	}

	return img1;
}

Image * imgBinarizacao(Image * img0)
{
	int w = imgGetWidth (img0);
	int h = imgGetHeight(img0);
	Image * imgB = imgCreate(w,h);
	float rgb[3], rgbB[3];
	float sumR = 0;
	float sumG = 0;
	float sumB = 0;

	int x, y;

	imgB = imgGrey(img0);

	for (y=0;y<h;y++){
		for (x=0;x<w;x++) {

			imgGetPixel3fv(imgB,x,y,rgb);

			sumR += rgb[0];
			sumG += rgb[0];
			sumB += rgb[0];
		}
	}

	for (y=0;y<h;y++){
		for (x=0;x<w;x++) {

			imgGetPixel3fv(imgB,x,y,rgb);

			if(rgb[0] > (sumR / (w*h))) rgbB[0] = 1;
			else rgbB[0] = 0;
			if(rgb[1] > (sumG / (w*h))) rgbB[1] = 1;
			else rgbB[1] = 0;
			if(rgb[2] > (sumB / (w*h))) rgbB[2] = 1;
			else rgbB[2] = 0;

			imgSetPixel3fv(imgB,x,y,rgbB);
		}
	}

	return imgB;
}

