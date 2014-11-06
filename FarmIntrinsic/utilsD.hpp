/*
 * utilsD.hpp
 *
 *  Created on: 05/nov/2014
 *      Author: atzoril
 */

#ifndef UTILSD_HPP_
#define UTILSD_HPP_

#include <stdio.h>
#include <stdlib.h>
#include "utils.hpp"


#ifndef TILE
	#if defined(__MIC__)
		#define TILE 120
	#else
		#define TILE 128
	#endif
#endif

/** (Random) initialization of double matrix "matrix" with rows rows and columns cols.
 *
 */

void initializeDoubleMatrix(double *matrix, int rows, int cols) {
	for(int i = 0; i < rows; i++) {
		for(int j = 0; j < cols; j++) {
			matrix[i*cols+j] = ((double) (rand()%50)/(rand()%20+1.1));
		}
	}
}

typedef struct __attribute__((align(ALIGNMENT))) {
	double *__restrict__ a;
	double *__restrict__ b;
	int num;
} taskDouble_t;



#endif /* UTILSD_HPP_ */
