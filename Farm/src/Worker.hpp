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
	Worker():ff_node(){
		C = NULL;

	}
	void *svc(void * task) {
		//C = (NUM*) calloc(1024*1024, sizeof(NUM));
		for(int c = 0; c < 1024;c++) C[c] = (NUM*) calloc(1024, sizeof(NUM));
		if(task == NULL) return NULL;

		simple_task<NUM> *t = (simple_task<NUM> *) task;
		NUM** A = t->getFirst();
		NUM** B = t->getSecond();
		unsigned int size = t->getRows();
		start_time();
		for(register unsigned int i = 0; i < size; i++) {
			for(register unsigned int k = 0; k < size; k++) {
				for(register unsigned int j = 0; j < size; j++) {
					//printf("Ready for sum %d\n", i);
					C[i*size+j] += A[i][k] * B[k][j];
					//printf("Ended sum");
				}
			}

		}
		//printf("Terminated \n");
		sleep(1);
		elapsed_time("Worker");
		printf("WID %d\n", this->getCPUId());
		free(C);
		delete t;
		return GO_ON;
	}
private:
	NUM * C;
};
#endif /* OURFARM_H_ */
