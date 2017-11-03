#ifndef __Std__
#define __Std__

// #include for I/O
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#endif

// #include pour crypto
#include "head/Crypto.h"

// #include pour le png
#include "head/Png.h"



void main(int argc, char *argv[]){

	// file
	char *filename = "test.txt";

	// key & iv
	unsigned char key[32], iv[16];
	if (!RAND_bytes(key, sizeof key)) handleErrorsCrypto("RAND_bytes");
	if (!RAND_bytes(iv, sizeof iv)) handleErrorsCrypto("RAND_bytes");
	
	// PlainText
	int plaintext_len = getFileSize(filename);
	unsigned char *plaintext = malloc(plaintext_len);
	

	// Cypher
	int cyphertext_len = 0;
	unsigned char *cyphertext = malloc(plaintext_len*10);
	


	cyphertext_len = chiffrerUnFichier(filename, key, iv, plaintext, plaintext_len, cyphertext);
	//printf("\n%s\n",cyphertext);


	plaintext_len = dechiffrer(key, iv, plaintext, cyphertext, cyphertext_len);

	printf("\n%s\n",plaintext);


	PNG png = getPNG("Test.png");


	// fabrication des tableau de masques
	MASQUE *tab_masques = malloc(cyphertext_len * sizeof(MASQUE));

	// USINE A MASQUE binaire
	int i;
	for(i = 0; i < cyphertext_len; i++){
		tab_masques[i].duo_premier = cyphertext[i] & 0b11000000;
		tab_masques[i].duo_premier = tab_masques[i].duo_premier >> 6;

		tab_masques[i].duo_second = cyphertext[i] & 0b00110000;
		tab_masques[i].duo_second = tab_masques[i].duo_second >> 4;

		tab_masques[i].duo_troisieme = cyphertext[i] & 0b00001100;
		tab_masques[i].duo_troisieme = tab_masques[i].duo_troisieme >> 2;

		tab_masques[i].duo_dernier = cyphertext[i] & 0b00000011;
	}


	LSB(png, tab_masques, cyphertext_len);
	setPNG(png);


	// On prend l'image out et on déchiffre
	PNG png_out = getPNG("out.png");

	findCypher(png_out, cyphertext, cyphertext_len);

	dechiffrer(key, iv, plaintext, cyphertext, cyphertext_len);
	printf("\n\n%s\n",plaintext);



	// FREE
	free(tab_masques);

	for(i = 0; i < png_out.height; i++) {
		free(png_out.row_pointers[i]);
	}
	free(png_out.row_pointers);


	for(i = 0; i < png.height; i++) {
		free(png.row_pointers[i]);
	}
	free(png.row_pointers);


	free(plaintext);
	free(cyphertext);

}