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

using namespace ff;

template<typename NUM>
class StreamGenerator: public ff_node {
public:
	StreamGenerator(unsigned int streamLength,unsigned int matrixSize):ff_node() {this->streamLength = streamLength; this->matrixSize = matrixSize;}
	~StreamGenerator() {}
	void* svc(void * task) {
		if(current < this->streamLength) {
			current++;
			A = (NUM *) calloc(matrixSize*matrixSize, sizeof(NUM));
			B = (NUM *) calloc(matrixSize*matrixSize, sizeof(NUM));
			printf("Generating matrix %d\n", current);
			this->initialize();
			FarmTask<NUM> *t = new FarmTask<NUM>(A, B, matrixSize);
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
