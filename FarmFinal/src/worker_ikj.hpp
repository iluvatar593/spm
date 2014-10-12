/*
 * worker_ikj.hpp
 *
 *  Created on: 12/ott/2014
 *      Author: alessandro
 */

#include "worker.hpp"

#ifndef WORKER_IKJ_HPP_
#define WORKER_IKJ_HPP_

template <typename NUM>
class IKJWorker: public Worker<NUM> {
public:
	IKJWorker(unsigned int size, unsigned int id, NUM*** outputBuffer):Worker<NUM>(size,id, outputBuffer){}
	~IKJWorker(){}
protected:
	inline void matrixMultiplication(NUM **restrict A, NUM**restrict B, NUM** restrict C) {
		normalMultiplication(A, B, C, this->size);
	}
};



#endif /* WORKER_IKJ_HPP_ */
