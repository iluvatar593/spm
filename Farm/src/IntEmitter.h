/*
 * IntEmitter.h
 *
 *  Created on: 30/set/2014
 *      Author: atzoril
 */

#ifndef INTEMITTER_H_
#define INTEMITTER_H_

#include "Emitter.hpp"

class IntEmitter: public Emitter {
public:
	IntEmitter(long s, long n):Emitter(s, n){};
	virtual ~IntEmitter();
protected:
	void Emitter:init() {

	}
};

#endif /* INTEMITTER_H_ */
