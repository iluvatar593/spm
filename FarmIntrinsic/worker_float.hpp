/*
 * worker_float.hpp
 *
 *  Created on: 05/nov/2014
 *      Author: atzoril
 */

#ifndef WORKER_FLOAT_HPP_
#define WORKER_FLOAT_HPP_

#include <ff/node.hpp>
#include "matrixmultiplication_float.hpp"
#include "utils.hpp"

using namespace ff;

#if define(__MIC__)
#define OFFSET_ROW 30
#define OFFSET_COL 16
#else
#define OFFSET_ROW 8
#define OFFSET_COL 8
#endif

class WorkerFloat : public ff_node {
public:
	WorkerFloat(int n, int oldn, int k, int oldk, int m, int oldm):
		n(n), oldn(oldn), k(k), oldk(oldk), m(m), oldm(oldm){
		_MM_MALLOC(C, float*, sizeof(float)*n*m);
	}
	~WorkerFloat() {_MM_FREE(C);}
	void *svc(void *__restrict__ task) {
		taskFloat_t *t = (taskFloat_t *) task;
		float *__restrict__ a = t->a;
		float *__restrict__ b = t->b;
		float aT[k*OFFSET_ROW] __attribute__ ((aligned(ALIGNMENT)));
		for(int i = 0; i < oldn; i+=OFFSET_ROW) {
			recTranspose<float>(&a[i*k], aT, OFFSET_ROW, k, k, OFFSET_ROW);
			for(int j = 0; j < oldm; j+=OFFSET_COL) {
				MMKernel(aT, &b[j], &C[i*m+j], m, k/TILE);

			}
			/** n/2 can always be divided by 30*/
			if(i >= oldn/2) ff_send_out(C);
		}

		return (oldn > n/2) ? &C[n/2] : C;
	}
private:
	int n, oldn, k, oldk, m, oldm;
	float *__restrict__ C;
};

#endif /* WORKER_FLOAT_HPP_ */
