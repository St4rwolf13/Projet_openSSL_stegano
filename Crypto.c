#ifndef __CryptoH__
#define __CryptoH__

#include "head/Crypto.h"

#endif

int chiffrer (unsigned char *cKey, unsigned char *ivec, unsigned char *plainText, int plainText_len, unsigned char *cypherText){

	int cypherText_len;
	int len;

	// Set up encryption CTX Structure
	EVP_CIPHER_CTX *ctx;
	if(!(ctx = EVP_CIPHER_CTX_new()))
		handleErrorsCrypto("EVP_CIPHER_CTX");

	// CHIFFREMENT

	if(1 != (EVP_EncryptInit(ctx, EVP_aes_256_cbc(), cKey, ivec)))
		handleErrorsCrypto("EVP_EncryptInit");

	if(1 != EVP_EncryptUpdate(ctx, cypherText, &len, plainText, plainText_len))
		handleErrorsCrypto("EVP_EncryptUpdate");
	cypherText_len = len;

	if(1 != EVP_EncryptFinal(ctx, cypherText + len, &len))
		handleErrorsCrypto("EVP_EncryptFinal");
	cypherText_len += len;

	// CHIFFREMENT FINI

	EVP_CIPHER_CTX_free(ctx);

	return cypherText_len;
}

int chiffrerUnFichier(char *filename, unsigned char *cKey, unsigned char *ivec, unsigned char *plainText, int plainText_len, unsigned char *cypherText){

	FILE *inFile = fopen(filename, "r+");

	// Get file size
	fseek(inFile, 0L, SEEK_END);
	int fsize = ftell(inFile);
	//set curseur back to normal
	fseek(inFile, 0L, SEEK_SET);

	// Lecture du fichier
	int nmb = fread(plainText, sizeof(char), plainText_len, inFile);
	fclose(inFile);


	return chiffrer (cKey, ivec, plainText, plainText_len, cypherText);

}


int dechiffrer (unsigned char *cKey, unsigned char *ivec, unsigned char *plainText, unsigned char *cypherText, int cypherText_len){

	int plainText_len;
	int len;

	// Set up encryption CTX Structure
	EVP_CIPHER_CTX *ctx;
	if(!(ctx = EVP_CIPHER_CTX_new())) 
		handleErrorsCrypto("EVP_CIPHER_CTX");

	// DECHIFFREMENT

	if(1 != EVP_DecryptInit(ctx, EVP_aes_256_cbc(), cKey, ivec))
		handleErrorsCrypto("EVP_DecryptInit");

	if(1 != EVP_DecryptUpdate(ctx, plainText, &len, cypherText, cypherText_len))
		handleErrorsCrypto("EVP_DecryptUpdate");
	plainText_len = len;


	if(1 != EVP_DecryptFinal(ctx, plainText + len, &len))
		handleErrorsCrypto("EVP_DecryptFinal");
	plainText_len += len;

	// DECHIFFREMENT FINI

	EVP_CIPHER_CTX_free(ctx);

	return plainText_len;

}


int dechiffrerUnFichier(char *filename, unsigned char *cKey, unsigned char *ivec, unsigned char *plainText, unsigned char *cypherText, int cypherText_len){

	FILE *outFile = fopen(filename, "r+");

	// Lecture du fichier
	int nmb;
	nmb = fread(cypherText, sizeof(char), cypherText_len, outFile);
	fclose(outFile);

	return dechiffrer (cKey, ivec, plainText, cypherText, cypherText_len);

}

void handleErrorsCrypto(char *fn){
	printf("ERROR AT FONCTION = %s\n\n", fn);
	ERR_print_errors_fp(stderr);
	abort();
}

int getFileSize(char *filename){
	FILE *inFile = fopen(filename, "r+");
	
	// Get file size
	fseek(inFile, 0L, SEEK_END);
	int fsize = ftell(inFile);
	//set curseur back to normal
	fseek(inFile, 0L, SEEK_SET);

	fclose(inFile);

	return fsize;
}