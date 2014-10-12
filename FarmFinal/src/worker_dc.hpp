/*
 * worker_dc.hpp
 *
 *  Created on: 10/ott/2014
 *      Author: alessandro
 */

#ifndef WORKER_DC_HPP_
#define WORKER_DC_HPP_

#include <ff/node.hpp>
#include "worker.hpp"

template <typename NUM>
class DCWorker: public Worker<NUM> {
public:
	DCWorker(unsigned int size, unsigned int id, NUM*** outputBuffer, unsigned int treshold=256):Worker<NUM>(size,id, outputBuffer),treshold(treshold){}
	~DCWorker(){}
protected:
	inline void matrixMultiplication(NUM **restrict A, NUM**restrict B, NUM** restrict C) {
		dcMatrixMultiplication(A, B, C, this->size);
	}
private:
	unsigned int treshold;
	void dcMatrixMultiplication(NUM **restrict A, NUM** restrict B, NUM** restrict C, unsigned int size, unsigned int offsetA = 0, unsigned int offsetB = 0, unsigned int offsetC = 0) {
		if (size <= treshold) {
			this->normalMatrixMultiplication(A, B, C, size, offsetA, offsetB, offsetC);
			return;
		}
		//printf("First decomposition.\n");
		dcMatrixMultiplication(A, B, C, size/2, offsetA, offsetB, offsetC); //C11 = A11*B11
		dcMatrixMultiplication(&A[size/2], B, &C[size/2], size/2, offsetA, offsetB, offsetC);
		dcMatrixMultiplication(&A[size/2], B, &C[size/2], size/2, offsetA, offsetB+size/2, offsetC+size/2); //C22 ended
		dcMatrixMultiplication(A, B, C, size/2, offsetA, offsetB+size/2, offsetC+size/2);

		dcMatrixMultiplication(A, &B[size/2], C, size/2, offsetA+size/2, offsetB, offsetC); //C11 += A12* B21 [ok]
		dcMatrixMultiplication(&A[size/2], &B[size/2], &C[size/2], size/2, offsetA+size/2, offsetB, offsetC); //C21 += A22 * B21; C21 complete
		dcMatrixMultiplication(&A[size/2], &B[size/2], &C[size/2], size/2, offsetA+size/2, offsetB+size/2, offsetC+size/2);
		dcMatrixMultiplication(A, &B[size/2], C, size/2, offsetA+size/2, offsetB+size/2, offsetC+size/2); //C12 ended
		 //C22 = A22*B22

	}
};

#endif /* WORKER_DC_HPP_ */