/*
 * emitter.hpp
 *
 *  Created on: 13/ott/2014
 *      Author: atzoril
 */

#ifndef EMITTER_HPP_
#define EMITTER_HPP_

#include <ff/node.hpp>
#include "input_buffer.hpp"
#include "utils.hpp"
using namespace ff;

template <typename NUM>
class Emitter: public ff_node{
public:
	Emitter(input_buffer<NUM>* a, input_buffer<NUM>* b,
			unsigned int bufferSize, unsigned int streamLength, unsigned int size, unsigned int chunkSize):
			ff_node(),bufferSize(bufferSize),streamLength(streamLength),size(size),chunkSize(chunkSize){
		offsetA = offsetB = 0;
		this->a = a;
		this->b = b;
	}
	~Emitter(){}
	void * svc(void*) {
		for(unsigned int p = 0; p < streamLength; p++) {
			task_t<NUM> *t = new task_t<NUM>();
			t->A = a->getNext();
			t->B = b->getNext();
			t->C = new NUM*[size]();
			t->matrixId = p;
			for(unsigned int i = 0; i < size; i++){
				t->C[i] = new NUM[size]();
			}
			NUM ** tmpA = t->A;
			NUM ** tmpC = t->C;
			for(unsigned int i = 0; i < size; i+=chunkSize){
				tmpA = t->A[i];
				tmpC = t->C[i];
			}
			ff_send_out((void*) t);

			if(offsetA % bufferSize == 0 && offsetA > 0) {
				offsetB = offsetB +1;
				b->setOffset(offsetB);
			}
			if(p % bufferSize == 0) {
				offsetA = offsetA+1;
				a->setOffset(offsetA);
			}
		}
		return NULL;
	}

private:
	unsigned int bufferSize, streamLength, size, offsetA, offsetB, chunkSize;
	input_buffer<NUM> *a, *b;

};



#endif /* EMITTER_HPP_ */
