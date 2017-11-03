#ifndef __Std__
#define __Std__

// #include for I/O
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#endif

#ifndef __Crypto__
#define __Crypto__

// #include for Openssl crypto
#include <openssl/aes.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#endif



/**
*	int chiffrer (unsigned char *cKey, unsigned char *ivec, unsigned char *plainText, int plainText_len, unsigned char *cypherText);
*		fonction qui chiffre le plaintext en aes_256_cbc
*		return int cyphertext_len : la taille du chiffrer
*		unsigned char *cKey : la clé secrete
*		unsigned char *ivec : vecteur d'initialisation
*		unsigned char *plainText : pointeur sur le texte plain
*		int plainText_len : taille du plain text
*		unsigned char *cypherText : pointeur sur le chiffré
*/
int chiffrer (unsigned char *cKey, unsigned char *ivec, unsigned char *plainText, int plainText_len, unsigned char *cypherText);


/**
*	int chiffrerUnFichier(char *filename, unsigned char *cKey, unsigned char *ivec, unsigned char *plainText, int plainText_len, unsigned char *cypherText);
*		fonction qui chiffre le plaintext obtenue d'un fichier en aes_256_cbc
*		return int cyphertext_len : la taille du chiffrer
*		char *filename : fichier qui contient le plain text
*		unsigned char *cKey : la clé secrete
*		unsigned char *ivec : vecteur d'initialisation
*		unsigned char *plainText : pointeur sur le texte plain
*		int plainText_len : taille du plain text
*		unsigned char *cypherText : pointeur sur le chiffré
*/
int chiffrerUnFichier(char *filename, unsigned char *cKey, unsigned char *ivec, unsigned char *plainText, int plainText_len, unsigned char *cypherText);




int dechiffrer (unsigned char *cKey, unsigned char *ivec, unsigned char *plainText, unsigned char *cypherText, int cipherText_len);

int dechiffrerUnFichier(char *filename, unsigned char *cKey, unsigned char *ivec, unsigned char *plainText, unsigned char *cypherText, int cipherText_len);



void handleErrorsCrypto(char *fn);
int getFileSize(char *filename);