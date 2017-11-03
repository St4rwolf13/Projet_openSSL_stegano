// #include for I/O
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// #include for Openssl crypto
#include <openssl/aes.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>

// #include for PNG & CRC
#include <zlib.h>
#include <png.h>
#include <pngconf.h>



// Fonctions de Crypto
int chiffrer (unsigned char *cKey, unsigned char *ivec, unsigned char *plainText, int plainText_len, unsigned char *cypherText);
int dechiffrer (unsigned char *cKey, unsigned char *ivec, unsigned char *plainText, unsigned char *cypherText, int cipherText_len);
void handleErrorsCrypto(int line);

// Fonctions de PNG
void workonPNG (char *filename);
void handleErrorsPNG(int line, int rorw, FILE *file, png_structp png, png_infop info);


void main(int argc, char *argv[])
{
	/* code */

	/* //////////////////////////// SETUP FOR CRYPTO ////////////////////////////*/

	// File_init
	char *inName;
	if (argc < 2) inName = "test.txt";
	else inName = argv[1];
	char *outName = "out.txt";

	FILE *inFile = fopen(inName, "r+");
	FILE *outFile = fopen(outName, "w+");

	// Get file size
	fseek(inFile, 0L, SEEK_END);
	int fsize = ftell(inFile);
	//set curseur back to normal
	fseek(inFile, 0L, SEEK_SET);
	printf("%d\n",fsize );
	// Data_init
	unsigned char *plainData = malloc(fsize);
	unsigned char *cypherData = malloc(fsize*8);

	int plainData_len = fsize;
	int cypherData_len = 0;

	// Key_init 256 bits & Iv_init 128 bits
	unsigned char key[32], iv[16];

	if (!RAND_bytes(key, sizeof key)) handleErrorsCrypto(46);
	if (!RAND_bytes(iv, sizeof iv)) handleErrorsCrypto(47);
	
	/* save la clé
	FILE *keyfile = fopen("key.txt", "w+");
	fwrite(key, sizeof(unsigned char), 32, keyfile);
	*/

	/* //////////////////////////// CHIFFRER ////////////////////////////*/

	// Lecture du fichier
	int nmb = fread(plainData, sizeof(char), plainData_len, inFile);

	cypherData_len = chiffrer (key, iv, plainData, plainData_len, cypherData);

	// write the cypher
	fwrite(cypherData, sizeof(char), cypherData_len, outFile);
	printf("%s",cypherData);


	/* //////////////////////////// DECHIFFRER ////////////////////////////*/

	// Lecture du fichier
	nmb = fread(cypherData, sizeof(char), cypherData_len, outFile);

	//unsigned char *newData = malloc(fsize*100);
	dechiffrer (key, iv, plainData, cypherData, cypherData_len);

	// Print the plaintext
	printf("%s",plainData);
	printf("\n\n");

	/* //////////////////////////// PARSE DATA LSB ////////////////////////////*/

	
	/* //////////////////////////// WORK ON PNG ////////////////////////////*/

	//workonPNG("demo");


	/* //////////////////////////// CLOSE & FREE ////////////////////////////*/

	free(plainData);
	free(cypherData);


	fclose(inFile);
	fclose(outFile);

}






int chiffrer (unsigned char *cKey, unsigned char *ivec, unsigned char *plainText, int plainText_len, unsigned char *cypherText){

	int cipherText_len;
	int len;

	// Set up encryption CTX Structure
	EVP_CIPHER_CTX *ctx;
	if(!(ctx = EVP_CIPHER_CTX_new()))
		handleErrorsCrypto(124);

	// CHIFFREMENT

	if(1 != (EVP_EncryptInit(ctx, EVP_aes_256_cbc(), cKey, ivec)))
		handleErrorsCrypto(129);

	if(1 != EVP_EncryptUpdate(ctx, cypherText, &len, plainText, plainText_len))
		handleErrorsCrypto(132);
	cipherText_len = len;

	if(1 != EVP_EncryptFinal(ctx, cypherText + len, &len))
		handleErrorsCrypto(136);
	cipherText_len += len;

	// CHIFFREMENT FINI

	EVP_CIPHER_CTX_free(ctx);

	return cipherText_len;
}




int dechiffrer (unsigned char *cKey, unsigned char *ivec, unsigned char *plainText, unsigned char *cypherText, int cipherText_len){

	int plainText_len;
	int len;

	// Set up encryption CTX Structure
	EVP_CIPHER_CTX *ctx;
	if(!(ctx = EVP_CIPHER_CTX_new())) 
		handleErrorsCrypto(121);

	// DECHIFFREMENT

	if(1 != EVP_DecryptInit(ctx, EVP_aes_256_cbc(), cKey, ivec))
		handleErrorsCrypto(126);

	if(1 != EVP_DecryptUpdate(ctx, plainText, &len, cypherText, cipherText_len))
		handleErrorsCrypto(129);
	plainText_len = len;

	if(1 != EVP_DecryptFinal(ctx, plainText + len, &len))
		handleErrorsCrypto(133);
	plainText_len += len;

	// DECHIFFREMENT FINI

	EVP_CIPHER_CTX_free(ctx);

	return plainText_len;

}


void handleErrorsCrypto(int line){
	printf("Line = %d\n", line);
	ERR_print_errors_fp(stderr);
	abort();
}



void workonPNG(char *filename){

	/* //////////////////////////// READ PNG ////////////////////////////*/

	FILE *pngFile = fopen(filename, "rb");
	if (pngFile == NULL) perror("fopen");

	// ON cree la structure qui va contenir le PNG
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png_ptr) handleErrorsPNG(198, 0, pngFile, NULL, NULL);

	// On cree les strcture d'informations sr le PNG
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr) handleErrorsPNG(202, 0, pngFile, png_ptr, NULL);

	if(setjmp(png_jmpbuf(png_ptr))) handleErrorsPNG(204, 0, pngFile, png_ptr, info_ptr);

	// on associe le ptr du PNG au Fichier.PNG
	png_init_io(png_ptr, pngFile);

	// les information sont stocké dans le ptr info
	png_read_info(png_ptr, info_ptr);

	// on obtient les infos
	int png_width = png_get_image_width(png_ptr, info_ptr);
	int png_height = png_get_image_height(png_ptr, info_ptr);
	png_byte color_type = png_get_color_type(png_ptr, info_ptr);
	png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);


	// Read any color_type into 8bit depth, RGBA format.
	// See http://www.libpng.org/pub/png/libpng-manual.txt

	if(bit_depth == 16)
		png_set_strip_16(png_ptr);

	if(color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);

	// PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
	if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png_ptr);

	if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);

	// These color_type don't have an alpha channel then fill it with 0xff.
	if(color_type == PNG_COLOR_TYPE_RGB ||
	   color_type == PNG_COLOR_TYPE_GRAY ||
	   color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);

	if(color_type == PNG_COLOR_TYPE_GRAY ||
	   color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png_ptr);

	png_read_update_info(png_ptr, info_ptr);

	// on créer un pointeur sur les lignes du png
	png_bytep *row_pointers;
	row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * png_height);
	int y, x;
	for(y = 0; y < png_height; y++) {
		row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png_ptr,info_ptr));
	}
	// on stocke les ligne du png dans row_pointers
	png_read_image(png_ptr, row_pointers);

	for(y = 0; y < png_height; y++) {
		png_bytep row = row_pointers[y];
		for(x = 0; x < png_width; x++) {
			png_bytep px = &(row[x * 4]);
			// Do something awesome for each pixel here...
			//printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);

		}
	}

	fclose(pngFile);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	
	/* //////////////////////////// WRITE PNG ////////////////////////////*/

	pngFile = fopen("Test.png", "wb");
	if(!pngFile) abort();

	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png) handleErrorsPNG(274, 1, pngFile, NULL, NULL);

	png_infop info = png_create_info_struct(png);
	if (!info) handleErrorsPNG(277, 1, pngFile, png, NULL);

	if (setjmp(png_jmpbuf(png))) handleErrorsPNG(274, 1, pngFile, png, info);

	png_init_io(png, pngFile);

	// Output is 8bit depth, RGBA format.
	png_set_IHDR(
				png,
				info,
				png_width, png_height,
				8,
				PNG_COLOR_TYPE_RGBA,
				PNG_INTERLACE_NONE,
				PNG_COMPRESSION_TYPE_DEFAULT,
				PNG_FILTER_TYPE_DEFAULT
				);
	png_write_info(png, info);

	// To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
	// Use png_set_filler().
	//png_set_filler(png, 0, PNG_FILLER_AFTER);

	png_write_image(png, row_pointers);
	png_write_end(png, NULL);



	// FREE ALL


	for(y = 0; y < png_height; y++) {
		free(row_pointers[y]);
	}
	free(row_pointers);
	fclose(pngFile);
	png_destroy_write_struct(&png, &info);
}

void handleErrorsPNG(char * line, int rorw, FILE *file, png_structp png, png_infop info){
	
	if (rorw = 0) {
		if (png && info){
			png_destroy_read_struct(&png, &info, NULL);
		} else if (png && !info){
			png_destroy_read_struct(&png, NULL, NULL);
		}
		fclose(file);
		printf("ERREUR ligne %d\n", line);
		abort();
	} else if(rorw = 1) {
		if (png && info){
			png_destroy_write_struct(&png, &info);
		} else if (png && !info){
			png_destroy_write_struct(&png, NULL);
		}
		fclose(file);
		printf("ERREUR ligne %d\n", line);
		abort();
	}


}