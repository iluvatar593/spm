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
	virtual void initialize();
};

#endif /* DOUBLEEMITTER_H_ */
