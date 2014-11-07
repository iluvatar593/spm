/*
 * worker_float.hpp
 *
 *  Created on: 05/nov/2014
 *      Author: atzoril
 */

#ifndef WORKER_FLOAT_HPP_
#define WORKER_FLOAT_HPP_

#include <ff/node.hpp>
#include "matrixmultiplication.hpp"
#include "utils.hpp"
#include "utilsF.hpp"

using namespace ff;

#if defined(__MIC__)
#define OFFSET_ROW 30
#define OFFSET_COL 16
#else
#define OFFSET_ROW 8
#define OFFSET_COL 8
#endif

class WorkerFloat : public ff_node {
public:
	WorkerFloat(int n, int oldn, int k, int oldk, int m, int oldm, int id):
		n(n), oldn(oldn), k(k), oldk(oldk), m(m), oldm(oldm), ff_node(){
		_MM_MALLOC(C, float*, sizeof(float)*n*m);
		output = new workerOutput_t<float>(C, id, -1);

	}
	~WorkerFloat() {_MM_FREE(C);}
	void *svc(void *__restrict__ task) {
		taskFloat_t *t = (taskFloat_t *) task;
		float *__restrict__ a = t->a;
		float *__restrict__ b = t->b;
		float aT[k*OFFSET_ROW] __attribute__ ((aligned(ALIGNMENT)));
		output->id = t->num;
		for(int i = 0; i < oldn; i+=OFFSET_ROW) {
			recTranspose<float>(&a[i*k], aT, OFFSET_ROW, k, k, OFFSET_ROW);
			for(int j = 0; j < oldm; j+=OFFSET_COL) {
				Kernel<float>(aT, &b[j], &C[i*m+j], m, k/TILE);

			}
			/** n/2 can always be divided by 30*/
			if(i == n/2) {
				output->matrixChunk = C;
				ff_send_out(output);
			}
		}

		if(oldn > n/2) {output->matrixChunk = &C[n/2];}
		return output;
	}
private:
	int n, oldn, k, oldk, m, oldm;
	float *__restrict__ C;
	workerOutput_t<float> * output;
};

#endif /* WORKER_FLOAT_HPP_ */
