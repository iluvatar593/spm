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
#include <chrono>
#include "buffers/matrix.h"
using namespace ff;

template <typename NUM>
class Worker: public ff_node {
public:
	Worker():ff_node(){}
	void *svc(void * task) {
		//auto start = std::chrono::high_resolution_clock::now();
		FarmTask<NUM> *t = (FarmTask<NUM> *) task;
		matrix<NUM> *A = t->getFirst();
		matrix<NUM> *B = t->getSecond();
		unsigned int size = t->getSize();
		matrix<NUM> *C = new simple_matrix<NUM>(size,size);
		for(register unsigned int i = 0; i < size; i++) {
			for(register unsigned int k = 0; k < size; k++) {
				for(register unsigned int j = 0; j < size; j++) {
					(*C)[i][j] += (*A)[i][k] * (*B)[j][k];
				}
		}
		}
		//auto elapsed = std::chrono::high_resolution_clock::now() - start;
		//long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
		//printf("WorkerNoDel:%lld\n", microseconds);
		delete t;
		delete C;
		//elapsed = std::chrono::high_resolution_clock::now() - start;
		//microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
		//printf("WorkerWithDel:%lld\n", microseconds);

		return GO_ON;
	}
};
#endif /* OURFARM_H_ */
