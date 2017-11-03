#ifndef __Std__
#define __Std__

// #include for I/O
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#endif

#ifndef __PNG__
#define __PNG__

// #include for PNG & CRC
#include <zlib.h>
#include <png.h>
#include <pngconf.h>

#endif



/**
*	Structure MASQUE :
*		unsigned char duo_premier : les 2 premiers bits d'un octet de chiffrer -> 0b11000000
*		unsigned char duo_second : les 2 second bits d'un octet de chiffrer -> 0b00110000
*		unsigned char duo_troisieme : les 2 troisieme bits d'un octet de chiffrer -> 0b00001100
*		unsigned char duo_dernier : les 2 dernier bits d'un octet de chiffrer -> 0b00000011
*/
typedef struct MASQUE {

	unsigned char duo_premier;
	unsigned char duo_second;
	unsigned char duo_troisieme;
	unsigned char duo_dernier;

} MASQUE;

/**
*	Structure PNG :
*		char *filename : nom du fichier .png
*		int width : longueur du fichier horizontale
*		int height : largeur du fichier verticale
*		png_byte color_type : taille de la couleur en nombre de bits
*		png_byte bit_depth : profondeur en bits
*		png_bytep *row_pointers : pointeur sur les lignes du png
*/
typedef struct PNG
{
	char *filename;
	int width;
	int height;
	png_byte color_type;
	png_byte bit_depth;
	png_bytep *row_pointers;
} PNG;


/**
*	PNG getPNG(char *filename)
*		fonction qui ouvre le png spécifié par filename et met les informations dans une structure PNG
*		return PNG : renvoi une structure PNG contenant les informations sur le png
*		char *filename : nom du fichier png a ouvrir
*/
PNG getPNG(char *filename);


/**
*	void setPNG(PNG png)
*		fonctions qui va créer un nouveau fichier png avec comme valeur ce qui'il y a dans la structure png qu'on lui envoi
*		PNG png : Structure png qui va contenir les info a écrire dans le png
*/
void setPNG(PNG png);


/**
*	void LSB(PNG png, MASQUE *tab, int cypher_len);
*		fonctions qui va appliquer le LSB sur le png spécifié par PNG avec le chiffré spécifié dans le MASQUE
*		PNG png : Structure png qui va contenir les info du png
*		MASQUE *tab : Masque binaire représentant le chiffré qui doit être appliqué sur le RVBA su png
*		int cypher_len : taille du chiffré
*/
void LSB(PNG png, MASQUE *tab, int cypher_len);


/**
*	void findCypher(PNG png, MASQUE *tab, unsigned char *cyphertext, int cyphertext_len);
*		fonction qui va retrouver le chiffré dans une image spécifié par PNG
*		PNG png : structure PNG qui définit le fichier png
*		unsigned char *cyphertext : pointeur sur le chiffré
*		int cyphertext_len : taille du chiffré
*/
void findCypher(PNG png, unsigned char *cyphertext, int cyphertext_len);


/**
*	void handleErrorsPNG(char *fn, int rorw, FILE *file, png_structp png, png_infop info);
*		fonction gestionnaire d'erreur
*		char *fn : fonction qui bug
*		int rorw : lecture ou ecriture du png
*		FILE *file : fichier a fermer en cas de bug
*		png_structp png : structure png a fermer en cas de bug
*		png_infop info : structure info a fermer en cas de bug
*/
void handleErrorsPNG(char *fn, int rorw, FILE *file, png_structp png, png_infop info);