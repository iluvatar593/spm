/*
 * Task.hpp
 *
 *  Created on: 30/set/2014
 *      Author: alessandro
 */

#ifndef TASK_HPP_
#define TASK_HPP_

#include <stdexcept>
#include <stdio.h>
#include "matrix.h"
using namespace ff;

template <typename NUM>
class FarmTask {
public:
	FarmTask(matrix<NUM>* First, matrix<NUM>*Second) {
		if(First == nullptr || First == NULL || Second == NULL || Second == nullptr) {
			printf("One is null\n");
			return;//throw std::invalid_argument{"Invalid Task initialization."};
		}
		A = First; B = Second;
		matrixSize = A->getRows();
	}

	FarmTask(NUM** First, NUM** Second, const unsigned int size) {
		if(First == nullptr || Second == nullptr || size == 0) return;
		printf("Legacy constructor \n");
		A = new simple_matrix<NUM>(size, size, First);
		B = new simple_matrix<NUM>(size, size, Second);
		matrixSize = size;
	}
	~FarmTask() {}

	matrix<NUM>* getFirst() {
		return A;
	}

	matrix<NUM>* getSecond() {
		return B;
	}

	unsigned int getSize() {
		return matrixSize;
	}
private:
	matrix<NUM> *A;
	matrix<NUM> *B;
	unsigned int matrixSize;
};

template<typename NUM>
class simple_task {
public:
	simple_task(NUM** first, NUM ** second, unsigned int rows, unsigned int cols):rows(rows),cols(cols) {
		A = first; B = second;
	}
	~simple_task(){}
	inline unsigned int getRows(){return rows;}
	inline unsigned int getCols(){return cols;}
	inline NUM** getFirst(){return A;}
	inline NUM** getSecond(){return B;}
private:
	unsigned int rows, cols;
	NUM **A, **B;
};


template<typename NUM>
class simple_linear_task {
public:
	simple_linear_task(NUM* first, NUM * second, unsigned int rows, unsigned int cols):rows(rows),cols(cols) {
		A = first; B = second;
	}
	~simple_linear_task(){}
	inline unsigned int getRows(){return rows;}
	inline unsigned int getCols(){return cols;}
	inline NUM* getFirst(){return A;}
	inline NUM* getSecond(){return B;}
private:
	unsigned int rows, cols;
	NUM *A, *B;
};


#endif /* TASK_HPP_ */
