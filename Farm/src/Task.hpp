/*
 * Task.hpp
 *
 *  Created on: 30/set/2014
 *      Author: alessandro
 */

#ifndef TASK_HPP_
#define TASK_HPP_

#include <stdexcept>
using namespace ff;

template <typename NUM>
class FarmTask {
public:
	FarmTask(NUM* First, NUM*Second, NUM*Result, const unsigned int size) {
		if(First == nullptr || Second == nullptr || Result == nullptr || size == 0) throw std::invalid_argument{"Invalid Task initialization."};
		A = First; B = Second; C = Result;
		matrixSize = size;
	}
	~FarmTask() {
		delete A;
		delete B;
		//delete C; //TODO: check if it makes sense.
	}

	NUM* getFirst() {
		return A;
	}

	NUM* getSecond() {
		return B;
	}

	void set(const unsigned int i,const unsigned int j, NUM value) {
		if(i >= matrixSize || j >= matrixSize) throw std::out_of_range{"Cannot access specified position."};
		C[i*matrixSize+j] = value;
	}

	unsigned int getSize() {
		return matrixSize;
	}
private:
	NUM *A;
	NUM *B;
	NUM *C;
	unsigned int matrixSize;
};



#endif /* TASK_HPP_ */
