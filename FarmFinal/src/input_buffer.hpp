/*
 * input_buffer.hpp
 *
 *  Created on: 07/ott/2014
 *      Author: alessandro
 */

#ifndef INPUT_BUFFER_HPP_
#define INPUT_BUFFER_HPP_
#include "utils.hpp"
#if defined(__INTEL_COMPILER)
#include <malloc.h>
#else
#include <mm_malloc.h>
#endif

template <typename NUM>
class input_buffer {
public:
	input_buffer(unsigned int size):size(size){
		buffer = new NUM**[size]();
		nextFree = 0;
		offset = 0;
		current = 0;
	}
	~input_buffer(){}
	/** WARNING: don't mix with the other add with specified index!!!!*/
	void add(NUM**restrict matrix){
		if(nextFree >= size) return; //has no effects. this is a buffer of fixed size in which nobody can write once is full
		buffer[nextFree] = matrix;
		nextFree++;
	}

	void add(NUM** restrict matrix, unsigned int position) {
		if(position >= size) return;
		//printf("I buffer initialized %d\n", position);
		buffer[position] = matrix;
	}

	/** This allows to generate, in combination with another buffer, all the possible couples of matrices to multiply*/
	NUM **restrict getNext() {
		NUM ** next = buffer[(current+offset)%size];
		current = (current+1)%size;
		return next;
	}

	void setOffset(unsigned int num) {
		offset = num;
	}
private:
	NUM*** buffer;
	unsigned int nextFree, current, offset;
	unsigned int size;
};


template<typename NUM>
class linear_buffer {
public:
	linear_buffer(unsigned int size):size(size){
		nextFree = current = offset = 0;
		buffer = _mm_malloc(size*sizeof(NUM*), 64);
	}
	~linear_buffer() {
		for(unsigned int i = 0; i < nextFree; i++)
			_mm_free(buffer[i]);
		_mm_free(buffer);
	}

	NUM *restrict getNext() {
		NUM * next = buffer[(current+offset)%size];
		current = (current+1)%size;
		return next;
	}

	void add(NUM*restrict matrix){
		if(nextFree >= size) return; //has no effects. this is a buffer of fixed size in which nobody can write once is full
		buffer[nextFree] = matrix;
		nextFree++;
	}


	void setOffset(unsigned int num) {
			offset = num;
	}


private:
	NUM**buffer;
	unsigned int nextFree, current, offset, size;
};



#endif /* INPUT_BUFFER_HPP_ */
