/*
 * stream.hpp
 *
 *  Created on: 03/ott/2014
 *      Author: alessandro
 */

#ifndef STREAM_HPP_
#define STREAM_HPP_

#include "Buffer.hpp"
template<typename NUM>
class stream: public matrix_buffer<NUM> {
public:
	stream(unsigned int streamLength, unsigned int size, unsigned int rows, unsigned int cols):matrix_buffer<NUM>(size, rows, cols), start(0),streamLength(streamLength),offset(2),size(size){};
	~stream(){};
	FarmTask<NUM>* getNext() {
		if(produced == streamLength) return NULL;
		FarmTask<NUM> * next = new FarmTask<NUM>(matrix_buffer<NUM>::get(start%size), matrix_buffer<NUM>::get((start+offset)%size));
		start++;
		produced++;
		if(produced % size == 0) offset++;
		return next;
	}
	void add(NUM** matrix) {matrix_buffer<NUM>::add(matrix);}
private:
	unsigned int start, streamLength,offset,size,produced=0;
};

//class returning tasks.






#endif /* STREAM_HPP_ */
