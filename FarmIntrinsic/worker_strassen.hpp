/*
 * worker_strassen.hpp
 *
 *  Created on: 07/nov/2014
 *      Author: atzoril
 */

#ifndef WORKER_STRASSEN_HPP_
#define WORKER_STRASSEN_HPP_

#include <ff/node.hpp>
#include "utils.hpp"

/* Undefining TILE
*  Strassen splits the matrix in four submatrices
*  =>
*  we need 2*TILE on Xeon Phi with float values because of the 30x16 core (120%16 != 0)
*/
#ifdef FLOAT
#undef TILE
	#if defined(__MIC__)
		#define TILE 240
	#else
		#define TILE 128
	#endif
#endif

using namespace ff;

#ifdef FLOAT
	#define TYPE float
	#if defined(__MIC__)
		#define OFFSET_ROW 30
		#define OFFSET_COL 16
	#else
		#define OFFSET_ROW 8
		#define OFFSET_COL 8
	#endif
#else //default = double
	#define TYPE double
	#if defined(__MIC__)
		#define OFFSET_ROW 30
		#define OFFSET_COL 8
	#else
		#define OFFSET_ROW 8
		#define OFFSET_COL 4
	#endif
#endif

#include "matrixmultiplication.hpp"

/*
 * Farm worker used in Strassen algorithm for both value types.
 * Foreach task received calls the strassenMatrixMultiplication function. See inside for the details.
 */
template<typename NUM>
class WorkerStrassen : public ff_node {
public:
	WorkerStrassen(int size, int oldsize, int id):
		size(size), oldsize(oldsize), ff_node(){
		_MM_MALLOC(C, NUM*, sizeof(NUM)*size*size);
		output = new workerOutput_t<NUM>(C, id, -1);
		int newsize=size/2;
		S1 = (NUM*)_mm_malloc(sizeof(NUM)*newsize*newsize, ALIGNMENT);
		S2 = (NUM*)_mm_malloc(sizeof(NUM)*newsize*newsize, ALIGNMENT);
		P1 = (NUM*)_mm_malloc(sizeof(NUM)*newsize*newsize, ALIGNMENT);
		P4 = (NUM*)_mm_malloc(sizeof(NUM)*newsize*newsize, ALIGNMENT);
		P5 = (NUM*)_mm_malloc(sizeof(NUM)*newsize*newsize, ALIGNMENT);
	}
	~WorkerStrassen() {
		_MM_FREE(C);
		_MM_FREE(S1);
		_MM_FREE(S2);
		_MM_FREE(P1);
		_MM_FREE(P4);
		_MM_FREE(P5);
	}

	void *svc(void *__restrict__ task) {
		task_t<NUM> * t = (task_t<NUM> *) task;
		NUM *__restrict__ a = t->a;
		NUM *__restrict__ b = t->b;
		output->id = t->num;

		return this->strassenMatrixMultiplication(a, b, C, size);
	}
private:
	int size, oldsize;
	NUM *__restrict__ C;
	workerOutput_t<NUM>* output;
	NUM *S1, *S2, *P1, *P4, *P5;

	/** Performs the sum of matrix A and matrix B and stores the result into C.
	A, B and C are required to be square matrices.
	They will be represented as matrices whose rows are long ldA, ldB and ldC respectively. This is useful mainly for submatrices.
	 */
	inline void matrixSum(NUM* restrict A, NUM* restrict B, NUM*restrict C, int size, int ldA, int ldB, int ldC) {
		__assume_aligned(A, ALIGNMENT);
		__assume_aligned(B, ALIGNMENT);
		__assume_aligned(C, ALIGNMENT);
		for(int i = 0; i < size; i++){
			NUM* restrict rowA = &A[i*ldA], *restrict rowB = &B[i*ldB], *restrict rowC = &C[i*ldC];
			for(int j =0 ; j < size; j++) {
				rowC[j] = rowA[j] + rowB[j];
			}
		}
	}

	/** Subtraction of two matrices. Same meaning of the parameters as matrixSum above.*/
	inline void matrixSub(NUM* restrict A, NUM* restrict B, NUM*restrict C, int size, int ldA, int ldB, int ldC) {
		__assume_aligned(A, ALIGNMENT);
		__assume_aligned(B, ALIGNMENT);
		__assume_aligned(C, ALIGNMENT);
		for(int i = 0; i < size; i++){
			NUM* restrict rowA = &A[i*ldA], *restrict rowB = &B[i*ldB], *restrict rowC = &C[i*ldC];
			for(int j =0 ; j < size; j++) {
				rowC[j] = rowA[j] - rowB[j];
			}
		}
	}

	//Strassen wrapper to call the kernel with the right offsetting
	inline void coreMult(NUM * a, NUM * b, NUM* c, int size, int lda, int ldb, int ldc){
		NUM aT[size*OFFSET_ROW] __attribute__ ((aligned(ALIGNMENT)));
		for(int i=0; i <size; i+=OFFSET_ROW){
			recTranspose<NUM>(&a[i*lda], aT, OFFSET_ROW, size, lda, OFFSET_ROW); //a, aT, n, k, lda, ldat
			for(int j=0; j< size; j+=OFFSET_COL){
				Kernel<NUM>(aT, &b[j], &c[i*ldc+j], ldb, size/TILE, ldc);

			}
		}
	}

	inline void * strassenMatrixMultiplication(NUM* restrict A, NUM* restrict B, NUM* restrict C, unsigned int size) {
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
		 */
		unsigned int newsize = size/2;
		matrixSum(A, &A[newsize*size+newsize], S1, newsize, size, size, newsize); //A11 + A22
		matrixSum(B, &B[newsize*size+newsize], S2, newsize, size, size, newsize); //B11 + B22
		coreMult(S1, S2, P1, newsize, newsize, newsize, newsize); //P1  = A11 + A22 * B11 + B22
		matrixSub(&B[newsize*size], B, S1, newsize, size, size, newsize); //B21 - B11
		coreMult(&A[newsize*size+newsize], S1, P4, newsize, size, newsize, newsize); //P4 = A22 * B21 - B11
		matrixSum(&A[newsize*size], &A[newsize*size+newsize], S1, newsize, size, size, newsize); //A21 + A22
		NUM* Ctmp = &C[newsize*size]; //C21
		coreMult(S1, B, Ctmp, newsize, newsize, size, size); //C21 = (A21 + A22) * B11
		matrixSub(&B[newsize], &B[newsize*size+newsize], S2, newsize, size, size, newsize); //B12 - B22
		coreMult(A, S2, &C[newsize], newsize, size, newsize, size); //C12 = A11 * B12 - B22
		matrixSub(&A[newsize*size], A, S1, newsize, size, size, newsize); //A21 - A11
		matrixSum(B, &B[newsize], S2, newsize, size, size, newsize); //B11 + B12
		coreMult(S1, S2, &Ctmp[newsize], newsize, newsize, newsize, size); //C22 = A21 - A11 * B11 + B12

		for(int i = 0; i < newsize; i++) { //C22 += P1 - C21 + C12
			NUM* restrict Ccol = &Ctmp[i*size+newsize]; //Ctmp is C21, so we offset it to obtain Ccol, a row of C22
			NUM* restrict Ccol2 = &C[i*size+newsize]; //Row of C12
			#pragma ivdep
			for(int j = 0; j < newsize; j++) {
				Ccol[j] += P1[i*newsize+j] - Ctmp[i*size+j] + Ccol2[j];
			}
		}
		for(int i = 0; i < newsize; i++) { //C21 += P4
			NUM *restrict Ccol = &Ctmp[i*size];
			for(int j = 0; j < newsize; j++) {
				Ccol[j] += P4[i*newsize+j];
			}
		}
		//Lower half of the matrix is ready so we can send out.
		output->matrixChunk = Ctmp;
		ff_send_out(output);

		matrixSum(A, &A[newsize], S2, newsize, size, size, newsize); //A11+A12
		coreMult(S2, &B[newsize+newsize*size], P5, newsize, newsize, size, newsize); //P5  = A11 + A12 * B22
		matrixSub(&A[newsize], &A[newsize*size+newsize],S1, newsize, size, size, newsize); //A12 - A22
		matrixSum(&B[newsize*size], &B[newsize*size+newsize], S2, newsize, size, size, newsize); //B21 + B22
		coreMult(S1, S2, C, newsize, newsize, newsize, size); //C11 = A12 - A22 * B21 + B22

		for(int i = 0; i < newsize; i++){ //C11 += P1 + P4 - P5
			for(int j = 0; j < newsize; j++) {
				C[i*size+j] += P1[i*newsize+j] + P4[i*newsize+j] - P5[i*newsize+j];
			}
		}

		for(int i = 0; i < newsize; i++) { //C12 += P5
			NUM* restrict Ccol2 = &C[i*size+newsize];
			for(int j = 0; j < newsize; j++) {
				Ccol2[j] += P5[i*newsize+j];
			}
		}
		//Sending out upper half of C.
		output->matrixChunk = C;
		return output;
	}
};

/*
 * Farm worker used in Strassen algorithm for both value types. Used in pedantic-mode debugging.
 * Foreach task received calls the strassenMatrixMultiplication function. See inside for the details.
 */
template<typename NUM>
class WorkerStrassenPedantic : public ff_node {
public:
	WorkerStrassenPedantic(int size, int oldsize, int id):
		size(size), oldsize(oldsize), ff_node(){
		printf("Worker constructor\n");
		_MM_MALLOC(C1, NUM*, sizeof(NUM)*size*size);
		_MM_MALLOC(C2, NUM*, sizeof(NUM)*size*size);
		output = new workerOutput_t<NUM>(C1, id, -1);
		int newsize=size/2;
		S1 = (NUM*)_mm_malloc(sizeof(NUM)*newsize*newsize, ALIGNMENT);
		S2 = (NUM*)_mm_malloc(sizeof(NUM)*newsize*newsize, ALIGNMENT);
		P1 = (NUM*)_mm_malloc(sizeof(NUM)*newsize*newsize, ALIGNMENT);
		P4 = (NUM*)_mm_malloc(sizeof(NUM)*newsize*newsize, ALIGNMENT);
		P5 = (NUM*)_mm_malloc(sizeof(NUM)*newsize*newsize, ALIGNMENT);
	}
	~WorkerStrassenPedantic() {
		_MM_FREE(C1);
		_MM_FREE(C2);
		_MM_FREE(S1);
		_MM_FREE(S2);
		_MM_FREE(P1);
		_MM_FREE(P4);
		_MM_FREE(P5);
	}

	void *svc(void *__restrict__ task) {
		task_t<NUM> * t = (task_t<NUM> *) task;
		NUM *__restrict__ a = t->a;
		NUM *__restrict__ b = t->b;
		output->id = t->num;
		NUM *__restrict__ C = (first) ? C1 : C2;
		first = !first;
		return this->strassenMatrixMultiplication(a, b, C, size);
	}
private:
	int size, oldsize;
	NUM *__restrict__ C1, *__restrict__ C2, *__restrict__ S1, *__restrict__ S2, *__restrict__ P1, *__restrict__ P4, *__restrict__ P5;;
	workerOutput_t<NUM>* output;
	bool first = true;

	/** Performs the sum of matrix A and matrix B and stores the result into C.
	A, B and C are required to be square matrices.
	They will be represented as matrices whose rows are long ldA, ldB and ldC respectively. This is useful mainly for submatrices.
	 */
	inline void matrixSum(NUM* restrict A, NUM* restrict B, NUM*restrict C, int size, int ldA, int ldB, int ldC) {
		__assume_aligned(A, ALIGNMENT);
		__assume_aligned(B, ALIGNMENT);
		__assume_aligned(C, ALIGNMENT);
		for(int i = 0; i < size; i++){
			NUM* restrict rowA = &A[i*ldA], *restrict rowB = &B[i*ldB], *restrict rowC = &C[i*ldC];
			for(int j =0 ; j < size; j++) {
				rowC[j] = rowA[j] + rowB[j];
			}
		}
	}

	/** Subtraction of two matrices. Same meaning of the parameters as matrixSum above.*/
	inline void matrixSub(NUM* restrict A, NUM* restrict B, NUM*restrict C, int size, int ldA, int ldB, int ldC) {
		__assume_aligned(A, ALIGNMENT);
		__assume_aligned(B, ALIGNMENT);
		__assume_aligned(C, ALIGNMENT);
		for(int i = 0; i < size; i++){
			NUM* restrict rowA = &A[i*ldA], *restrict rowB = &B[i*ldB], *restrict rowC = &C[i*ldC];
			for(int j =0 ; j < size; j++) {
				rowC[j] = rowA[j] - rowB[j];
			}
		}
	}

	//Strassen wrapper to call the kernel with the right offsetting
	inline void coreMult(NUM * a, NUM * b, NUM* c, int size, int lda, int ldb, int ldc){
		NUM aT[size*OFFSET_ROW] __attribute__ ((aligned(ALIGNMENT)));
		for(int i=0; i <size; i+=OFFSET_ROW){
			recTranspose<NUM>(&a[i*lda], aT, OFFSET_ROW, size, lda, OFFSET_ROW); //a, aT, n, k, lda, ldat
			for(int j=0; j< size; j+=OFFSET_COL){
				Kernel<NUM>(aT, &b[j], &c[i*ldc+j], ldb, size/TILE, ldc);

			}
		}
	}

	inline void * strassenMatrixMultiplication(NUM* restrict A, NUM* restrict B, NUM* restrict C, unsigned int size) {
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
		 */
		unsigned int newsize = size/2;
		matrixSum(A, &A[newsize*size+newsize], S1, newsize, size, size, newsize); //A11 + A22
		matrixSum(B, &B[newsize*size+newsize], S2, newsize, size, size, newsize); //B11 + B22
		coreMult(S1, S2, P1, newsize, newsize, newsize, newsize); //P1  = A11 + A22 * B11 + B22
		matrixSub(&B[newsize*size], B, S1, newsize, size, size, newsize); //B21 - B11
		coreMult(&A[newsize*size+newsize], S1, P4, newsize, size, newsize, newsize); //P4 = A22 * B21 - B11
		matrixSum(&A[newsize*size], &A[newsize*size+newsize], S1, newsize, size, size, newsize); //A21 + A22
		NUM* Ctmp = &C[newsize*size]; //C21
		coreMult(S1, B, Ctmp, newsize, newsize, size, size); //C21 = (A21 + A22) * B11
		matrixSub(&B[newsize], &B[newsize*size+newsize], S2, newsize, size, size, newsize); //B12 - B22
		coreMult(A, S2, &C[newsize], newsize, size, newsize, size); //C12 = A11 * B12 - B22
		matrixSub(&A[newsize*size], A, S1, newsize, size, size, newsize); //A21 - A11
		matrixSum(B, &B[newsize], S2, newsize, size, size, newsize); //B11 + B12
		coreMult(S1, S2, &Ctmp[newsize], newsize, newsize, newsize, size); //C22 = A21 - A11 * B11 + B12

		for(int i = 0; i < newsize; i++) { //C22 += P1 - C21 + C12
			NUM* restrict Ccol = &Ctmp[i*size+newsize]; //Ctmp is C21, so we offset it to obtain Ccol, a row of C22
			NUM* restrict Ccol2 = &C[i*size+newsize]; //Row of C12
			for(int j = 0; j < newsize; j++) {
				Ccol[j] += P1[i*newsize+j] - Ctmp[i*size+j] + Ccol2[j];
			}
		}
		for(int i = 0; i < newsize; i++) { //C21 += P4
			NUM *restrict Ccol = &Ctmp[i*size];
			for(int j = 0; j < newsize; j++) {
				Ccol[j] += P4[i*newsize+j];
			}
		}

		matrixSum(A, &A[newsize], S2, newsize, size, size, newsize); //A11+A12
		coreMult(S2, &B[newsize+newsize*size], P5, newsize, newsize, size, newsize); //P5  = A11 + A12 * B22
		matrixSub(&A[newsize], &A[newsize*size+newsize],S1, newsize, size, size, newsize); //A12 - A22
		matrixSum(&B[newsize*size], &B[newsize*size+newsize], S2, newsize, size, size, newsize); //B21 + B22
		coreMult(S1, S2, C, newsize, newsize, newsize, size); //C11 = A12 - A22 * B21 + B22

		for(int i = 0; i < newsize; i++){ //C11 += P1 + P4 - P5
			for(int j = 0; j < newsize; j++) {
				C[i*size+j] += P1[i*newsize+j] + P4[i*newsize+j] - P5[i*newsize+j];
			}
		}

		for(int i = 0; i < newsize; i++) { //C12 += P5
			NUM* restrict Ccol2 = &C[i*size+newsize];
			for(int j = 0; j < newsize; j++) {
				Ccol2[j] += P5[i*newsize+j];
			}
		}

		//Pedantic mode: we need to send the full matrix.
		output->matrixChunk = C;
		return output;
	}
};




#endif /* WORKER_STRASSEN_HPP_ */
