/*
 * Emitter.hpp
 *
 *  Created on: 30/set/2014
 *      Author: atzoril
 */

#include <vector>
#include <iostream>
#include <cstdlib>
#include <ff/node.hpp>
#include "Utils.h"
#include "buffers/stream.hpp"
using namespace ff;

#ifndef EMITTER_HPP_
#define EMITTER_HPP_
template<typename NUM>
class Emitter: public ff_node {
public:
    Emitter(plain_stream<NUM>* input_stream):ff_node(),asd(input_stream) {}
    void* svc(void*) {
		printf("EID %d\n", this->getCPUId());
    	return (void*) asd->getNext();
    }
    ~Emitter(){}
private:
	plain_stream<NUM> *asd;
};

#endif /* EMITTER_HPP_ */
