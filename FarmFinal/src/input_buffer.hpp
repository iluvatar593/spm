/*
 * input_buffer.hpp
 *
 *  Created on: 07/ott/2014
 *      Author: alessandro
 */

#ifndef INPUT_BUFFER_HPP_
#define INPUT_BUFFER_HPP_
#include "utils.hpp"
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




#endif /* INPUT_BUFFER_HPP_ */
