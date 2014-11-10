/*
 * worker_double_pedantic.hpp
 *
 *  Created on: 05/nov/2014
 *      Author: atzoril
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

class WorkerDoublePedantic : public ff_node {
public:
	WorkerDoublePedantic(int n, int oldn, int k, int oldk, int m, int oldm, int id):
		n(n), oldn(oldn), k(k), oldk(oldk), m(m), oldm(oldm), ff_node(){
		_MM_MALLOC(C1, double*, sizeof(double)*n*m);
		_MM_MALLOC(C2, double*, sizeof(double)*n*m);
		//_MM_MALLOC(output, workerOutput_t*, sizeof(workerOutput_t));
		output = new workerOutput_t<double>(C1, id, -1);
	}
	~WorkerDoublePedantic() {_MM_FREE(C1); _MM_FREE(C2);}

	void *svc(void *__restrict__ task) {
		double *__restrict__ C;
		if(first)
			C=C1;
		else
			C=C2;
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
		}
		output->matrixChunk = C;
		first = !first;
		return output;
	}
private:
	int n, oldn, k, oldk, m, oldm;
	double *__restrict__ C1;
	double *__restrict__ C2;
	workerOutput_t<double>* output;
	bool first = true;
};


#endif /* WORKER_MIC_HPP_ */
