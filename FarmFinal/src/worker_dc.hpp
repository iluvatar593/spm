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
#include "workerOutput.hpp"

template <typename NUM>
class DCWorker: public Worker<NUM> {
public:
	DCWorker(unsigned int size, unsigned int id, NUM*** outputBuffer, unsigned int treshold=256):Worker<NUM>(size,id, outputBuffer),treshold(treshold){}
	~DCWorker(){}
protected:
	inline void* matrixMultiplication(NUM **restrict A, NUM**restrict B, NUM** restrict C) {
		if (this->size <= treshold) {
			NUM *restrict Bvector;
			NUM *restrict Cvector;
			for(unsigned int i = 0; i < this->size; i++) {
				Cvector = C[i];
				for(unsigned int k = 0; k < this->size; k++) {
					Bvector = B[k];
					register NUM aik = A[i][k];
					for (unsigned int j = 0; j < this->size; j++)
							Cvector[j] += aik* Bvector[j];
				}
				if(i == this->size/2) {
					this->ff_send_out((void*) new workerOutput<NUM>(C, this->id));
				}
			}

			return new workerOutput<NUM>(&C[this->size/2], this->id);
		}
		unsigned int size = this->size;
				//printf("First decomposition.\n");
		dcMatrixMultiplication(A, B, C, size/2); //C11'
		dcMatrixMultiplication(A, B, C, size/2, 0, size/2, size/2); //C12
		dcMatrixMultiplication(A, &B[size/2], C, size/2, size/2, 0); //C11 ended
		dcMatrixMultiplication(&A[size/2], &B[size/2], &C[size/2], size/2, size/2, size/2, size/2); //C12 ended

		//Here we can send an half
		this->ff_send_out((void*) new workerOutput<NUM>(C, this->id));

		dcMatrixMultiplication(&A[size/2], B, &C[size/2], size/2); //C21'
		dcMatrixMultiplication(A, &B[size/2], C, size/2, size/2, size/2, size/2); //C21
		dcMatrixMultiplication(&A[size/2], B, &C[size/2], size/2, 0, size/2, size/2); //C22 ended
		dcMatrixMultiplication(&A[size/2], &B[size/2], &C[size/2], size/2, size/2); //C21 += A22 * B21; C21 complete
		return new workerOutput<NUM>(&C[size/2], this->id);
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
