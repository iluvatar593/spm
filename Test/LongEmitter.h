/*
 * LongEmitter.h
 *
 *  Created on: 30/set/2014
 *      Author: atzoril
 */

#ifndef LONGEMITTER_H_
#define LONGEMITTER_H_

#include "Emitter.hpp"

class LongEmitter: public Emitter<long> {

public:
	LongEmitter(long n, unsigned int s):Emitter<long>(n,s) {}
	~LongEmitter(){}
	virtual void initialize();

};

#endif /* LONGMITTER_H_ */
