/*
 * IntegerEmitter.h
 *
 *  Created on: 30/set/2014
 *      Author: atzoril
 */

#ifndef INTEGEREMITTER_H_
#define INTEGEREMITTER_H_

#include "Emitter.hpp"

class IntegerEmitter: public Emitter<int> {

public:
	IntegerEmitter(long n, unsigned int s):Emitter<int>(n,s) {}
	~IntegerEmitter(){}
	virtual void initialize();

};

#endif /* INTEGEREMITTER_H_ */
