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
		int *restrict dummyC, *restrict dummyB; //*restrict dummyA

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
	StrassenWorker(unsigned int size, unsigned int id, NUM*** output_buffer, unsigned int threshold):ff_node(),size(size), threshold(threshold) {
			this->_C = output_buffer[2*id];
			this->first = output_buffer[2*id];
			this->second = output_buffer[2*id+1];
			isFirst = true;
			if(size>threshold){
				P1 = new NUM*[size/2]();
				P2 = new NUM*[size/2]();
				P3 = new NUM*[size/2]();
				P4 = new NUM*[size/2]();
				P5 = new NUM*[size/2]();
				P6 = new NUM*[size/2]();
				P7 = new NUM*[size/2]();
				for(int i = 0; i < size/2; i++){
					P1[i] = new NUM[size/2]();
					P2[i] = new NUM[size/2]();
					P3[i] = new NUM[size/2]();
					P4[i] = new NUM[size/2]();
					P5[i] = new NUM[size/2]();
					P6[i] = new NUM[size/2]();
					P7[i] = new NUM[size/2]();
				}
			}

	};

	~StrassenWorker(){};

	void* svc(void * restrict task) {
		pointerTask<NUM> *restrict t = (pointerTask<NUM>*restrict) task;

		NUM **restrict  _A = t->A;
		NUM **restrict  _B = t->B;

		for(register int i = 0; i < size; i++){
			for(register int j = 0; j < size; j++) _C[i][j] = 0;
		}
//		for(int i=0; i<size; i++){
//					printf("c%d ",_C[i][i]);
//				}
		strassenMatrixMultiplication(_A, _B, _C, size, threshold);

//		for(int i=0; i<size; i++){
//			printf("c%d ",_C[i][i]);
//		}
		printf("\n");
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
	NUM **restrict _C, **restrict first, **restrict second;
	bool isFirst;
	unsigned int size,threshold;
	NUM **P1;
	NUM **P2;
	NUM **P3;
	NUM **P4;
	NUM **P5;
	NUM **P6;
	NUM **P7;

	inline void normalMatrixMultiplication(NUM **restrict A, NUM** restrict B, NUM** restrict C, unsigned int size, unsigned int offsetA=0, unsigned int offsetB=0, unsigned int offsetC=0) {
			NUM *restrict Bvector;
			NUM *restrict Cvector;
			for(unsigned int i = 0; i < size; i++) {
				Cvector = &C[i][offsetC];
				for(unsigned int k = 0; k < size; k++) {
					Bvector = &B[k][offsetB];
					register int aik = A[i][k+offsetA];
					for (unsigned int j = 0; j < size; j++)
						Cvector[j] += aik* Bvector[j];
				}
			}
		}

	inline void matrixSum(NUM **restrict A, unsigned int offsetColA, NUM** restrict B, unsigned int offsetColB, NUM**restrict C, unsigned int size) {
			for(int i = 0; i < size; i++){
				NUM *restrict rowA = &A[i][offsetColA], *restrict rowB = &B[i][offsetColB], *restrict rowC = C[i];
				for(int j = 0 ; j < size; j++) {
					rowC[j] = rowA[j] + rowB[j];

				}
//				printf("A[%d][%d] = %d\n", i, i, A[i][i]);
//				printf("C[%d][%d] = %d\n", i, i, C[i][i]);
			}
		}


	inline void matrixSub(NUM **restrict A, unsigned int offsetColA, NUM** restrict B, unsigned int offsetColB, NUM**restrict C, unsigned int size) {
				for(int i = 0; i < size; i++){
					NUM *restrict rowA = &A[i][offsetColA], *restrict rowB = &B[i][offsetColB], *restrict rowC = C[i];
					for(int j = 0; j < size; j++) {
						//if(i == j) printf("%d, %d -- %d %d\n", i, j, rowA[j+offsetColA], rowB[j+offsetColB]);
						rowC[j] = rowA[j] - rowB[j];
					}
				}
			}


	void strassenMatrixMultiplication(NUM **restrict A, NUM** restrict B, NUM**restrict C, unsigned int size, unsigned int offsetA = 0, unsigned int offsetB = 0) {
			if(size <= threshold){ //Stop recursion.
				normalMatrixMultiplication(A, B, C, size, offsetA, offsetB);
				return;
			}
			unsigned int newsize = size/2;
			NUM **S1 = new NUM*[newsize]();
			NUM **S2 = new NUM*[newsize]();
			for(int i = 0; i < newsize; i++) {
				S1[i] = new NUM[newsize]();
				S2[i] = new NUM[newsize]();
			}
			for(int i=0; i<newsize; i++){
						printf("bS1=%d ",S1[i][i]);
			}
			printf("\n");
			printf("------ Current size is %d, offsetA is %d, offsetB is %d-----\n", size, offsetA, offsetB);
			matrixSum(A, offsetA, &A[newsize], offsetA+newsize, S1, newsize); //S1 has been calculated
			for(int i=0; i<newsize; i++){
						printf("aS1=%d ",S1[i][i]);
			}
			printf("\n");
			matrixSum(B, offsetB, &B[newsize], offsetB+newsize, S2, newsize); //S2 has been calculated
			//printf("Until now it seems ok!\n");

			for(int i=0; i<newsize; i++){
						printf("S2=%d ",S2[i][i]);
			}
			normalMatrixMultiplication(S1, S2, P1, newsize); //P1 = S1*S2
			matrixSum(&A[newsize], offsetA, &A[newsize], offsetA+newsize, S1, newsize); //S3
			normalMatrixMultiplication(B, S1, P2, newsize); //P2 calculated
			matrixSub(B, offsetB+newsize, &B[newsize], offsetB+newsize, S2, newsize); //S4
			//printf("Until now it seems ok!\n");
			normalMatrixMultiplication(A, S2, P3, newsize); //P3 calculated, check fi space can be reused
			matrixSub(&B[newsize], offsetB, B, offsetB, S1, newsize); //S5
			normalMatrixMultiplication(&A[newsize], S1, P4, newsize, newsize);
			matrixSum(A, offsetA, A, offsetA+newsize, S2, newsize); //S6
			normalMatrixMultiplication(S2, &B[newsize], P5, newsize, 0, newsize);
			matrixSub(&A[newsize], offsetA, A, offsetA, S1, newsize); //S7
			matrixSum(B, offsetB, B, offsetB+newsize, S2, newsize); //S8
			normalMatrixMultiplication(S1, S2, P6, newsize);
			matrixSub(A, offsetA+newsize, &A[newsize], offsetA+newsize, S1, newsize); //S9
			matrixSum(&B[newsize], offsetB, &B[newsize], offsetB+newsize, S2, newsize); //S10
			normalMatrixMultiplication(S1, S2, P7,newsize);

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
			//printf("Last block...\n");
			for(int i = newsize; i < size; i++) {
				for(int j = newsize; j < size; j++) {
					C[i][j] = P1[i-newsize][j-newsize] - P2[i-newsize][j-newsize] + P3[i-newsize][j-newsize] + P6[i-newsize][j-newsize];
					//printf("C[%d][%d] = %d\n", i, j, C[i][j]);
				}
			}

			for(int i = 0; i < newsize; i++) {
				delete[] S1[i];
				delete[] S2[i];
				for(int j=0; j < newsize; j++){
					P1[i][j] = 0;
					P2[i][j] = 0;
					P3[i][j] = 0;
					P4[i][j] = 0;
					P5[i][j] = 0;
					P6[i][j] = 0;
					P7[i][j] = 0;
				}
				//delete[] P1[i];
				//delete[] P2[i];
				//delete[] P3[i];
				//delete[] P4[i];
				//delete[] P5[i];
				//delete[] P6[i];
				//delete[] P7[i];
			}
			delete[] S1;
			delete[] S2;
			//delete[] P1;
			//delete[] P2;
			//delete[] P3;
			//delete[] P4;
			//delete[] P5;
			//delete[] P6;
			//delete[] P7;
		}
};


#endif /* WORKER_HPP_ */
