///*
// * IntegerEmitter.cpp
// *
// *  Created on: 30/set/2014
// *      Author: atzoril
// */
//

//#include <vector>
//#include <iostream>
//#include <cstdlib>
//#include <ff/node.hpp>
//#include "Task.hpp"

#include "IntegerEmitter.h"

	void IntegerEmitter::initialize() { //TODO: randomize
		for(register unsigned int i = 0; i < this->msize; i++) {
			for(register unsigned int j = 0; j < this->msize; j++) {
				A[i*this->msize+j] = B[i*this->msize+j] = (i == j) ? 1 : 0;
			}
		}
	}
