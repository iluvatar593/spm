/*
 * worker_float.hpp
 *
 *  Created on: 05/nov/2014
 *      Author: atzoril
 */

#ifndef WORKER_MIC_HPP_
#define WORKER_MIC_HPP_

#include <ff/node.hpp>
#include "matrixmultiplication_float.hpp"
#include "utils.hpp"

using namespace ff;

class WorkerFloatMic : public ff_node {
public:
	WorkerFloatMic(int n, int oldn, int k, int oldk, int m, int oldm):
		n(n), oldn(oldn), k(k), oldk(oldk), m(m), oldm(oldm){
		_MM_MALLOC(C, float*, sizeof(float)*n*m);
	}
	~WorkerFloatMic() {_MM_FREE(C);}
	void *svc(void *__restrict__ task) {
		taskFloat_t *t = (taskFloat_t *) task;
		float *__restrict__ a = t->a;
		float *__restrict__ b = t->b;
		float aT[k*30] __attribute__ ((aligned(64)));
		for(int i = 0; i < oldn; i+=30) {
			recTranspose<float>(&a[i*k], aT, 30, k, k, 30);
			for(int j = 0; j < oldm; j+=16) {
				MMKernel(aT, &b[j], &C[i*m+j], m, k/TILE);

			}
			/** n/2 can always be divided by 30*/
			//if(i >= n/2) ff_send_out(C);
		}

		return (oldn > n/2) ? &C[n/2] : C;
	}
private:
	int n, oldn, k, oldk, m, oldm;
	float *__restrict__ C;
};


#endif /* WORKER_MIC_HPP_ */
