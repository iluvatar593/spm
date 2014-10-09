/*
 * worker.hpp
 *
 *  Created on: 07/ott/2014
 *      Author: alessandro
 */

#ifndef WORKER_HPP_
#define WORKER_HPP_

#include <ff/node.hpp>
#include <cmath>
using namespace ff;


template <typename NUM>
class Worker: public ff_node {
public:
	Worker(unsigned int size, unsigned int id, NUM*** output_buffer):ff_node(),size(size) {
		this->_C = output_buffer[2*id];
		this->first = output_buffer[2*id];
		this->second = output_buffer[2*id+1];
		isFirst = true;
	};
	~Worker(){};
	void* svc(void * restrict task) {
		//Clean up the matrix.
		for(register int i = 0; i < size; i++){
			for(register int j = 0; j < size; j++) _C[i][j] = 0;
		}
		pointerTask<NUM> *restrict t = (pointerTask<NUM>*restrict) task;

		NUM **restrict  _A = t->A;
		NUM **restrict  _B = t->B;
		int *restrict dummyA, *restrict dummyC, *restrict dummyB;

		for(register unsigned int i = 0; i < size; i++){
			for(register unsigned int k = 0; k < size; k++){
				dummyC = _C[i];
				dummyB = _B[k];
				register int aik = _A[i][k];
				for(register unsigned int j = 0; j < size; j++){
					dummyC[j] += aik*dummyB[j];
				}
			}
		}
		if(isFirst) {
			_C = second;
			isFirst = false;
			return first;
		} else {
			_C = first;
			isFirst = true;
			return second;
		}

		return _C;
	}
private:
	unsigned int size;
	NUM **restrict first, **restrict second;
	NUM **restrict _C;
	bool isFirst;
};

template <typename NUM>
class StrassenWorker: public ff_node {
public:
	StrassenWorker(unsigned int size, unsigned int id, NUM*** output_buffer):ff_node(),size(size) {
			this->C = output_buffer[2*id];
			this->first = output_buffer[2*id];
			this->second = output_buffer[2*id+1];
			isFirst = true;
	};
	~StrassenWorker(){};
	void* svc(void * restrict task) {

		return NULL;
	}
private:
	NUM **restrict C, **restrict first, **restrict second;
	bool isFirst;
	unsigned int treshold, size;
	unsigned int nextPowerOfTwo(unsigned int size) {
		 return pow(2, int(ceil(log2(size))));
	}

	//C must be initialized to 0 in all its elements
	void normalMatrixMultiplication(NUM ** restrict A, NUM** restrict B, NUM** restrict C, unsigned int size, unsigned int offsetA=0, unsigned int offsetB=0) {
		NUM *restrict Bvector;
		NUM *restrict Cvector;
		for(unsigned int i = offsetA; i < size; i++) {
			Cvector = C[i];
			for(unsigned int k = 0; k < size; k++) {
				Bvector = B[k];
				register int aik = A[i][k];
				for (unsigned int j = offsetB; j < size; j++)
					Cvector[j-offsetB] += aik* Bvector[j];
			}
		}
	}

	void matrixSum(NUM **restrict A, unsigned int offsetColA, NUM** restrict B, unsigned int offsetColB, NUM**restrict C, unsigned int size) {
		for(int i = offsetColA; i < size; i++){
			NUM **restrict rowA = A[i], **restrict rowB = B[i], **restrict rowC = C[i];
			for(int j = offsetColB; j < size; j++) {
				rowC[j] = rowA[j] + rowB[j];
			}
		}
	}

	void matrixSub(NUM **restrict A, unsigned int offsetColA, NUM** restrict B, unsigned int offsetColB, NUM**restrict C, unsigned int size) {
			for(int i = offsetColA; i < size; i++){
				NUM **restrict rowA = A[i], **restrict rowB = B[i], **restrict rowC = C[i];
				for(int j = offsetColB; j < size; j++) {
					rowC[j] = rowA[j] - rowB[j];
				}
			}
		}

	void strassenMatrixMultiplication(NUM **restrict A, NUM** restrict B, NUM**restrict C, unsigned int size, unsigned int offsetA = 0, unsigned int offsetB = 0) {
		if(size <= treshold){ //Stop recursion.
			normalMatrixMultiplication(A, B, C, size, offsetA, offsetB);
			return;
		}
		unsigned int newsize = size/2;
		NUM **S1 = new NUM*[newsize]();
		for(int i = 0; i < newsize; i++) S1[i] = new NUM[newsize]();
		matrixSum(A, offsetA, &A[newsize], offsetA+newsize, S1, newsize); //S1 has been calculated

		NUM **S2 = new NUM*[newsize]();
		for(int i = 0; i < newsize; i++) S2[i] = new NUM[newsize]();
		matrixSum(B, offsetB, &B[newsize], offsetB+newsize, S2, newsize); //S2 has been calculated

		NUM**P1 = new NUM*[newsize]();
		for(int i = 0; i < newsize; i++) P1[i] = new NUM[newsize]();

		strassenMatrixMultiplication(S1, S2, P1, newsize); //P1 = S1*S2

		matrixSum(&A[newsize], offsetA, &A[newsize], offsetA+newsize, S1, newsize); //S3
		NUM **P2 = new NUM*[newsize]();
		for(int i = 0; i < newsize; i++) P2[i] = new NUM[newsize](); //Allocation of new space for s2, C can be reused but check.

		strassenMatrixMultiplication(B, S1, P2, newsize); //P2 calculated

		matrixSub(B, offsetB+newsize, &B[newsize], offsetB+newsize, S2, newsize); //S4

		NUM**P3 = new NUM*[newsize]();
		for(int i = 0;i < newsize; i++) P3[i] = new NUM[newsize]();

		strassenMatrixMultiplication(A, S2, P3, newsize); //P3 calculated, check fi space can be reused

		matrixSub(&B[newsize], offsetB+newsize, B, offsetB, S1, newsize); //S5
		NUM**P4 = new NUM*[newsize]();
		for(int i = 0;i < newsize; i++) P4[i] = new NUM[newsize]();
		strassenMatrixMultiplication(&A[newsize], S1, P4, newsize, newsize);

		matrixSum(A, offsetA, A, offsetA+newsize, S2, newsize); //S6
		NUM**P5 = new NUM*[newsize]();
		for(int i = 0;i < newsize; i++) P5[i] = new NUM[newsize]();
		strassenMatrixMultiplication(S2, &B[newsize], P5, newsize, 0, newsize);

		matrixSub(&A[newsize], offsetA, A, offsetA, S1, newsize); //S7

		matrixSum(B, offsetB, B, offsetB+newsize, S2, newsize); //S8

		NUM**P6 = new NUM*[newsize]();
		for(int i = 0;i < newsize; i++) P6[i] = new NUM[newsize]();

		strassenMatrixMultiplication(S1, S2, P6, newsize);

		matrixSub(A, offsetA+newsize, &A[newsize], offsetA+newsize, S1, newsize); //S9
		matrixSum(&B[newsize], offsetB, &B[newsize], offsetB+newsize, S2, newsize); //S10

		NUM**P7 = new NUM*[newsize]();
		for(int i = 0; i < newsize; i++) P7[i] = new NUM[newsize]();

		strassenMatrixMultiplication(S1, S2, P7,newsize);

		//Recombine all this stuff
		//C11
		for(int i = 0; i < newsize; i++){
			for(int j = 0; j < newsize; j++) {
				C[i][j] = P1[i][j] + P4[i][j] - P5[i][j] + P7[i][j];
			}
		}
		for(int i = 0; i < newsize; i++) {
			for(int j = newsize; j < size; j++) {
				C[i][j] = P3[i][j-newsize] + P5[i][j-newsize];
			}
		}
		for(int i = newsize; i < size; i++) {
			for(int j = 0; j < newsize; j++) {
				C[i][j] = P2[i-newsize][j] + P4[i-newsize][j];
			}
		}

		for(int i = newsize; i < size; i++) {
			for(int j = newsize; j < newsize; j++) {
				C[i][j] = P1[i-newsize][j-newsize] - P2[i-newsize][j-newsize] + P3[i-newsize][j-newsize] + P6[i-newsize][j-newsize];
			}
		}


	}
};


#endif /* WORKER_HPP_ */
