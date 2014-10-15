/*
 * worker_ikj.hpp
 *
 *  Created on: 12/ott/2014
 *      Author: alessandro
 */

#include "worker.hpp"
#include "workerOutput.hpp"
#ifndef WORKER_IKJ_HPP_
#define WORKER_IKJ_HPP_

using namespace ff;

template <typename NUM>
class IKJWorker: public Worker<NUM> {
public:
	IKJWorker(unsigned int size, unsigned int id, NUM*** outputBuffer):Worker<NUM>(size,id, outputBuffer){}
	~IKJWorker(){}
protected:
	inline void* matrixMultiplication(NUM **restrict A, NUM**restrict B, NUM** restrict C) {
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

		//this->normalMatrixMultiplication(A, B, C, this->size);
		//matMul(A,B,C);
	}
	/*inline void matMul(NUM**restrict A, NUM**restrict B, NUM**restrict C) {
		for(unsigned int i = 0; i < this->size; i++) {
			NUM* restrict Avect = A[i];
			for(unsigned int j = 0; j < this->size; j++) {
				NUM* restrict Bvect = B[j];
				register NUM cij = 0;
				for(unsigned int k = 0; k < this->size; k++) {
					cij += Avect[k] * Bvect[k];
				}
				C[i][j] = cij;
			}
		}
	}*/
};
/*
template <typename NUM>
class IKJWorker: public ff_node {
public:
       	IKJWorker(unsigned int size, unsigned int id, NUM*** C):ff_node(),size(size) {
                this->_C = C[id];
                //first = C[2*id];
                //second = C[2*id+1];
                //isFirst = true;
                this->id = id;
                //printf("Worker initialized \n");
        };
	~IKJWorker(){};
        void* svc(void * restrict task) {
                //Clean up the matrix.
                for(register int i = 0; i < size; i++){
                        for(register int j = 0; j < size; j++) _C[i][j] = 0;
                }
               pointerTask<NUM> *restrict t = (pointerTask<NUM>*restrict) task;
               NUM **restrict  _A = t->A;
               NUM **restrict  _B = t->B;
               NUM *restrict dummyA, *restrict dummyC, *restrict dummyB;
			   for(register unsigned int i = 0; i < size; i++){
					   dummyC = _C[i];
				   	   for(register unsigned int k = 0; k < size; k++){
							   //dummyC = _C[i];
							   dummyB = _B[k];
							   register int aik = _A[i][k];
							   for(register unsigned int j = 0; j < size; j++){
									   dummyC[j] += aik*dummyB[j];
							   }
					   }
					   if(i == size/2) {
							   ff_send_out((void*) new workerOutput<NUM>(_C, id));
					   }
			   }
			   return new workerOutput<NUM>(&_C[size/2], id);
        }
private:
        unsigned int size, id;
        NUM**restrict _C;
};
*/
#endif /* WORKER_IKJ_HPP_ */
