/*
 * StreamGenerator.hpp
 *
 *  Created on: 01/ott/2014
 *      Author: alessandro
 */

#ifndef STREAMGENERATOR_HPP_
#define STREAMGENERATOR_HPP_
#include "Utils.h"
#include <ff/node.hpp>
#include <chrono>
#define CHRONO

using namespace ff;

template<typename NUM>
class StreamGenerator: public ff_node {
public:
	StreamGenerator(unsigned int streamLength,unsigned int matrixSize):ff_node() {this->streamLength = streamLength; this->matrixSize = matrixSize;}
	~StreamGenerator() {}
	void* svc(void * task) {
		if(current < this->streamLength) {
			current++;
			matrix<NUM> *A = new simple_matrix<NUM>(matrixSize,matrixSize);
			matrix<NUM> *B = new simple_matrix<NUM>(matrixSize,matrixSize);
			auto start = std::chrono::high_resolution_clock::now();
			//printf("Generating matrix %d\n", current);
			this->initialize();
			FarmTask<NUM> *t = new FarmTask<NUM>(A, B, matrixSize);
			auto elapsed = std::chrono::high_resolution_clock::now() - start;
			long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
			printf("Generation time (no malloc): %lld\n", microseconds);
			return (void*) t;
		}
		return NULL;
	}
	virtual void initialize()=0;
	NUM * A=nullptr, *B=nullptr;
	unsigned int matrixSize;
private:

	unsigned int streamLength;
	unsigned int current = 0;
};

class IntGenerator: public StreamGenerator<int> {
public:
	IntGenerator(unsigned int streamLength,unsigned int matrixSize):StreamGenerator<int>(streamLength, matrixSize){}
	~IntGenerator(){}
	void initialize() {
		for(register unsigned int i = 0; i < this->matrixSize; i++) {
			for(register unsigned int j = 0; j < this->matrixSize; j++) {
				if(i == j) {
					A[i*this->matrixSize+j] = B[i*this->matrixSize+j] = 1;
				} else {
					A[i*this->matrixSize+j] = B[i*this->matrixSize+j] = 0;
				}

			}
		}
	}
};


#endif /* STREAMGENERATOR_HPP_ */
