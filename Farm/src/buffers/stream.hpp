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
class streamInterface {
public:
	virtual void* getNext()=0;
	~streamInterface(){};
};

template<typename NUM>
class stream: public matrix_buffer<NUM> {
public:
	stream(unsigned int streamLength, unsigned int size, unsigned int rows, unsigned int cols, unsigned int offset=2):matrix_buffer<NUM>(size, rows, cols), start(0),streamLength(streamLength),offset(offset),size(size){};
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
template<typename NUM>
class plain_stream: public plain_buffer<NUM>,public streamInterface<NUM> {
public:
	plain_stream(unsigned int streamLength, unsigned int size, unsigned int rows, unsigned int cols, unsigned int offset=2):plain_buffer<NUM>(size, rows, cols), start(0),streamLength(streamLength),offset(offset),size(size),rows(rows),cols(cols){};
	~plain_stream(){};
	void add(NUM** m) {
		if(m == NULL) printf("Cannot add null matrix");
		plain_buffer<NUM>::add(m);
	}
	simple_task<NUM>* getNext() {
		if(produced == streamLength) return NULL;
		NUM**A = plain_buffer<NUM>::get(start%size);
		NUM**B = plain_buffer<NUM>::get((start+offset)%size);
		printf("s %d o %d\n", start%size, (start+offset)%size);
		simple_task<NUM> * next = new simple_task<NUM>(A,B, rows, cols);
		start++;
		produced++;
		if(produced % size == 0) offset++;
		return next;
	}

private:
	unsigned int start, streamLength, offset, size, rows, cols,produced=0;
};


template<typename NUM>
class linearized_stream: public linearized_buffer<NUM>,public streamInterface<NUM> {
public:
	linearized_stream(unsigned int streamLength, unsigned int size, unsigned int rows, unsigned int cols, unsigned int offset=2):linearized_buffer<NUM>(size, rows, cols), start(0),streamLength(streamLength),offset(offset),size(size),rows(rows),cols(cols){};
	~linearized_stream(){};
	void add(NUM* m) {
		if(m == NULL) printf("Cannot add null matrix");
		linearized_buffer<NUM>::add(m);
	}
	simple_linear_task<NUM>* getNext() {
		if(produced == streamLength) return NULL;
		NUM*A = linearized_buffer<NUM>::get(start%size);
		NUM*B = linearized_buffer<NUM>::get((start+offset)%size);
		printf("s %d o %d\n", start%size, (start+offset)%size);
		simple_linear_task<NUM> * next = new simple_linear_task<NUM>(A,B, rows, cols);
		start++;
		produced++;
		if(produced % size == 0) offset++;
		return next;
	}

private:
	unsigned int start, streamLength, offset, size, rows, cols,produced=0;
};

//class returning tasks.






#endif /* STREAM_HPP_ */
