
float* readFileTest(FILE *file){

    int fsize;
    float *sample;

    if(file == NULL){

        printf("Failed to open file!");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    sample = (float*)malloc(fsize);
    fread(sample, 1, fsize, file);
    fclose(file);

    return sample;
}

void mountMatrixTest(float vec[][n_bands], int arraySize, float *sample){

    int aux;

	aux = 0;

	for (int i = 0; i < arraySize; i++){
		for (int j = 0; j < n_bands; j++){

			vec[i][j] = sample[i + aux];
			aux += arraySize;
		}

		aux = 0;
	}
}


