/*
 * utilsF.hpp
 *
 *  Created on: 05/nov/2014
 *      Author: atzoril
 */

#ifndef UTILSF_HPP_
#define UTILSF_HPP_

#include <stdio.h>
#include <stdlib.h>


#if defined(__MIC__)
	#ifndef TILE
		#define TILE 240
	#else
		#if (TILE%240) != 0
			#error TILE must be multiple of 240
		#endif
	#endif
#else
	#ifndef TILE
		#define TILE 128
	#else
		#if (TILE%128) != 0
			#error TILE must be multiple of 128
		#endif
	#endif
#endif

/** (Random) initialization of float matrix "matrix" with rows rows and columns cols.
 *
 */

void initializeFloatMatrix(float *matrix, int rows, int cols) {
	for(int i = 0; i < rows; i++) {
		for(int j = 0; j < cols; j++) {
			matrix[i*cols+j] = ((float) (rand()%50)/(rand()%20+1.1f));
		}
	}
}

typedef struct __attribute__((align(ALIGNMENT))) {
	float *__restrict__ a;
	float *__restrict__ b;
	int num;
} taskFloat_t;


#endif /* UTILSF_HPP_ */
