/*	FILE *pngFile = fopen("gradient", "rb+");
	if (pngFile == NULL) perror("fopen");
	// Get file size
	if(0 != fseek(pngFile, 0L, SEEK_END)) perror("fseek");
	int png_len = ftell(pngFile);
	//set curseur back to normal
	if(0 != fseek(pngFile, 0L, SEEK_SET)) perror("fseek");
	

	// Signature = 8 Octets
	// IHDR = 25 octets
	// on positionne le curseur sur le debut du chunk IDAT -> (25+8) octets
	fseek(pngFile, 33, SEEK_SET);

	// Taille des données du Chunk -> 4 octets
	unsigned int data_len;
	unsigned char data_len_hex[4];
	nmb = fread(data_len_hex, 1, 4, pngFile);
	data_len = (data_len_hex[3]<<0) | (data_len_hex[2]<<8) | (data_len_hex[1]<<16) | (data_len_hex[0]<<24);
	printf("%d\n", data_len);

	// Type du chunk -> 4 octets == "IDAT"
	char *chunk_name = malloc(4);
	nmb = fread(chunk_name, 1, 4, pngFile);
	if(0 == strcmp(chunk_name, "IDAT")){
		printf("Nous somme dans le chunk %s\n\n", chunk_name);
	} else printf("ERREUR\n");

	// Donnes du chunk -> data_len octets 
	unsigned char *pngData = malloc(data_len);
	nmb = fread(pngData, 1, data_len, pngFile);
	int i;
	for(i = 0; i < data_len; i++){
		printf("%02X ", pngData[i]);
		if (i % 17 == 16) printf("\n");
	}

	printf("\n\n");

	// CRC du chunk
	printf("CRC = ");
	unsigned char *pngCRC = malloc(4);
	
	nmb = fread(pngCRC, 1, 4, pngFile);
	
	for(i = 0; i < 4; i++){
		printf("%02X ", pngCRC[i]);
	}

	int crc = crc32(0L, Z_NULL, 0);

	for (i = 0; i < data_len; i++) {
		crc = crc32(crc, pngData, 1);
	}
	printf("CRC32 value is: %d \n", crc);


		//fclose(pngFile);
