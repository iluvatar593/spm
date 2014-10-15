/*
 * worker_s.hpp
 *
 *  Created on: 12/ott/2014
 *      Author: atzoril
 */

#ifndef WORKER_S_HPP_
#define WORKER_S_HPP_

#include <ff/node.hpp>
#include "worker.hpp"

template <typename NUM>
class SWorker: public Worker<NUM> {
public:
	SWorker(unsigned int size, unsigned int id, NUM*** outputBuffer):Worker<NUM>(size, id, outputBuffer){
		initializeOptimizedStrassenMatrices(size);
	}
	~SWorker(){
		for(int i = 0; i < this->size/2; i++) {
			delete[] P1[i];
			delete[] P4[i];
			delete[] P5[i];
			delete[] S1[i];
			delete[] S2[i];
		}
		delete[] P1;
		delete[] P4;
		delete[] P5;
		delete[] S1;
		delete[] S2;
	}
protected:
	inline void * matrixMultiplication(NUM **restrict A, NUM**restrict B, NUM** restrict C) {
		return strassenOptimizedMatrixMultiplication(A, B, C, this->size);
	}

private:
	NUM **restrict S1, **restrict S2, **restrict P1, **restrict P4, **restrict P5;

	inline void initializeOptimizedStrassenMatrices(unsigned int size) {
		S1 = new NUM*[size/2]();
		S2 = new NUM*[size/2]();
		P1 = new NUM*[size/2]();
		P4 = new NUM*[size/2]();
		P5 = new NUM*[size/2]();
		for(int i = 0; i < size/2; i++) {
			S1[i] = new NUM[size/2]();
			S2[i] = new NUM[size/2]();
			P1[i] = new NUM[size/2]();
			P1[i] = new NUM[size/2]();
			P4[i] = new NUM[size/2]();
			P5[i] = new NUM[size/2]();
		}
	}

	inline void cleanUp(){
		for(int i = 0; i < this->size/2; i++){
			for(int j = 0; j < this->size/2; j++) {
				P1[i][j] = P4[i][j] = P5[i][j] = 0;
			}
		}
	}

	/** Sum of two matrices */
	inline void matrixSum(NUM **restrict A, unsigned int offsetColA, NUM** restrict B, unsigned int offsetColB, NUM**restrict C, unsigned int size, unsigned int offsetColC=0) {
		for(int i = 0; i < size; i++){
			NUM *restrict rowA = &A[i][offsetColA], *restrict rowB = &B[i][offsetColB], *restrict rowC = C[i];
			for(int j =0 ; j < size; j++) {
				rowC[j] = rowA[j] + rowB[j];
			}
		}
	}

	/** Subtraction of two matrices */
	inline void matrixSub(NUM **restrict A, unsigned int offsetColA, NUM** restrict B, unsigned int offsetColB, NUM**restrict C, unsigned int size, unsigned int offsetColC=0) {
		for(int i = 0; i < size; i++){
			NUM *restrict rowA = &A[i][offsetColA], *restrict rowB = &B[i][offsetColB], *restrict rowC = C[i];
			for(int j = 0; j < size; j++) {
				rowC[j] = rowA[j] - rowB[j];
			}
		}
	}

	inline void * strassenOptimizedMatrixMultiplication(NUM **restrict A, NUM** restrict B, NUM**restrict C, unsigned int size, unsigned int offsetA = 0, unsigned int offsetB = 0) {
		/*
		 * Strassen algorithm:
		 *
		 *  S1  = A11 + A22
		 *  S2  = B11 + B22
		 *  P1  = S1 * S2
		 *  S3  = A21 + A22
		 *  P2  = S3 * B11
		 *  S4  = B12 - B22
		 *  P3  = A11 * S4
		 *  S5  = B21 - B11
		 *  P4  = A22 * S5
		 *  S6  = A11 + A12
		 *  P5  = S6 * B22
		 *  S7  = A21 - A11
		 *  S8  = B11 + B12
		 *  P6  = S7 * S8
		 *  S9  = A12 - A22
		 *  S10 = B21 + B22
		 *  P7  = S9*S10
		 *  C11 = P1 + P4 - P5 + P7
		 *  C12 = P3 + P5
		 *  C21 = P2 + P4
		 *  C22 = P1 - P2 + P3 + P6
		 *
		 *
		 * Memory usage optimizations, reordering for sending out:		 *
		 *	P2  = C21;
		 *	P3  = C12;
		 *	P6  = C22;
		 *	P7  = C11;
		 *	-----------------------
		 *
		 *	Optimized algorithm
		 *
		 *  P1  = A11 + A22 * B11 + B22
		 *  P4  = A22 * B21 - B11
		 *  C21 = A21 + A22 * B11
		 *  C12 = A11 * B12 - B22
		 *  C22 = A21 - A11 * B11 + B12
		 *  C22 += P1 - C21 + C12
		 *  C21 += P4
		 *
		 *  P5  = A11 + A12 * B22
		 *  C11 = A12 - A22 * B21 + B22
		 *  C11 += P1 + P4 - P5
		 *  C12 += P5
		 *
		 *
		 *
		 */

		unsigned int newsize = size/2;
		matrixSum(A, offsetA, &A[newsize], offsetA+newsize, S1, newsize); //A11 + A22
		matrixSum(B, offsetB, &B[newsize], offsetB+newsize, S2, newsize); //B11 + B22
		this->normalMatrixMultiplication(S1, S2, P1, newsize); //P1  = A11 + A22 * B11 + B22
		matrixSub(&B[newsize], offsetB, B, offsetB, S1, newsize); //B21 - B11
		this->normalMatrixMultiplication(&A[newsize], S1, P4, newsize, newsize); //P4 = A22 * B21 - B11
		matrixSum(&A[newsize], offsetA, &A[newsize], offsetA+newsize, S1, newsize); //A21 + A22
		NUM** Ctmp = &C[newsize];
		this->normalMatrixMultiplication(S1, B, Ctmp, newsize); //C21 = A21 + A22 * B11
		matrixSub(B, offsetB+newsize, &B[newsize], offsetB+newsize, S2, newsize); //B12 - B22
		this->normalMatrixMultiplication(A, S2, C, newsize, 0, 0, newsize); //C12 = A11 * B12 - B22
		matrixSub(&A[newsize], offsetA, A, offsetA, S1, newsize); //A21 - A11
		matrixSum(B, offsetB, B, offsetB+newsize, S2, newsize); //B11 + B12
		this->normalMatrixMultiplication(S1, S2, Ctmp, newsize,0,0,newsize); //C22 = A21 - A11 * B11 + B12

		for(int i = 0; i < newsize; i++) { //C22 += P1 - C21 + C12
			NUM* restrict Ccol = &Ctmp[i][newsize];
			NUM* restrict Ccol2 = &C[i][newsize];
			#pragma ivdep
			for(int j = 0; j < newsize; j++) {
				Ccol[j] += P1[i][j] - Ctmp[i][j] + Ccol2[j];
			}
		}

		for(int i = 0; i < newsize; i++) { //C21 += P4
			NUM *restrict Ccol = Ctmp[i];
			#pragma ivdep
			#pragma vector always
			for(int j = 0; j < newsize; j++) {
				Ccol[j] += P4[i][j];
			}
		}


		this->ff_send_out(new workerOutput<NUM>(Ctmp, this->id));

		matrixSum(A, offsetA, A, offsetA+newsize, S2, newsize); //A11+A12
		this->normalMatrixMultiplication(S2, &B[newsize], P5, newsize, 0, newsize); //P5  = A11 + A12 * B22
		matrixSub(A, offsetA+newsize, &A[newsize], offsetA+newsize, S1, newsize); //A12 - A22
		matrixSum(&B[newsize], offsetB, &B[newsize], offsetB+newsize, S2, newsize); //B21 + B22
		this->normalMatrixMultiplication(S1, S2, C, newsize); //C11 = A12 - A22 * B21 + B22

		for(int i = 0; i < newsize; i++){ //C11 += P1 + P4 - P5
			#pragma ivdep
			for(int j = 0; j < newsize; j++) {
				C[i][j] += P1[i][j] + P4[i][j] - P5[i][j];
			}
		}

		for(int i = 0; i < newsize; i++) { //C12 += P5
			NUM* restrict Ccol2 = &C[i][newsize];
			#pragma ivdep
			for(int j = 0; j < newsize; j++) {
				Ccol2[j] += P5[i][j];
			}
		}

		cleanUp();
		return new workerOutput<NUM>(C, this->id);

		//Check
//		NUM** D = new NUM*[size]();
//		for (int j = 0; j < size; j++){
//			D[j] = new NUM[size]();
//		}
//		this->normalMatrixMultiplication(A,B,D,size);
//
//		for (int i=0; i<size; i++){
//			for(int k = 0; k< size; k++){
//				if (C[i][k] != D[i][k]) printf("Error: %d != %d\n", C[i][k], D[i][k]);
//			}
//		}



	}
};

#endif /* WORKER_S_HPP_ */
