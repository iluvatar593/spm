/*
 * DoubleEmitter.cpp
 *
 *  Created on: 30/set/2014
 *      Author: alessandro
 */

#include "DoubleEmitter.h"


void DoubleEmitter::initialize() { //TODO: randomize
	for(register unsigned int i = 0; i < this->msize; i++) {
		for(register unsigned int j = 0; j < this->msize; j++) {
			A[i*this->msize+j] = B[i*this->msize+j] = (i == j) ? 1.0 : 0.0;
		}
	}
}
