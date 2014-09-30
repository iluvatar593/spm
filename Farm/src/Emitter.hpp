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

using namespace ff;

#ifndef EMITTER_HPP_
#define EMITTER_HPP_


class Emitter: public ff_node {
public:
    Emitter(long ntasks):ntasks(ntasks) {}
    void* svc(void*) {
	for(long i=1;i<=ntasks;++i)
	    ff_send_out((void*)i);
	return NULL;
    }
    ~Emitter();
private:
    long ntasks;
};

#endif /* EMITTER_HPP_ */
