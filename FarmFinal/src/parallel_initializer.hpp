/*
 * parallel_initializer.hpp
 *	Use this to avoid waiting for a sequential initialization of the stream.
 *  Created on: 09/ott/2014
 *      Author: alessandro
 */

#ifndef PARALLEL_INITIALIZER_HPP_
#define PARALLEL_INITIALIZER_HPP_

#include <ff/farm.hpp>
#include "input_buffer.hpp"
#include <climits>
#include <time.h>
using namespace ff;

typedef struct {
	unsigned int number;
	int *seeds;
} initialization_task_t;

template<typename NUM>
class InitializerEmitter: public ff_node {
public:
	InitializerEmitter(unsigned int bufferSize, unsigned int seedSize = 10): bufferSize(bufferSize), seedSize(seedSize){

	}
	~InitializerEmitter(){};
	void *svc(void *) {
		srand(time(NULL));
		for(int i = 0; i < bufferSize; i++) {
			initialization_task_t *t = new initialization_task_t;

			int *seeds = new int[seedSize]();
			for(register unsigned int j = 0; j < seedSize; j++) {
				seeds[j] = rand();
			}
			t->number = i;
			t->seeds = seeds;
			//send to worker a task in the form: index for the couples of matrices to generate + some random seeds to produce the matrix
			ff_send_out((void*)t);
		}
		return NULL;
	}
private:
	unsigned int bufferSize, seedSize;
};

template<typename NUM>
class InitializerWorker: public ff_node {
public:
	InitializerWorker(input_buffer<NUM> *A, input_buffer<NUM> *B,unsigned int size, unsigned int randMax = INT_MAX,unsigned int seedSize = 10):
							A(A), B(B), size(size), randMax(randMax), seedSize(seedSize){

	}
	void *svc(void *restrict task) {
		initialization_task_t *t = (initialization_task_t *) task;
		NUM **matrixA, **matrixB;
		matrixA = new NUM*[size]();
		matrixB = new NUM*[size]();
		for(unsigned int i = 0; i < size; i++){
			matrixA[i] = new NUM[size]();
			matrixB[i] = new NUM[size]();
		}
		for(unsigned int i = 0; i < size; i++) {
			for(unsigned int j = 0; j < size; j++) {
				//matrixA[i][j] = (NUM)(((NUM)t->seeds[i%seedSize])*((NUM)t->seeds[(2*i+j)%seedSize]))/((NUM)randMax);
				//matrixB[i][j] = (NUM)(((NUM)t->seeds[(i+j)%seedSize])*((NUM)t->seeds[(i+2*j)%seedSize]))/((NUM)randMax);
				matrixA[i][j] = ((NUM)((t->seeds[i%seedSize] % randMax) * (t->seeds[(2*i+j) % seedSize] % randMax))) / t->seeds[j%seedSize];
				matrixB[i][j] = ((NUM) ((t->seeds[(i+j)%seedSize] % randMax)*(t->seeds[(i+2*j)%seedSize] % seedSize)))/(t->seeds[(2*(i+j)) %seedSize]);
			}
		}
		A->add(matrixA,t->number);
		B->add(matrixB,t->number);
		delete[] t->seeds;
		delete t;
		return GO_ON;
	}
private:
	input_buffer<NUM> *A, *B;
	unsigned int size, randMax, seedSize;
};

/**
 * Initializes the buffers using a farm.
 */
template<typename NUM>
void initializeInParallel(input_buffer<NUM> *a, input_buffer<NUM>*b, unsigned int matrixSize, unsigned int bufferSize, unsigned int numWorkers) {
	ff_farm<> *farm = new ff_farm<>();
	farm->set_scheduling_ondemand(0);
	std::vector<ff_node *> w;
	for(unsigned int i=0;i<numWorkers;++i) w.push_back(new InitializerWorker<NUM>(a, b, matrixSize, 5470128, 20));
	farm->add_workers(w);
	farm->add_emitter(new InitializerEmitter<NUM>(bufferSize, 20));
	farm->run_and_wait_end();
	printf("Ended initialization: %d", farm->ffTime());
}




#endif /* PARALLEL_INITIALIZER_HPP_ */
