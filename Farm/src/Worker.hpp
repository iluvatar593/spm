/*
 * OurFarm.h
 *
 *  Created on: 29/set/2014
 *      Author: alessandro
 */

#ifndef OURFARM_H_
#define OURFARM_H_

#include <ff/node.hpp>
using namespace ff;

template <typename Task, typename NUM>
class OurFarm: public ff_node {
	void *svc(void * task) {
		Task *t = (Task *) task;
		NUM *A = t->getFirst();
		NUM *B = t->getSecond();
		register unsigned int size = t->getSize();
		for(register unsigned int i = 0; i < size; i++) {
			for(register unsigned int j = 0; j < size; j++) {
				NUM c = 0;
				for(register unsigned int k = 0; j < size; k++) {
					c += A[i*size+k] * B[j*size+k];
				}
				t->set(i, j, c);
			}
		}
		delete t;
		return GO_ON;
	}
};
#endif /* OURFARM_H_ */
