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

	void*svc(void * task) {
		task_t * t = (task_t *) task;
		for(unsigned int i = 0; i < chunkSize; i++) {
			for(unsigned int k = 0; k < size; k++) {
				for(unsigned int j = 0; j < size; j++) {
					t->C[i][j] += t->A[i][j] * t->B[i][j];
				}
			}
		}
	}
private:
	unsigned int chunkSize;
	unsigned int size;
};



#endif /* WORKER_HPP_ */
