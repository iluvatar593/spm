/*
 * worker_ikj.hpp
 *
 *  Created on: 12/ott/2014
 *      Author: alessandro
 */

#include "worker.hpp"
#include "workerOutput.hpp"
#ifndef WORKER_IKJ_HPP_
#define WORKER_IKJ_HPP_

using namespace ff;

template <typename NUM>
class IKJWorker: public Worker<NUM> {
public:
	IKJWorker(unsigned int size, unsigned int id, NUM*** outputBuffer):Worker<NUM>(size,id, outputBuffer){}
	~IKJWorker(){}
protected:
	inline void* matrixMultiplication(NUM **restrict A, NUM**restrict B, NUM** restrict C) {
		NUM *restrict Bvector;
		NUM *restrict Cvector;
		for(unsigned int i = 0; i < this->size; i++) {
			Cvector = C[i];
			for(unsigned int k = 0; k < this->size; k++) {
				Bvector = B[k];
				register NUM aik = A[i][k];
				for (unsigned int j = 0; j < this->size; j++)
						Cvector[j] += aik* Bvector[j];
			}
			if(i == this->size/2) this->ff_send_out((void*) new workerOutput<NUM>(C, this->id));
		}
		return new workerOutput<NUM>(&C[this->size/2], this->id);

		//this->normalMatrixMultiplication(A, B, C, this->size);
		//matMul(A,B,C);
	}
	inline void matMul(NUM**restrict A, NUM**restrict B, NUM**restrict C) {
		for(unsigned int i = 0; i < this->size; i++) {
			NUM* restrict Avect = A[i];
			for(unsigned int j = 0; j < this->size; j++) {
				NUM* restrict Bvect = B[j];
				register NUM cij = 0;
				for(unsigned int k = 0; k < this->size; k++) {
					cij += Avect[k] * Bvect[k];
				}
				C[i][j] = cij;
			}
		}
	}
};



#endif /* WORKER_IKJ_HPP_ */
