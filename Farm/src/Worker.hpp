/*
 * OurFarm.h
 *
 *  Created on: 29/set/2014
 *      Author: alessandro
 */

#ifndef OURFARM_H_
#define OURFARM_H_
#include "Utils.h"
#include "Task.hpp"
#include <ff/node.hpp>
#include <ff/utils.hpp>
#include <stdio.h>
using namespace ff;

template <typename NUM>
class Worker: public ff_node {
public:
	Worker():ff_node(){}
	void *svc(void * task) {
		//printf("Received a task; [%d]\n", this->getCPUId());
		FarmTask<NUM> *t = (FarmTask<NUM> *) task;
		NUM *A = t->getFirst();
		NUM *B = t->getSecond();
		unsigned int size = t->getSize();
		NUM *C = (NUM*) calloc(size*size, sizeof(NUM));
		for(register unsigned int i = 0; i < size; i++) {
			for(register unsigned int k = 0; k < size; k++) {
				for(register unsigned int j = 0; j < size; j++) {
					C[i*size+j] += A[i*size+k] * B[j*size+k];
				}
		}
		}
		delete t;
		delete C;
		return GO_ON;
	}
};
#endif /* OURFARM_H_ */
