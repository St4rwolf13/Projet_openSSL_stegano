#ifndef __PNGH__
#define __PNGH__

#include "head/Png.h"

#endif

PNG getPNG(char *filename){

	/* //////////////////////////// READ PNG ////////////////////////////*/

	PNG png;

	FILE *pngFile = fopen(filename, "rb");
	if (pngFile == NULL) perror("fopen");
	png.filename = filename;

	// ON cree la structure qui va contenir le PNG
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png_ptr) handleErrorsPNG("png_create_read_struct", 0, pngFile, NULL, NULL);

	// On cree les strcture d'informations sr le PNG
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr) handleErrorsPNG("png_create_info_struct", 0, pngFile, png_ptr, NULL);

	if(setjmp(png_jmpbuf(png_ptr))) handleErrorsPNG("setjmp", 0, pngFile, png_ptr, info_ptr);

	// on associe le ptr du PNG au Fichier.PNG
	png_init_io(png_ptr, pngFile);

	// les information sont stocké dans le ptr info
	png_read_info(png_ptr, info_ptr);

	// on obtient les infos
	png.width = png_get_image_width(png_ptr, info_ptr);
	png.height = png_get_image_height(png_ptr, info_ptr);
	png.color_type = png_get_color_type(png_ptr, info_ptr);
	png.bit_depth = png_get_bit_depth(png_ptr, info_ptr);


	// Read any color_type into 8bit depth, RGBA format.
	// See http://www.libpng.org/pub/png/libpng-manual.txt

	if(png.bit_depth == 16)
		png_set_strip_16(png_ptr);

	if(png.color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);

	// PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
	if(png.color_type == PNG_COLOR_TYPE_GRAY && png.bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png_ptr);

	if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);

	// These color_type don't have an alpha channel then fill it with 0xff.
	if(png.color_type == PNG_COLOR_TYPE_RGB ||
	   png.color_type == PNG_COLOR_TYPE_GRAY ||
	   png.color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);

	if(png.color_type == PNG_COLOR_TYPE_GRAY ||
	   png.color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png_ptr);

	png_read_update_info(png_ptr, info_ptr);

	// on créer un pointeur sur les lignes du png
	//png_bytep *row_pointers;
	png.row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * png.height);
	int y, x;
	for(y = 0; y < png.height; y++) {
		png.row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png_ptr,info_ptr));
	}

	// on stocke les ligne du png dans row_pointers
	png_read_image(png_ptr, png.row_pointers);

	fclose(pngFile);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	return png;
}



void setPNG(PNG png) {

	/* //////////////////////////// WRITE PNG //////////////////////////// */

	FILE *pngFile = fopen("out.png", "wb");
	if(!pngFile) abort();

	png_structp ptr_png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!ptr_png) handleErrorsPNG("png_create_write_struct", 1, pngFile, NULL, NULL);

	png_infop ptr_info = png_create_info_struct(ptr_png);
	if (!ptr_info) handleErrorsPNG("png_create_info_struct", 1, pngFile, ptr_png, NULL);

	if (setjmp(png_jmpbuf(ptr_png))) handleErrorsPNG("setjmp", 1, pngFile, ptr_png, ptr_info);

	png_init_io(ptr_png, pngFile);

	// Output is 8bit depth, RGBA format.
	png_set_IHDR(
				ptr_png,
				ptr_info,
				png.width, png.height,
				8,
				PNG_COLOR_TYPE_RGBA,
				PNG_INTERLACE_NONE,
				PNG_COMPRESSION_TYPE_DEFAULT,
				PNG_FILTER_TYPE_DEFAULT
				);

	png_write_info(ptr_png, ptr_info);

	// To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
	// Use png_set_filler().
	//png_set_filler(png, 0, PNG_FILLER_AFTER);

	png_write_image(ptr_png, png.row_pointers);
	png_write_end(ptr_png, NULL);

	fclose(pngFile);
	png_destroy_write_struct(&ptr_png, &ptr_info);
}

void LSB(PNG png, MASQUE *tab, int cypher_len){

	int i = 0;
	int y,x;
	for(y = 0; y < png.height; y++) {
		png_bytep row = png.row_pointers[y];
		for(x = 0; x < png.width; x++) {
			png_bytep px = &(row[x * 4]);
			// Do something awesome for each pixel here...
			//printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);
			px[0] = px[0] & 0b11111100;
			px[1] = px[1] & 0b11111100;
			px[2] = px[2] & 0b11111100;
			px[3] = px[3] & 0b11111100;

			if (i <= cypher_len){
				px[0] = px[0] | tab[i].duo_premier;
				px[1] = px[1] | tab[i].duo_second;
				px[2] = px[2] | tab[i].duo_troisieme;
				px[3] = px[3] | tab[i].duo_dernier;
			}

			i++;
		}
	}
}


void findCypher(PNG png, unsigned char *cyphertext, int cyphertext_len){

	MASQUE *tab = malloc(cyphertext_len * sizeof(MASQUE));
	int i = 0;
	int y,x;
	for(y = 0; y < png.height; y++) {
		png_bytep row = png.row_pointers[y];
		for(x = 0; x < png.width; x++) {
			png_bytep px = &(row[x * 4]);
			// Do something awesome for each pixel here...
			//printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);
			if (i <= cyphertext_len){

				tab[i].duo_premier = px[0] & 0b00000011;
				tab[i].duo_second = px[1] & 0b00000011;
				tab[i].duo_troisieme = px[2] & 0b00000011;
				tab[i].duo_dernier = px[3] & 0b00000011;

				tab[i].duo_premier = tab[i].duo_premier << 6;
				tab[i].duo_second = tab[i].duo_second << 4;
				tab[i].duo_troisieme = tab[i].duo_troisieme << 2;
				tab[i].duo_dernier = tab[i].duo_dernier ;
			}
			
			i++;
		}
	}

	for(i = 0; i < cyphertext_len; i++){
		cyphertext[i] = tab[i].duo_premier | tab[i].duo_second | tab[i].duo_troisieme | tab[i].duo_dernier;
	}

	free(tab);
}

void handleErrorsPNG(char* fn, int rorw, FILE *file, png_structp png, png_infop info){

	if (rorw = 0) {
		if (png && info){
			png_destroy_read_struct(&png, &info, NULL);
		} else if (png && !info){
			png_destroy_read_struct(&png, NULL, NULL);
		}
		fclose(file);
		printf("ERREUR fonction %s\n", fn);
		abort();
	} else if(rorw = 1) {
		if (png && info){
			png_destroy_write_struct(&png, &info);
		} else if (png && !info){
			png_destroy_write_struct(&png, NULL);
		}
		fclose(file);
		printf("ERREUR fonction %s\n", fn);
		abort();
	}

}