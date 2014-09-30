/*
 * DoubleEmitter.h
 *
 *  Created on: 30/set/2014
 *      Author: alessandro
 */

#ifndef DOUBLEEMITTER_H_
#define DOUBLEEMITTER_H_

#include "Emitter.hpp"

class DoubleEmitter: public Emitter<double> {
public:
	DoubleEmitter(long streamLen, unsigned int size):Emitter<double>(streamLen, size) {}
	~DoubleEmitter(){}
	//void* svc(void*){Emitter<double>:}
	void initialize() {
		for(register unsigned int i = 0; i < this->msize; i++) {
			for(register unsigned int j = 0; j < this->msize; j++) {
				A[i*this->msize+j] = B[i*this->msize+j] = (i == j) ? 1.0 : 0.0;
			}
		}
	}
};

#endif /* DOUBLEEMITTER_H_ */
