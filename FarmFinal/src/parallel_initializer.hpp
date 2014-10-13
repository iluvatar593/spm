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
	int *seeds;
} initialization_task_t;

template <typename NUM>
class initialization_output {
public:
	initialization_output(NUM**A, NUM**B):A(A),B(B){}
	~initialization_output() {}
	NUM** A;
	NUM** B;
};

template<typename NUM>
class initialization_linear {
public:
	initialization_linear(NUM*A, NUM*B):A(A),B(B){}
	~initialization_linear(){}
	NUM*A, *B;
};

template<typename NUM>
class InitializerEmitter: public ff_node {
public:
	InitializerEmitter(unsigned int bufferSize, unsigned int seedSize = 10): bufferSize(bufferSize), seedSize(seedSize), ff_node(){

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
	InitializerWorker(unsigned int size, unsigned int randMax = INT_MAX,unsigned int seedSize = 10):
							size(size), randMax(randMax), seedSize(seedSize), ff_node(){

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
		initialization_output<NUM> *output = new initialization_output<NUM>(matrixA, matrixB);
		delete[] t->seeds;
		delete t;
		return output;
	}
private:
	unsigned int size, randMax, seedSize;
};

template <typename NUM>
class InitializerCollector: public ff_node {
public:
	InitializerCollector(input_buffer<NUM> *A, input_buffer<NUM> *B):ff_node(),A(A),B(B){
	}
	~InitializerCollector(){}
	void *svc (void * t) {
		if(t == NULL) return NULL;
		initialization_output<NUM> *matrices = (initialization_output<NUM> *) t;

		A->add(matrices->A);
		B->add(matrices->B);
		return GO_ON;
	}
private:
	input_buffer<NUM> * A;
	input_buffer<NUM> * B;
};

/**
 * Initializes the buffers using a farm.
 */
template<typename NUM>
void initializeInParallel(input_buffer<NUM> *a, input_buffer<NUM>*b, unsigned int matrixSize, unsigned int bufferSize, unsigned int numWorkers) {
	ff_farm<> * farm = new ff_farm<>(false, 10, 10, true,240,true);
	farm->set_scheduling_ondemand(0);
	std::vector<ff_node *> w;
	for(unsigned int i=0;i<numWorkers;++i) w.push_back(new InitializerWorker<NUM>(matrixSize, 5470128, 20));
	farm->add_workers(w);
	farm->add_emitter(new InitializerEmitter<NUM>(bufferSize, 20));
	farm->add_collector(new InitializerCollector<NUM>(a, b));
	farm->run_and_wait_end();
	delete farm;
}

template<typename NUM>
class InitializeLinearWorker: public ff_node {
public:
	InitializeLinearWorker(unsigned int size, unsigned int randMax, unsigned int seedSize):size(size),randMax(randMax),seedSize(seedSize),ff_node(){}
	~InitializeLinearWorker(){}
	void *svc(void * task) {
		initialization_task_t *t = (initialization_task_t *) task;
		NUM *matrixA,* matrixB;
		matrixA = (NUM*) malloc_huge_pages(sizeof(NUM)*size*size);
		matrixB = (NUM*) malloc_huge_pages(sizeof(NUM)*size*size);
		for(unsigned int i = 0; i < size; i++) {
			for(unsigned int j = 0; j < size; j++) {
				matrixA[i*size+j] = ((NUM)((t->seeds[i%seedSize] % randMax) * (t->seeds[(2*i+j) % seedSize] % randMax))) / t->seeds[j%seedSize];
				matrixB[i*size+j] = ((NUM) ((t->seeds[(i+j)%seedSize] % randMax)*(t->seeds[(i+2*j)%seedSize] % seedSize)))/(t->seeds[(2*(i+j)) %seedSize]);
			}
		}
		initialization_linear<NUM> *out = new initialization_linear<NUM>(matrixA, matrixB);
		delete t->seeds;
		delete t;
		return out;
	}
private:
	unsigned int size, randMax, seedSize;
};

template <typename NUM>
class InitializeLinearCollector: public ff_node {
public:
	InitializeLinearCollector(linear_buffer<NUM> *A, linear_buffer<NUM> *B):ff_node(),A(A),B(B){
	}
	~InitializeLinearCollector(){}
	void *svc (void * t) {
		if(t == NULL) return NULL;
		initialization_linear<NUM> *matrices = (initialization_linear<NUM> *) t;
		A->add(matrices->A);
		B->add(matrices->B);
		return GO_ON;
	}
private:
	linear_buffer<NUM> * A;
	linear_buffer<NUM> * B;
};

template<typename NUM>
void initializeInParallelLinear(linear_buffer<NUM> *a, linear_buffer<NUM> *b, unsigned int matrixSize, unsigned int bufferSize, unsigned int numWorkers) {
	ff_farm<> *farm = new ff_farm<>(false, 10, 10, true, 240, true);
	farm->set_scheduling_ondemand(0);
	std::vector<ff_node*> w;
	for(unsigned int i=0;i<numWorkers;++i) w.push_back(new InitializeLinearWorker<NUM>(matrixSize, 5470128, 20));
	farm->add_emitter(new InitializerEmitter<NUM>(bufferSize, 20));
	farm->add_workers(w);
	farm->add_collector(new InitializeLinearCollector<NUM>(a, b));
	farm->run_and_wait_end();
	delete farm;
}




#endif /* PARALLEL_INITIALIZER_HPP_ */
