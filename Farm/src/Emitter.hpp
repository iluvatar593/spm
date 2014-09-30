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
    Emitter(long ntasks, unsigned int msize):ntasks(ntasks), msize(msize) {
    A = B = C = nullptr;
    }

    void* svc(void*) {
    	for(long i=1;i<=ntasks;++i){
        	A = (NUM*)calloc(msize*msize, sizeof(NUM));
    		B = (NUM*)calloc(msize*msize, sizeof(NUM));
    		C = (NUM*)calloc(msize*msize, sizeof(NUM));
    		FarmTask<NUM> *t = new FarmTask<NUM>(A, B, C, msize);
    		ff_send_out((void*) t);
    	}
    	return NULL;
    }
    ~Emitter();

protected:
    virtual void init() = 0;
    NUM* A, B, C;

private:
    long ntasks;
    unsigned int msize;

};

#endif /* EMITTER_HPP_ */
