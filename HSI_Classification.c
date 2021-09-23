//Title: HSI classification - FixedPoint
//Author: Leonardo Rebello Januário

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "defines.h"
//#include "fileReader.h"
#include "qmath.h"
//#include "test.h"

//File Reader
float* readFile(FILE *file){

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
    //fclose(file);

    printf("Deu boa!");

    return sample;
}

void mountMatrix(float vec[][n_bands], int arraySize, float *sample){

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


/**
 * log (base e)
 */


int main(){

    //clock_t start, end;
	//double cpu_time_used;

    FILE *fid;
    float *sample, *bias, *kernelScale, *supportVector, *alpha, *labels;

    unsigned short int resultCode[n_classifiers] = { 0 };

    unsigned short int svSizes[] = { size1 , size2 , size3 , size4 , size5 ,
									 size6 , size7 , size8 , size9 , size10,
									 size11, size12, size13, size14, size15,
									 size16, size17, size18, size19, size20,
									 size21, size22, size23, size24, size25,
									 size26, size27, size28 };

    unsigned short int idCodes[n_classes][n_classifiers] = { {1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
															{0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
															{0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0},
															{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0},
															{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0},
															{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0},
															{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
															{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} };

	unsigned short int maskCodes[n_classes][n_classifiers] = { {1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
															  {1,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
															  {0,1,0,0,0,0,0,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0},
															  {0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,1,1,1,0,0,0,0,0,0},
															  {0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,0,0,1,1,1,0,0,0},
															  {0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,0,1,0,0,1,1,0},
															  {0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,0,1,0,1,0,1},
															  {0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,0,1,0,1,1} };

    //Read sample data ------------------------------------------------------------------
    fid = fopen("stdPixelC3_08.dat", "rb");
    printf("LEITURAAA ");
    sample = readFile(fid);
    fclose(fid);
    //-----------------------------------------------------------------------------------

    //Read bias data --------------------------------------------------------------------
    fid = fopen("SWIR_Parameters/bias.dat", "rb");
    bias = readFile(fid);
    fclose(fid);
    //-----------------------------------------------------------------------------------

    //Read kernelScale ------------------------------------------------------------------
    fid = fopen("SWIR_Parameters/kernelScale.dat", "rb");
    kernelScale = readFile(fid);
    fclose(fid);
    //-----------------------------------------------------------------------------------

    //Read supportVector data -----------------------------------------------------------
    fid = fopen("SWIR_Parameters/supportVector_allData.dat", "rb");
    supportVector = readFile(fid);
    fclose(fid);
    //-----------------------------------------------------------------------------------

    //Read alpha data -------------------------------------------------------------------
    fid = fopen("SWIR_Parameters/alphas_allData.dat", "rb");
    alpha = readFile(fid);
    fclose(fid);
    //-----------------------------------------------------------------------------------

    //Read labels data ------------------------------------------------------------------
    fid = fopen("SWIR_Parameters/labels_allData.dat", "rb");
    labels = readFile(fid);
    fclose(fid);
    //-----------------------------------------------------------------------------------

    fixedp fixedp_sample[n_classifiers * n_bands];
    fixedp fixedp_bias[n_classifiers];
    fixedp fixedp_kernelScale[n_classifiers];
    fixedp fixedp_supportVector[5532 * n_bands];
    fixedp fixedp_alpha[5532];
    fixedp fixedp_labels[5532];

    unsigned short int band_index = 0;
    unsigned short int sample_index = 0;
    unsigned int supp_index = 0;
    unsigned short int sv_index = 0;

    printf("\n\n");

    // Chamando função svm
    svm(fixedp_bias, kernelScale, fixedp_sample, sample_index, sample, band_index, svSizes, fixedp_alpha, fixedp_labels, fixedp_kernelScale, sv_index, fixedp_supportVector, supp_index, supportVector, resultCode, alpha, labels, bias);

		//Hamming distance
	unsigned short int nOnes = n_classifiers;
	unsigned short int count = 0;
	unsigned short int pixelClass;

	for (int i = 0; i < n_classes; i++)
	{
		count = 0;

		for (int j = 0; j < n_classifiers; j++)
		{
			if (resultCode[j] != idCodes[i][j])
			{
				if (maskCodes[i][j] == 1)
				{
					count++;
				}
			}
		}

		if (count < nOnes)
		{
			nOnes = count;
			pixelClass = i + 1;
		}
	}

	//end = clock();
	//cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
	//printf("Total time taken by CPU: %lf\n\n", cpu_time_used);

	printf("\nPixel classified to class: %d", pixelClass);
	printf("\n");

	return 0;
}
