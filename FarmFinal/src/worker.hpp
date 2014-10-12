/*
 * worker.hpp
 *
 *  Created on: 12/ott/2014
 *      Author: alessandro
 */

#ifndef WORKER_HPP_
#define WORKER_HPP_

#include <ff/node.hpp>
#include "utils.hpp"
using namespace ff;

template <typename NUM>
class Worker: public ff_node {
public:
	Worker(unsigned int size, unsigned int id, NUM*** outputBuffer):size(size) {
		first = _C = outputBuffer[2*id];
		second = outputBuffer[2*id+1];
	}
	~Worker() {
		for(int i = 0; i < size; i++) {
			delete[] first[i];
			delete[] second[i];
		}
		delete[] first;
		delete[] second;
	}
	void *svc(void*restrict task) {
		cleanC();
		pointerTask<NUM> *restrict t = (pointerTask<NUM>*restrict) task;
		NUM **restrict  A = t->A;
		NUM **restrict  B = t->B;
		this->matrixMultiplication(A, B, _C);
		_C = (_C == first) ? second : first;
		return (_C == first) ? second : first;
	}
protected:
	NUM **first, ** second, ** _C;
	unsigned int size;
	virtual inline void matrixMultiplication(NUM **restrict A, NUM**restrict B, NUM**restrict C)=0;
	inline void normalMatrixMultiplication(NUM **restrict A, NUM** restrict B, NUM** restrict C, unsigned int size, unsigned int offsetA=0, unsigned int offsetB=0, unsigned int offsetC=0) {
		NUM *restrict Bvector;
		NUM *restrict Cvector;
		for(unsigned int i = 0; i < size; i++) {
			Cvector = &C[i][offsetC];
			for(unsigned int k = 0; k < size; k++) {
				Bvector = &B[k][offsetB];
				register NUM aik = A[i][k+offsetA];
				for (unsigned int j = 0; j < size; j++)
						Cvector[j] += aik* Bvector[j];
			}
		}
	}
private:
	void cleanC() {
		for(unsigned int i = 0; i < size; i++)
			for(unsigned int j = 0; j < size; j++)
				_C[i][j] = 0;
	}
};




#endif /* WORKER_HPP_ */