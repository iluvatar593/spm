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
class LWorker: public ff_node{
public:
	LWorker(unsigned int size, unsigned int id, NUM** outputBuffer):size(size) {
		first = _C = outputBuffer[2*id];
		second = outputBuffer[2*id+1];
	}
	~LWorker() {
		_mm_free(first);
		_mm_free(second);
	}
	void *svc(void*restrict task) {
		cleanC();
		LpointerTask<NUM> *restrict t = (LpointerTask<NUM>*restrict) task;
		NUM *restrict  A = t->A;
		NUM *restrict  B = t->B;
		this->matrixMultiplication(A, B, _C, size);
		_C = (_C == first) ? second : first;
		return (_C == first) ? second : first;
	}
protected:
	NUM *first, * second, * _C;
	unsigned int size;
	inline void matrixMultiplication(NUM *restrict A, NUM* restrict B, NUM* restrict C, unsigned int size) {

		NUM *restrict Bvector;
		NUM *restrict Cvector;
		for(unsigned int i = 0; i < size; i++) {
			Cvector = &C[i*size];
			for(unsigned int k = 0; k < size; k++) {
				Bvector = &B[k*size];
				register NUM aik = A[i*size+k];
				#pragma ivdep
				for (unsigned int j = 0; j < size; j++)
						Cvector[j] += aik* Bvector[j];
			}
		}
	}

private:
	void cleanC() {
		for(unsigned int i = 0; i < size*size; i++)
			_C[i] = 0;
		}
};

template <typename NUM>
class Worker: public ff_node {
public:
	Worker(unsigned int size, unsigned int id, NUM*** outputBuffer):size(size),id(id) {
		_C = outputBuffer[id];
	}
	~Worker() {
		for(int i = 0; i < size; i++) {
			delete[] _C[i];
			//delete[] second[i];
		}
		delete[] _C;
		//delete[] second;
	}
	void *svc(void*restrict task) {
		cleanC();
		pointerTask<NUM> *restrict t = (pointerTask<NUM>*restrict) task;
		NUM **restrict A = t->A;
		NUM **restrict B = t->B;
		return this->matrixMultiplication(A, B, _C);
	}
protected:
	NUM ** _C;
	unsigned int size, id;
	virtual inline void* matrixMultiplication(NUM **restrict A, NUM**restrict B, NUM**restrict C)=0;
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
	inline void cleanC() {
		for(unsigned int i = 0; i < size; i++)
			for(unsigned int j = 0; j < size; j++)
				_C[i][j] = 0;
	}
};




#endif /* WORKER_HPP_ */
