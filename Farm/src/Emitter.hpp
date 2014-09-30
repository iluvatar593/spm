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
#include "Task.hpp"

using namespace ff;

#ifndef EMITTER_HPP_
#define EMITTER_HPP_

template <typename NUM>
class Emitter: public ff_node {
public:
    Emitter(long ntasks, unsigned int msize){
    	this->ntasks = ntasks;
    	this->msize = msize;
    	A = B = C = nullptr;
    }

    void* svc(void*) {
    	for(long i=1;i<=ntasks;++i){
        	A = (NUM*)calloc(msize*msize, sizeof(NUM));
    		B = (NUM*)calloc(msize*msize, sizeof(NUM));
    		C = (NUM*)calloc(msize*msize, sizeof(NUM));
    		this->initialize();
    		FarmTask<NUM> *t = new FarmTask<NUM>(A, B, C, msize);
    		ff_send_out((void*) t);
    	}
    	return NULL;
    }
    ~Emitter();
    virtual void initialize() = 0;
    NUM * A, *B, *C;
    unsigned int msize;
private:
    long ntasks;


};

#endif /* EMITTER_HPP_ */
