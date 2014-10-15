/*
 * emitter.hpp
 *
 *  Created on: 07/ott/2014
 *      Author: alessandro
 */

#ifndef EMITTER_HPP_
#define EMITTER_HPP_

#include <ff/node.hpp>
#include "input_buffer.hpp"
#include "utils.hpp"
using namespace ff;

template <typename NUM>
class LEmitter: public ff_node{
public:
	LEmitter(linear_buffer<NUM>* a, linear_buffer<NUM>* b,unsigned int bufferSize, unsigned int streamLength, unsigned int size):ff_node(),bufferSize(bufferSize),streamLength(streamLength),size(size)
	{
		current=0;
		offsetA = offsetB = 0;
		this->a = a;
		this->b = b;
	}
	~LEmitter(){};
	void * svc(void*) {
		//totaltime = std::chrono::high_resolution_clock::now();
		for(unsigned int i = 0; i < streamLength; i++) {
			LpointerTask<NUM> *t = new LpointerTask<NUM>();
			t->A = a->getNext();
			t->B = b->getNext();
			if(t->A == NULL) printf("Something is null A [Emitter]\n");
			if(t->B == NULL) printf("Something is null B [Emitter]\n");
			ff_send_out((void*) t);
			if(offsetA % bufferSize == 0 && offsetA > 0) {
				offsetB = offsetB +1;
				b->setOffset(offsetB);
			}
			if(i % bufferSize == 0) {
				offsetA = offsetA+1;
				a->setOffset(offsetA);
			}

		}
		return NULL;
	}
private:
	unsigned int bufferSize, streamLength, size, current, offsetA, offsetB;
	linear_buffer<NUM> *a, *b;
};

template <typename NUM>
class Emitter: public ff_node{
public:
	Emitter(input_buffer<NUM>* a, input_buffer<NUM>* b,unsigned int bufferSize, unsigned int streamLength, unsigned int size):ff_node(),bufferSize(bufferSize),streamLength(streamLength),size(size)
	{
		current=0;
		offsetA = offsetB = 0;
		this->a = a;
		this->b = b;
	}
	~Emitter(){};
	void * svc(void*) {
		//totaltime = std::chrono::high_resolution_clock::now();
		for(unsigned int i = 0; i < streamLength; i++) {
			start_time();
			pointerTask<NUM> *t = new pointerTask<NUM>();
			t->A = a->getNext();
			t->B = b->getNext();
			ff_send_out((void*) t);
			if(offsetA % bufferSize == 0 && offsetA > 0) {
				offsetB = offsetB +1;
				b->setOffset(offsetB);
			}
			if(i % bufferSize == 0) {
				offsetA = offsetA+1;
				a->setOffset(offsetA);
			}
			elapsed_time(EMITTER);
		}
		return NULL;
	}
private:
	unsigned int bufferSize, streamLength, size, current, offsetA, offsetB;
	input_buffer<NUM> *a, *b;
};



#endif /* EMITTER_HPP_ */
