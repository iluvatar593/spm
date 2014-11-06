/*
 * worker_double.hpp
 *
 *  Created on: 05/nov/2014
 *      Author: alessandro
 */

#ifndef WORKER_MIC_HPP_
#define WORKER_MIC_HPP_

#include <ff/node.hpp>
#include "matrixmultiplication_double.hpp"
#include "utilsD.hpp"

#if defined(__MIC__)
	#define OFFSET_ROW 30
	#define OFFSET_COL 8
#else
	#define OFFSET ROW 8
	#define OFFSET_COL 4
#endif


using namespace ff;

class WorkerDouble : public ff_node {
public:
	WorkerDouble(int n, int oldn, int k, int oldk, int m, int oldm):
		n(n), oldn(oldn), k(k), oldk(oldk), m(m), oldm(oldm){
		_MM_MALLOC(C, double*, sizeof(double)*n*m);
	}
	~WorkerDouble() {_MM_FREE(C);}
	void *svc(void *__restrict__ task) {

		taskDouble_t *t = (taskDouble_t *) task;
		double *__restrict__ a = t->a;
		double *__restrict__ b = t->b;

		double aT[k*OFFSET_ROW] __attribute__ ((aligned(64)));

		for(int i = 0; i < oldn; i+=OFFSET_ROW) {
			recTranspose<double>(&a[i*k], aT, OFFSET_ROW, k, k, OFFSET_ROW);
			for(int j = 0; j < oldm; j+=OFFSET_MIC) {
				MMKernel(aT, &b[j], &C[i*m+j], m, k/TILE);
			}
			/** n/2 can always be divided by 30*/
			if(i >= n/2) ff_send_out(C);
		}

		return (oldn > n/2) ? &C[n/2] : C;
	}
private:
	int n, oldn, k, oldk, m, oldm;
	double *__restrict__ C;
};


#endif /* WORKER_MIC_HPP_ */
