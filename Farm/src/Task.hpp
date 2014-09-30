/*
 * Task.hpp
 *
 *  Created on: 30/set/2014
 *      Author: alessandro
 */

#ifndef TASK_HPP_
#define TASK_HPP_

#include <stdexcept>

template <typename NUM>
class FarmTask {
public:
	FarmTask(NUM* First, NUM*Second, NUM*Result, const unsigned int size) {
		A = First; B = Second; C = Result;
		matrixSize = size;
	}
	~FarmTask() {
		delete A;
		delete B;
		delete C;
	}

	NUM& getFirst() {
		return A;
	}

	NUM& getSecond() {
		return B;
	}

	void set(const unsigned int i,const unsigned int j, NUM value) {
		if(i >= matrixSize || j >= matrixSize) throw std::out_of_range{"Cannot access specified position."};
		C[i*matrixSize+j] = value;
	}
private:
	NUM *A;
	NUM *B;
	NUM *C;
	unsigned int matrixSize;
};



#endif /* TASK_HPP_ */
