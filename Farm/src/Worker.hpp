/*
 * OurFarm.h
 *
 *  Created on: 29/set/2014
 *      Author: alessandro
 */

#ifndef OURFARM_H_
#define OURFARM_H_
#include "Utils.h"
#include "buffers/Task.hpp"
#include <ff/node.hpp>
#include <ff/utils.hpp>
#include <stdio.h>
#include "buffers/matrix.h"
using namespace ff;

template <typename NUM>
class Worker: public ff_node {
public:
	Worker():ff_node(){}
	void *svc(void * task) {
		printf("Worker received\n");
		start_time();
		FarmTask<NUM> *t = (FarmTask<NUM> *) task;
		simple_matrix<NUM> *A = (simple_matrix<NUM>*)t->getFirst();
		simple_matrix<NUM> *B = (simple_matrix<NUM>*)t->getSecond();
		unsigned int size = A->getRows(); //TODO: Fix
		simple_matrix<NUM> *C = new simple_matrix<NUM>(size,size);

		for(register unsigned int i = 0; i < size; i++) {
			for(register unsigned int k = 0; k < size; k++) {
				for(register unsigned int j = 0; j < size; j++) {
					printf("Ready for sum %d\n", i);
					(*C)[i][j] += (*A)[i][k] * (*B)[j][k];
					printf("Ended sum");
				}
			}

		}

		delete t;
		delete C;
		elapsed_time("Worker");

		return GO_ON;
	}
};
#endif /* OURFARM_H_ */
