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

template <typename NUM>
class StrassenWorker:public ff_node {
public:
	StrassenWorker(unsigned int size, unsigned int id, NUM*** outputBuffer):msize(size){

		first = outputBuffer[2*id];
		second = outputBuffer[2*id+1];
		_C = first;
		initializeStrassenMatrices(size);
	};
	~StrassenWorker(){
		for(int i = 0; i < msize; i++) {
			delete[] first[i];
			delete[] second[i];
		}
		for(int i = 0; i < msize/2; i++) {
			delete[] P1[i];
//			delete[] P2[i];
//			delete[] P3[i];
			delete[] P4[i];
			delete[] P5[i];
//			delete[] P6[i];
//			delete[] P7[i];
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
		NUM** restrict D = new NUM*[msize]();
		for(int i=0; i<msize; i++) D[i]=new NUM[msize]();
		normalMatrixMultiplication(A,B,D,msize);

		this->strassenMatrixMultiplication(A, B, _C, msize);
		//Changes _C and returns the correct matrix
		for(int i=0; i<msize; i++){
			for(int k=0; k<msize; k++){
				if(_C[i][k] != D[i][k]) printf("Final check: %f should be %f\n", _C[i][k], D[i][k]);
			}
		}
		_C = (_C == first) ? second : first;
		return (_C == first) ? second : first;
	}

private:
	NUM **restrict S1, **restrict S2, **restrict P1, **restrict P2, **restrict P3, **restrict P4, **restrict P5, **restrict P6, **restrict P7, **restrict first, **restrict second, **_C;
	unsigned int msize;
	/** Cleans the temporary area used for strassen procedure */
	inline void cleanUp(){ //TODO specialize between optimized and not
		for(int i = 0; i < msize/2; i++){
			for(int j = 0; j < msize/2; j++) {
				//P1[i][j] = P2[i][j] = P3[i][j] = P4[i][j] = P5[i][j] = P6[i][j] = P7[i][j] = 0;
				P1[i][j] = P4[i][j] = P5[i][j] = 0;
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

	inline void strassenMatrixMultiplication(NUM **restrict A, NUM** restrict B, NUM**restrict C, unsigned int size, unsigned int offsetA = 0, unsigned int offsetB = 0) {

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
		 */

		unsigned int newsize = size/2;
		matrixSum(A, offsetA, &A[newsize], offsetA+newsize, S1, newsize); //S1 has been calculated
		matrixSum(B, offsetB, &B[newsize], offsetB+newsize, S2, newsize); //S2 has been calculated
		normalMatrixMultiplication(S1, S2, P1, newsize); //P1 = S1*S2
		matrixSum(&A[newsize], offsetA, &A[newsize], offsetA+newsize, S1, newsize); //S3
		normalMatrixMultiplication(S1, B, P2, newsize); //P2 = S3 * B11
		matrixSub(B, offsetB+newsize, &B[newsize], offsetB+newsize, S2, newsize); //S4
		normalMatrixMultiplication(A, S2, P3, newsize); //P3 = A11 * S4
		matrixSub(&B[newsize], offsetB, B, offsetB, S1, newsize); //S5
		normalMatrixMultiplication(&A[newsize], S1, P4, newsize, newsize);//P4  = A22 * S5
		matrixSum(A, offsetA, A, offsetA+newsize, S2, newsize); //S6
		normalMatrixMultiplication(S2, &B[newsize], P5, newsize, 0, newsize);//P5  = S6 * B22
		matrixSub(&A[newsize], offsetA, A, offsetA, S1, newsize); //S7
		matrixSum(B, offsetB, B, offsetB+newsize, S2, newsize); //S8
		normalMatrixMultiplication(S1, S2, P6, newsize);//P6  = S7 * S8
		matrixSub(A, offsetA+newsize, &A[newsize], offsetA+newsize, S1, newsize); //S9
		matrixSum(&B[newsize], offsetB, &B[newsize], offsetB+newsize, S2, newsize); //S10
		normalMatrixMultiplication(S1, S2, P7,newsize);//P7  = S9*S10
		NUM** Ctmp = &C[newsize];
		//C11
		for(int i = 0; i < newsize; i++){
			#pragma ivdep //Check if useful
			for(int j = 0; j < newsize; j++) {
				C[i][j] = P1[i][j] + P4[i][j] - P5[i][j] + P7[i][j];
			}
		}
		//C12
		for(int i = 0; i < newsize; i++) {
			NUM* restrict Ccol2 = &C[i][newsize];
			#pragma ivdep //Check if useful
			for(int j = 0; j < newsize; j++) {
				Ccol2[j] = P3[i][j] + P5[i][j];
			}
		}
		//C21
		for(int i = 0; i < newsize; i++) {
			#pragma ivdep //Check if useful
			for(int j = 0; j < newsize; j++) {
				Ctmp[i][j] = P2[i][j] + P4[i][j];
			}
		}
		//C22
		for(int i = 0; i < newsize; i++) {
			NUM* restrict Ccol = &Ctmp[i][newsize];
			#pragma ivdep //Check if useful
			for(int j = 0; j < newsize; j++) {
				Ccol[j] = P1[i][j] - P2[i][j] + P3[i][j] + P6[i][j];
			}
		}
		cleanUp();
	}

	void strassenOptimizedMatrixMultiplication(NUM **restrict A, NUM** restrict B, NUM**restrict C, unsigned int size, unsigned int offsetA = 0, unsigned int offsetB = 0) {
		/*
		 *
		 * Memory usage optimizations:
		 *
		P2  = C21;
		P3  = C12;
		P6  = C22;
		P7  = C11;
		*
		*/
					unsigned int newsize = size/2;
					matrixSum(A, offsetA, &A[newsize], offsetA+newsize, S1, newsize); //S1
					matrixSum(B, offsetB, &B[newsize], offsetB+newsize, S2, newsize); //S2
					normalMatrixMultiplication(S1, S2, P1, newsize);
					matrixSum(&A[newsize], offsetA, &A[newsize], offsetA+newsize, S1, newsize); //S3
					NUM** Ctmp = &C[newsize];
					normalMatrixMultiplication(S1, B, Ctmp, newsize); //optimization P2 = C21
					matrixSub(B, offsetB+newsize, &B[newsize], offsetB+newsize, S2, newsize); //S4
					normalMatrixMultiplication(A, S2, C, newsize, 0, 0, newsize); //optimization P3 = C12
					matrixSub(&B[newsize], offsetB, B, offsetB, S1, newsize); //S5
					normalMatrixMultiplication(&A[newsize], S1, P4, newsize, newsize);
					matrixSum(A, offsetA, A, offsetA+newsize, S2, newsize); //S6
					normalMatrixMultiplication(S2, &B[newsize], P5, newsize, 0, newsize);
					matrixSub(&A[newsize], offsetA, A, offsetA, S1, newsize); //S7
					matrixSum(B, offsetB, B, offsetB+newsize, S2, newsize); //S8
					normalMatrixMultiplication(S1, S2, Ctmp, newsize,0,0,newsize); //optimization P6=C22
					matrixSub(A, offsetA+newsize, &A[newsize], offsetA+newsize, S1, newsize); //S9
					matrixSum(&B[newsize], offsetB, &B[newsize], offsetB+newsize, S2, newsize); //S10
					normalMatrixMultiplication(S1, S2, C, newsize); //optimization P7 = C11

					//VEC Loops , reminder: NUM** Ctmp = &C[newsize];

					//C22
					for(int i = 0; i < newsize; i++) {
						NUM* restrict Ccol = &Ctmp[i][newsize];
						NUM* restrict Ccol2 = &C[i][newsize];
						#pragma ivdep
						for(int j = 0; j < newsize; j++) {
							Ccol[j] += P1[i][j] - Ctmp[i][j] + Ccol2[j];
						}
					}

					//C11
					for(int i = 0; i < newsize; i++){
						#pragma ivdep
						for(int j = 0; j < newsize; j++) {
							C[i][j] += P1[i][j] + P4[i][j] - P5[i][j];
						}
					}

					//C12
					for(int i = 0; i < newsize; i++) {
						#pragma ivdep
						NUM* restrict Ccol2 = &C[i][newsize];
						for(int j = 0; j < newsize; j++) {
							Ccol2[j] += P5[i][j];
						}
					}

					//C21 (some error in P2 or P4)
					for(int i = 0; i < newsize; i++) {
						#pragma ivdep
						for(int j = 0; j < newsize; j++) {
							Ctmp[i][j] += P4[i][j];
						}
					}

					cleanUp();
				}
};



#endif /* WORKER_HPP_ */
