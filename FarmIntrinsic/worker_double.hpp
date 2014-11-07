/*
 * worker_double.hpp
 *
 *  Created on: 05/nov/2014
 *      Author: alessandro
 */

#ifndef WORKER_MIC_HPP_
#define WORKER_MIC_HPP_

#include <ff/node.hpp>
#include "matrixmultiplication.hpp"
#include "utilsD.hpp"

#if defined(__MIC__)
	#define OFFSET_ROW 30
	#define OFFSET_COL 8
#else
	#define OFFSET_ROW 8
	#define OFFSET_COL 4
#endif


using namespace ff;

class WorkerDouble : public ff_node {
public:
	WorkerDouble(int n, int oldn, int k, int oldk, int m, int oldm, int id):
		n(n), oldn(oldn), k(k), oldk(oldk), m(m), oldm(oldm), ff_node(){
		_MM_MALLOC(C, double*, sizeof(double)*n*m);
		//_MM_MALLOC(output, workerOutput_t*, sizeof(workerOutput_t));
		output = new workerOutput_t<double>(C, id, -1);
	}
	~WorkerDouble() {_MM_FREE(C);}

	void *svc(void *__restrict__ task) {

		taskDouble_t *t = (taskDouble_t *) task;
		double *__restrict__ a = t->a;
		double *__restrict__ b = t->b;
		output->id = t->num;
		double aT[k*OFFSET_ROW] __attribute__ ((aligned(64)));

		for(int i = 0; i < oldn; i+=OFFSET_ROW) {
			recTranspose<double>(&a[i*k], aT, OFFSET_ROW, k, k, OFFSET_ROW);
			for(int j = 0; j < oldm; j+=OFFSET_COL) {
				Kernel<double>(aT, &b[j], &C[i*m+j], m, k/TILE);
			}
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
	double *__restrict__ C;
	workerOutput_t<double>* output;
};


#endif /* WORKER_MIC_HPP_ */
