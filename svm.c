#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "defines.h"
//#include "fileReader.h"
#include "qmath.h"
//#include "test.h"

fixedp qlog( fixedp p_Base )
{
    fixedp w = 0;
	fixedp y = 0;
	fixedp z = 0;
	fixedp num = int2q(1);
	fixedp dec = 0;

	if ( p_Base == int2q(1) )
		return 0;

	if ( p_Base == 0 )
		return 0xffffffff;

	for ( dec=0 ; qabs( p_Base ) >= int2q(2) ; dec += int2q(1) )

p_Base = qdiv(p_Base, QLN_E);

	p_Base -= int2q(1);
	z = p_Base;
	y = p_Base;
	w = int2q(1);

	while ( y != y + w )
	{
		z = 0 - qmul( z , p_Base );
		num += int2q(1);
		w = qdiv( z , num );
		y += w;
	}

	return y + dec;
}

/**
 * exp (e to the x)
 */
fixedp qexp(fixedp p_Base)
{
	fixedp w;
	fixedp y;
	fixedp num;

	for ( w=int2q(1), y=int2q(1), num=int2q(1) ; y != y+w ; num += int2q(1) )
	{
		w = qmul(w, qdiv(p_Base, num));
		y += w;
	}

	return y;
}

/**
 * pow
 */
fixedp qpow( fixedp p_Base, fixedp p_Power )
{
	if ( p_Base < 0 && qmod(p_Power, int2q(2)) != 0 )
		return - qexp( qmul(p_Power, qlog( -p_Base )) );
	else
		return qexp( qmul(p_Power, qlog(qabs( p_Base ))) );
}

void svm(fixedp fixedp_sample[n_classifiers * n_bands],
		unsigned short int svSizes[],
		fixedp fixedp_alpha[5532],
		fixedp fixedp_labels[5532],
		fixedp fixedp_kernelScale[n_classifiers],
		fixedp fixedp_bias[n_classifiers],
		fixedp fixedp_supportVector[5532 * n_bands],
		unsigned short int resultCode[n_classifiers],
		unsigned short int idCodes[n_classes][n_classifiers],
		unsigned short int maskCodes[n_classes][n_classifiers]){


	int i = 0, j = 0, k = 0, l = 0;

	fixedp fixedp_euclideanOP;
	fixedp fixedp_accumulator01 = int2q(0);
	fixedp fixedp_accumulator02 = int2q(0);
	fixedp fixedp_neg = float2q(-1);
	fixedp fixedp_alpha_label;
	fixedp fixedp_power2 = int2q(2);

	unsigned short int band_index = 0;
	unsigned int supp_index = 0;
	unsigned short int sv_index = 0;

    //start = clock();

	for (i = 0; i < n_classifiers; i++){
        for (j = 0; j < svSizes[i]; j++){
            for (k = 0; k < n_bands; k++){
//#pragma HLS PIPELINE
                fixedp_euclideanOP = qsub(fixedp_sample[k + band_index], fixedp_supportVector[supp_index]);

                fixedp_accumulator01 = qadd(fixedp_accumulator01, qmul(fixedp_euclideanOP, fixedp_euclideanOP));

                supp_index += 1;
            }

            fixedp_accumulator01 = qmul(qdiv(fixedp_accumulator01, qpow(fixedp_kernelScale[i], fixedp_power2)), fixedp_neg);

            if (fixedp_accumulator01 > int2q(0)) //Redundancy required by library failure
                fixedp_accumulator01 = qmul(fixedp_accumulator01, fixedp_neg);

            fixedp_accumulator01 = qexp(fixedp_accumulator01);

            if (qabs(fixedp_accumulator01) > int2q(1)) //Redundancy required by library failure
                fixedp_accumulator01 = int2q(0);

            fixedp_alpha_label = qmul(fixedp_alpha[sv_index], fixedp_labels[sv_index]);

            fixedp_accumulator01 = qmul(fixedp_accumulator01, fixedp_alpha_label);

            fixedp_accumulator02 = qadd(fixedp_accumulator02, fixedp_accumulator01);

            fixedp_accumulator01 = int2q(0);

            sv_index += 1;
        }

        fixedp_accumulator02 = qadd(fixedp_accumulator02, fixedp_bias[i]);

        if (fixedp_accumulator02 < int2q(0))
            resultCode[i] = 1;
        else
            resultCode[i] = 0;

        //printf("%f\n", q2float(fixedp_accumulator02));

        fixedp_accumulator02 = float2q(0);

        band_index += 20;
    }

	//return resultCode;

	//Hamming distance
	unsigned short int nOnes = n_classifiers;
	unsigned short int count = 0;
	unsigned short int pixelClass;

	for (i = 0; i < n_classes; i++)
	{
		count = 0;

		for (j = 0; j < n_classifiers; j++)
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
}
