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
using namespace ff;

template <typename NUM>
class FarmTask {
public:
	FarmTask(NUM* First, NUM*Second, const unsigned int size) {
		if(First == nullptr || Second == nullptr || size == 0) return;//throw std::invalid_argument{"Invalid Task initialization."};
		A = First; B = Second;
		matrixSize = size;
	}
	~FarmTask() {
		delete A;
		delete B;
	}

	NUM* getFirst() {
		return A;
	}

	NUM* getSecond() {
		return B;
	}

	unsigned int getSize() {
		return matrixSize;
	}
private:
	NUM *A;
	NUM *B;
	unsigned int matrixSize;
};



#endif /* TASK_HPP_ */
