/*
 * Buffer.hpp
 *
 *  Created on: 02/ott/2014
 *      Author: alessandro
 */

#ifndef BUFFER_HPP_
#define BUFFER_HPP_

#include <stdlib.h>
#include "matrix.h"
//Simple buffer of matrices, using simple_matrix from matrix.h
template<typename NUM>
class plain_buffer {
public:
	plain_buffer(unsigned int size, unsigned int rows, unsigned int cols): size(size), rows(rows), cols(cols),next(0) {
		buffer = new NUM**[size]();
	}
	~plain_buffer() {
		for(int i = 0; i < size; i++) delete_matrix(buffer[i]);
		delete[] buffer;
	}
	void add(NUM** matrix) {
		if(next >= size) return;
		buffer[next] = matrix;
		next++;
	}
	NUM** get(unsigned int offset) {
		if(offset >= next) return nullptr;
		return buffer[offset];
	}
private:
	NUM*** buffer;
	unsigned int size, rows, cols,next;
	void delete_matrix(NUM** m) {
		for(unsigned int i = 0; i < rows; i++) delete[] m[i];
		delete[] m;
	}
};


template<typename NUM>
class matrix_buffer {
public:
	matrix_buffer(unsigned int size, unsigned int rows, unsigned int cols): size(size), rows(rows), cols(cols){
		buffer = (simple_matrix<NUM>**) calloc(size, sizeof(simple_matrix<NUM>*));
	}
	~matrix_buffer(){
		for(int i = 0; i < next; i++) {delete buffer[i];}
		free(buffer);
	}

	void add(NUM** matrix) {
		if(next < size) {
		//simple_matrix<NUM> *a = new simple_matrix<NUM>(rows, cols, matrix);
		buffer[next] = new simple_matrix<NUM>(rows, cols, matrix);
		next++;
		}
	}

	void add() {
		if(next < size){
		buffer[next] = new simple_matrix<NUM>(rows, cols);
		next++;
		}
	}

	simple_matrix<NUM>* get(unsigned int index) {
		return buffer[index];
	}

private:
	simple_matrix<NUM> **buffer;

	unsigned int size, rows, cols;
	int next = 0;
};



#endif /* BUFFER_HPP_ */
