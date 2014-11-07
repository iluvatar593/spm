/*
 * utils.hpp
 *
 *  Created on: 05/nov/2014
 *      Author: alessandro
 */

#ifndef UTILS_HPP_
#define UTILS_HPP_



#include <stdio.h>
#include <stdlib.h>
#define start_time() \
	auto start = std::chrono::high_resolution_clock::now();
/** Shows the elapsed time. See start_time for usage*/
#define elapsed_time(STRING) \
	auto elapsed = std::chrono::high_resolution_clock::now() - start; \
	long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count(); \
	printf(#STRING":%lld\n", microseconds);
#define SPARE_MEMORY 3*1024*1024*1024 //6 gb

#ifndef ALIGNMENT
	#if defined(__MIC__)
		#define ALIGNMENT 64
	#else
		#define ALIGNMENT 32
	#endif
#endif
#ifndef MCOUPLES
	#define MCOUPLES 8
#endif

#define MIN(a, b) (((a) < (b)) ? a : b)

#define _MM_MALLOC(dst, type, size) \
	{(dst) = (type) _mm_malloc((size), ALIGNMENT); \
		if((dst) == nullptr) { \
			printf("ERROR: couldn't allocate memory \n"); \
			exit(-1);			\
		} \
	}
#define _MM_FREE(dst) {_mm_free(dst);}

/**
* Maximum size of the input buffer, in terms of the number of couples of matrix that can be allocated in it.
* Parameters:
* 	s: dimension of the representation of the data type
*	n: rows of the first matrix
*	k : cols of the first matrix / rows of the second one
*	m : cols of the second matrix
*	numWorkers : number of workers desired (each one has its own matrix)
*	streamLength : length of the stream
*
*	returns: number of input couples that can be allocated.
 * */
inline int calculateBufferSize(size_t s, int n, int k, int m, int numWorkers, int streamLength, bool tryanyway) {
	long outputSize = (long) n*m*s*numWorkers;
	if(outputSize > ((long)SPARE_MEMORY)) {
		if(tryanyway){
			if(isatty(fileno(stdout))) printf("I will go with %d matrices, but a SEGFAULT is likely\n", MCOUPLES);
			return 0;
		}
		if(isatty(fileno(stdout))) printf("Sorry, but it is not possible to allocate a %d x %d matrix for every worker, thus the job cannot start. \n Please try again decreasing the number of workers or the dimensions.\n", n, m);
		exit(-1);
	}
	long remainingSize = ((long) SPARE_MEMORY) - outputSize;
	long coupleSize = s*(n*k+k*m);
	long totalNumber = remainingSize/coupleSize;
	if (totalNumber > streamLength) return streamLength;
	return (int) totalNumber;
}
/**
 * Transpose tha matrix a into aT.
 * Parameters are:
 * a : input matrix
 * aT : output matrix
 * n : number of rows to transpose
 * k : number of columns to transpose
 * lda : number of columns in the matrix a
 * ldaT : number of columns in the matrix aT
 *
 * modifies aT
 * requires ldaT > k
 */
template<typename NUM>
inline void recTranspose(NUM *__restrict__ a, NUM *__restrict__ aT, const int n, const int k, const int lda, const int ldat){
	if (n*k <= 512) {
		for(int i = 0; i < n; i++) {
			for(int j = 0; j < k; j++) {
				aT[j*ldat+i] = a[i*lda+j];
			}
		}
		return;
	}
	if(k > n) {
		recTranspose(a, aT, n, (k+1)/2, lda, ldat);
		recTranspose(&a[(k+1)/2], &aT[(k+1)/2*ldat], n, k-((k+1)/2), lda, ldat);
	} else {
		recTranspose(a, aT, (n+1)/2, k, lda, ldat);
		recTranspose(&a[(n+1)/2*lda], &aT[(n+1)/2], n- (n+1)/2, k, lda, ldat);
	}
}
/**
 * Shows usage
 */
void printUsage() {
	printf("Usage: ./FarmIntrinsic.o mxw numWorkers n k m [tryanyway]\n");
	printf("Where:\n");
	printf("mxw is the number of matrices for each worker\n");
	printf("numWorkers is the number of threads allocated to the stream [1 .. 238]\n");
	printf("n k and m are the three dimensions of the matrix \n");
	printf("Optional tryanyway can be used (if equal to 1) to start the computation even at risk of insufficient memory\n");
}

/**
 * Shows usage (Strassen Algorithm)
 */
void printUsage_S() {
	printf("Usage: ./FarmIntrinsic.o mxw numWorkers size [tryanyway] [collector]\n");
	printf("Where:\n");
	printf("mxw is the number of matrices for each worker\n");
	printf("numWorkers is the number of threads allocated to the stream [1 .. 238]\n");
	printf("size is the dimension of the matrix \n");
	printf("Optional tryanyway can be used (if equal to 1) to start the computation even at risk of insufficient memory\n");
	printf("Optional collector can be dummy (default), sampler or pedantic\n");
}

#if defined(__MIC__)
	#define MAXWORKERS 238 //240 - emitter - collector
#else
	#define MAXWORKERS 14 //16 - emitter - collector
#endif

template<typename NUM>
class workerOutput_t{
public:
	workerOutput_t(NUM* matrixChunk, int worker, int id):matrixChunk(matrixChunk),worker(worker),id(id){}
	~workerOutput_t(){}
	NUM* matrixChunk;
	int worker;
	int id;
};

template<typename NUM>
class task_t{
public:
	task_t(NUM* __restrict__ a, NUM* __restrict__ b, int num):a(a),b(b),num(num){}
	~task_t(){}
	NUM* __restrict__ a;
	NUM* __restrict__ b;
	int num;
//	typedef struct __attribute__((align(ALIGNMENT))) {
//		double *__restrict__ a;
//		double *__restrict__ b;
//		int num;
//	} taskDouble_t;
};

#endif /* UTILS_HPP_ */
