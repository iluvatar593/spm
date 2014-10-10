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
		NUM *restrict dummyC, *restrict dummyB; //*restrict dummyA

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
/** TODO: eliminate 4 Pi to optimize space */
template <typename NUM>
class StrassenWorker:public ff_node {
public:
	StrassenWorker(unsigned int size, unsigned int id, NUM*** outputBuffer):msize(size){
		initializeStrassenMatrices(size);
		first = outputBuffer[2*id];
		second = outputBuffer[2*id+1];
		_C = first;
	};
	~StrassenWorker(){
		for(int i = 0; i < msize; i++) {
			delete[] first[i];
			delete[] second[i];
		}
		for(int i = 0; i < msize/2; i++) {
			delete[] P1[i];
			delete[] P2[i];
			delete[] P3[i];
			delete[] P4[i];
			delete[] P5[i];
			delete[] P6[i];
			delete[] P7[i];
			delete[] S1[i];
			delete[] S2[i];
		}
		delete[] first;
		delete[] second;
		delete[] P1;
		delete[] P2;
		delete[] P3;
		delete[] P4;
		delete[] P5;
		delete[] P6;
		delete[] P7;
		delete[] S1;
		delete[] S2;
	}

	void* svc(void * restrict task) {
		//Clean up the matrix.
		for(register int i = 0; i < msize; i++){
			for(register int j = 0; j < msize; j++) _C[i][j] = 0;
		}
		pointerTask<NUM> *restrict t = (pointerTask<NUM>*restrict) task;
		NUM **restrict  A = t->A;
		NUM **restrict  B = t->B;
		unsigned int newsize = msize/2;
		unsigned int offsetA = 0, offsetB = 0;
		NUM** restrict C = _C;
		//printf("------ Current size is %d, offsetA is %d, offsetB is %d-----\n", size, offsetA, offsetB);
		matrixSum(A, offsetA, &A[newsize], offsetA+newsize, S1, newsize); //S1 has been calculated
		matrixSum(B, offsetB, &B[newsize], offsetB+newsize, S2, newsize); //S2 has been calculated
		//printf("Until now it seems ok!\n");
		//NUM**P1 = new NUM*[newsize]();
		//for(int i = 0; i < newsize; i++) P1[i] = new NUM[newsize]();
		normalMatrixMultiplication(S1, S2, P1, newsize); //P1 = S1*S2
		matrixSum(&A[newsize], offsetA, &A[newsize], offsetA+newsize, S1, newsize); //S3
		//NUM **P2 = new NUM*[newsize]();
		//for(int i = 0; i < newsize; i++) P2[i] = new NUM[newsize](); //Allocation of new space for s2, C can be reused but check.
		//P2 = S3 * B11
		normalMatrixMultiplication(S1, B, P2, newsize); //P2 calculated
		matrixSub(B, offsetB+newsize, &B[newsize], offsetB+newsize, S2, newsize); //S4

				//NUM**P3 = new NUM*[newsize]();
				//for(int i = 0;i < newsize; i++) P3[i] = new NUM[newsize]();
				//printf("Until now it seems ok!\n");
		normalMatrixMultiplication(A, S2, P3, newsize); //P3 calculated, check fi space can be reused
		matrixSub(&B[newsize], offsetB, B, offsetB, S1, newsize); //S5
			//NUM**P4 = new NUM*[newsize]();
			//for(int i = 0;i < newsize; i++) P4[i] = new NUM[newsize]();
		normalMatrixMultiplication(&A[newsize], S1, P4, newsize, newsize);
		matrixSum(A, offsetA, A, offsetA+newsize, S2, newsize); //S6
	//			NUM**P5 = new NUM*[newsize]();
	//			for(int i = 0;i < newsize; i++) P5[i] = new NUM[newsize]();
		normalMatrixMultiplication(S2, &B[newsize], P5, newsize, 0, newsize);
		matrixSub(&A[newsize], offsetA, A, offsetA, S1, newsize); //S7
		matrixSum(B, offsetB, B, offsetB+newsize, S2, newsize); //S8
		//			NUM**P6 = new NUM*[newsize]();
		//			for(int i = 0;i < newsize; i++) P6[i] = new NUM[newsize]();
		normalMatrixMultiplication(S1, S2, P6, newsize);
		matrixSub(A, offsetA+newsize, &A[newsize], offsetA+newsize, S1, newsize); //S9
		matrixSum(&B[newsize], offsetB, &B[newsize], offsetB+newsize, S2, newsize); //S10
//			NUM**P7 = new NUM*[newsize]();
		//			for(int i = 0; i < newsize; i++) P7[i] = new NUM[newsize]();
		normalMatrixMultiplication(S1, S2, P7,newsize);
				//C11
		for(int i = 0; i < newsize; i++){
			#pragma ivdep
			for(int j = 0; j < newsize; j++) {
				C[i][j] = P1[i][j] + P4[i][j] -P5[i][j] + P7[i][j];
			}
		}
			//C12
		for(int i = 0; i < newsize; i++) {
			NUM*restrict Ccol = &C[i][newsize];
			#pragma ivdep
			for(int j = 0; j < newsize; j++) {
				Ccol[j] = P3[i][j] + P5[i][j];
			}
		}
				//C21 (some error in P2 or P4)
		NUM **Ctmp = &C[newsize];
		for(int i = 0; i < newsize; i++) {
			#pragma ivdep
			for(int j = 0; j < newsize; j++) {
				C[i][j] = P2[i][j] + P4[i][j];
			}
		}
				//C22
				//printf("Last block...\n");
		for(int i = 0; i < newsize; i++) {
			NUM *Ccol = &C[i][newsize];
			#pragma ivdep
			for(int j = 0; j < newsize; j++) {
				C[i][j] = P1[i][j] - P2[i][j] + P3[i][j] + P6[i][j];
					//printf("C[%d][%d] = %d\n", i, j, C[i][j]);
			}
		}
		cleanUp();
		//this->strassenMatrixMultiplication(_A, _B, _C, msize);
		//Changes _C and returns the correct matrix
		_C = (_C == first) ? second : first;
		return (_C == first) ? second : first;
	}

private:
	NUM **restrict S1, **restrict S2, **restrict P1, **restrict P2, **restrict P3, **restrict P4, **restrict P5, **restrict P6, **restrict P7, **restrict first, **restrict second, **_C;
	unsigned int msize;
	/** Cleans the temporary area used for strassen procedure */
	inline void cleanUp(){
		for(int i = 0; i < msize/2; i++){
			for(int j = 0; j < msize/2; j++) {
				P1[i][j] = P2[i][j] = P3[i][j] = P4[i][j] = P5[i][j] = P6[i][j] = P7[i][j] = 0;
			}
		}
	}
	/** Initializes the temporary areas used by strassen algorithm */
	inline void initializeStrassenMatrices(unsigned int size) {
		S1 = new NUM*[size/2]();
				S2 = new NUM*[size/2]();
				P1 = new NUM*[size/2]();
				P2 = new NUM*[size/2]();
				P3 = new NUM*[size/2]();
				P4 = new NUM*[size/2]();
				P5 = new NUM*[size/2]();
				P6 = new NUM*[size/2]();
				P7 = new NUM*[size/2]();
				for(int i = 0; i < size/2; i++) {
					S1[i] = new NUM[size/2]();
					S2[i] = new NUM[size/2]();
					P1[i] = new NUM[size/2]();
					P1[i] = new NUM[size/2]();
					P2[i] = new NUM[size/2]();
					P3[i] = new NUM[size/2]();
					P4[i] = new NUM[size/2]();
					P5[i] = new NUM[size/2]();
					P6[i] = new NUM[size/2]();
					P7[i] = new NUM[size/2]();
				}
	}
	/** Performs the normal matrix multiplication on a subchunk */
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

	/** Sum of two matrices */
	inline void matrixSum(NUM **restrict A, unsigned int offsetColA, NUM** restrict B, unsigned int offsetColB, NUM**restrict C, unsigned int size) {
		for(int i = 0; i < size; i++){
			NUM *restrict rowA = &A[i][offsetColA], *restrict rowB = &B[i][offsetColB], *restrict rowC = C[i];
			for(int j =0 ; j < size; j++) {
				rowC[j] = rowA[j] + rowB[j];
			}
		}
	}
	/** Subtraction of two matrices */
	inline void matrixSub(NUM **restrict A, unsigned int offsetColA, NUM** restrict B, unsigned int offsetColB, NUM**restrict C, unsigned int size) {
		for(int i = 0; i < size; i++){
			NUM *restrict rowA = &A[i][offsetColA], *restrict rowB = &B[i][offsetColB], *restrict rowC = C[i];
			for(int j = 0; j < size; j++) {
				//if(i == j) printf("%d, %d -- %d %d\n", i, j, rowA[j+offsetColA], rowB[j+offsetColB]);
				rowC[j] = rowA[j] - rowB[j];
			}
		}
	}
	/** Strassen matrix multplication */
	inline void strassenMatrixMultiplication(NUM **restrict A, NUM** restrict B, NUM**restrict C, unsigned int size, unsigned int offsetA = 0, unsigned int offsetB = 0) {
		unsigned int newsize = size/2;

		//printf("------ Current size is %d, offsetA is %d, offsetB is %d-----\n", size, offsetA, offsetB);
		matrixSum(A, offsetA, &A[newsize], offsetA+newsize, S1, newsize); //S1 has been calculated
		matrixSum(B, offsetB, &B[newsize], offsetB+newsize, S2, newsize); //S2 has been calculated
		//printf("Until now it seems ok!\n");
		//NUM**P1 = new NUM*[newsize]();
		//for(int i = 0; i < newsize; i++) P1[i] = new NUM[newsize]();

		normalMatrixMultiplication(S1, S2, P1, newsize); //P1 = S1*S2
		matrixSum(&A[newsize], offsetA, &A[newsize], offsetA+newsize, S1, newsize); //S3

		//NUM **P2 = new NUM*[newsize]();
		//for(int i = 0; i < newsize; i++) P2[i] = new NUM[newsize](); //Allocation of new space for s2, C can be reused but check.
		//P2 = S3 * B11
		normalMatrixMultiplication(S1, B, P2, newsize); //P2 calculated
		matrixSub(B, offsetB+newsize, &B[newsize], offsetB+newsize, S2, newsize); //S4



		//NUM**P3 = new NUM*[newsize]();
		//for(int i = 0;i < newsize; i++) P3[i] = new NUM[newsize]();
		//printf("Until now it seems ok!\n");
		normalMatrixMultiplication(A, S2, P3, newsize); //P3 calculated, check fi space can be reused
		matrixSub(&B[newsize], offsetB, B, offsetB, S1, newsize); //S5

		//NUM**P4 = new NUM*[newsize]();
		//for(int i = 0;i < newsize; i++) P4[i] = new NUM[newsize]();
		normalMatrixMultiplication(&A[newsize], S1, P4, newsize, newsize);
		matrixSum(A, offsetA, A, offsetA+newsize, S2, newsize); //S6
//			NUM**P5 = new NUM*[newsize]();
//			for(int i = 0;i < newsize; i++) P5[i] = new NUM[newsize]();
		normalMatrixMultiplication(S2, &B[newsize], P5, newsize, 0, newsize);
		matrixSub(&A[newsize], offsetA, A, offsetA, S1, newsize); //S7
		matrixSum(B, offsetB, B, offsetB+newsize, S2, newsize); //S8
//			NUM**P6 = new NUM*[newsize]();
//			for(int i = 0;i < newsize; i++) P6[i] = new NUM[newsize]();
		normalMatrixMultiplication(S1, S2, P6, newsize);
		matrixSub(A, offsetA+newsize, &A[newsize], offsetA+newsize, S1, newsize); //S9
		matrixSum(&B[newsize], offsetB, &B[newsize], offsetB+newsize, S2, newsize); //S10
//			NUM**P7 = new NUM*[newsize]();
//			for(int i = 0; i < newsize; i++) P7[i] = new NUM[newsize]();
		normalMatrixMultiplication(S1, S2, P7,newsize);
		//C11
		for(int i = 0; i < newsize; i++){
			for(int j = 0; j < newsize; j++) {
				C[i][j] = P1[i][j] + P4[i][j] - P5[i][j] + P7[i][j];
			}
		}
		//C12
		for(int i = 0; i < newsize; i++) {
			for(int j = newsize; j < size; j++) {
				C[i][j] = P3[i][j-newsize] + P5[i][j-newsize];
			}
		}
		//C21 (some error in P2 or P4)
		for(int i = newsize; i < size; i++) {
			for(int j = 0; j < newsize; j++) {
				C[i][j] = P2[i-newsize][j] + P4[i-newsize][j];
			}
		}
		//C22
		//printf("Last block...\n");
		for(int i = newsize; i < size; i++) {
			for(int j = newsize; j < size; j++) {
				C[i][j] = P1[i-newsize][j-newsize] - P2[i-newsize][j-newsize] + P3[i-newsize][j-newsize] + P6[i-newsize][j-newsize];
				//printf("C[%d][%d] = %d\n", i, j, C[i][j]);
			}
		}
		cleanUp();
	}
};



#endif /* WORKER_HPP_ */
