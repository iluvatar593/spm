/*
 * worker.hpp
 *
 *  Created on: 13/ott/2014
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
	Worker(unsigned int chunkSize, unsigned int size):chunkSize(chunkSize),size(size) {
	}
	~Worker(){}

	void*svc(void *restrict task) {
		task_t<NUM> * t = (task_t<NUM> *) task;
		NUM **restrict C = t->C;
		NUM **restrict A = t->A;
		NUM **restrict B = t->B;
		for(unsigned int i = 0; i < chunkSize; i++) {
			for(unsigned int k = 0; k < size; k++) {
				for(unsigned int j = 0; j < size; j++) {
					C[i][j] += A[i][k] * B[k][j];
				}
			}
		}
		delete t;
		return GO_ON;
	}
private:
	unsigned int chunkSize;
	unsigned int size;
};



#endif /* WORKER_HPP_ */
