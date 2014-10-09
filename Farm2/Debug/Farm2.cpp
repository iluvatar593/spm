//============================================================================
// Name        : Farm2.cpp
// Author      : Alessandro
// Version     : 2.0
// Description : Matrix multiplication implementation. For good performances
//				requires to compile with -restrict (icc tested)
//============================================================================

#include <iostream>
#include <ff/farm.hpp>
#include <stdio.h>
#include "utils.hpp"
#include "emitter.hpp"
#include "worker.hpp"
#include "input_buffer.hpp"
using namespace ff;

static int ***C;

template <typename NUM>
struct pointerTask {
	NUM **restrict A;
	NUM **restrict B;
};

template <typename NUM>
class Emitter: public ff_node{
public:
	Emitter(input_buffer<NUM>* a, input_buffer<NUM>* b,unsigned int bufferSize, unsigned int streamLength, unsigned int size):ff_node(),bufferSize(bufferSize),streamLength(streamLength),size(size){
		current=0;
		offsetA = offsetB = 0;
		this->a = a;
		this->b = b;
	}
	~Emitter(){};
	void * svc(void*) {
		for(unsigned int i = 0; i < streamLength; i++) {
			//task_t *t = (task_t *) malloc(sizeof(task_t));
			//t->a_index = (i+offsetA)%bufferSize;
			//t->b_index = (i+offsetB)%bufferSize;
			pointerTask<NUM> *t = new pointerTask<NUM>();
			t->A = a->getNext();
			t->B = a->getNext();
			ff_send_out((void*) t);
			//if(i % bufferSize == 0) offsetA = (offsetA+1)%bufferSize;
			//if(offsetA % bufferSize == 0) offsetB = (offsetB+1)%bufferSize;
		}
		return NULL;
	}
private:
	unsigned int bufferSize, streamLength, size, current, offsetA, offsetB;
	input_buffer<NUM> *a, *b;
};

template <typename NUM>
class Worker: public ff_node {
public:
	Worker(unsigned int size, unsigned int id):ff_node(),size(size) {
		this->_C = C[id];
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
		int *restrict dummyA, *restrict dummyC, *restrict dummyB;


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
		return _C;
	}
private:
	unsigned int size;
	int **restrict _C;
};

int main(int argc, char* argv[]) {
	/** Check parameters */
	if(argc < 3) { printUsage(); return 1;};
	unsigned int mxw = atoi(argv[1]);
	unsigned int matrixSize = atoi(argv[2]);
	unsigned int numWorkers = atoi(argv[3]);
	unsigned int streamLength = mxw*numWorkers;
	unsigned int bufferSize = calculateBufferSize(sizeof(int), numWorkers, matrixSize, streamLength);
	for(int i = 0; i < argc; i++) {
		printf("%d ------> %s\n",i, argv[i]);
	}
	if(streamLength <= 0 || matrixSize <= 0 || numWorkers <= 0) {
		printUsage();
		return 1;
	}
	input_buffer<int> *A = new input_buffer<int>(bufferSize);
	input_buffer<int> *B = new input_buffer<int>(bufferSize);
	C = new int**[numWorkers]();
	for(unsigned int m = 0; m < bufferSize; m++) {
		int **matrixA, **matrixB;
		matrixA = new int*[matrixSize]();
		matrixB = new int*[matrixSize]();
		for(unsigned int i = 0; i < matrixSize; i++){
			matrixA[i] = new int[matrixSize]();
			matrixB[i] = new int[matrixSize]();
			matrixA[i][i] = matrixB[i][i] = 1;
		}
		A->add(matrixA);
		B->add(matrixB);
	}
	for(unsigned int m = 0; m < numWorkers; m++) {
		C[m] = new int*[matrixSize]();
		for(unsigned int i = 0; i < matrixSize; i++) C[m][i] = new int[matrixSize]();
	}

	#if defined(__MIC__)
    	const char worker_mapping[]="1, 5, 9, 13, 17, 21, 25, 29, 33, 37, 41, 45, 49, 53, 57, 61, 65, 69, 73, 77, 81, 85, 89, 93, 97, 101, 105, 109, 113, 117, 121, 125, 129, 133, 137, 141, 145, 149, 153, 157, 161, 165, 169, 173, 177, 181, 185, 189, 193, 197, 201, 205, 209, 213, 217, 221, 225, 229, 233, 0, 2, 6, 10, 14, 18, 22, 26, 30, 34, 38, 42, 46, 50, 54, 58, 62, 66, 70, 74, 78, 82, 86, 90, 94, 98, 102, 106, 110, 114, 118, 122, 126, 130, 134, 138, 142, 146, 150, 154, 158, 162, 166, 170, 174, 178, 182, 186, 190, 194, 198, 202, 206, 210, 214, 218, 222, 226, 230, 234, 237, 3, 7, 11, 15, 19, 23, 27, 31, 35, 39, 43, 47, 51, 55, 59, 63, 67, 71, 75, 79, 83, 87, 91, 95, 99, 103, 107, 111, 115, 119, 123, 127, 131, 135, 139, 143, 147, 151, 155, 159, 163, 167, 171, 175, 179, 183, 187, 191, 195, 199, 203, 207, 211, 215, 219, 223, 227, 231, 235, 238, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144, 148, 152, 156, 160, 164, 168, 172, 176, 180, 184, 188, 192, 196, 200, 204, 208, 212, 216, 220, 224, 228, 232, 236, 239";
    	threadMapper::instance()->setMappingList(worker_mapping);
	#endif

	Emitter<int> E(A, B, bufferSize, streamLength, matrixSize);
	ff_farm<> * farm = new ff_farm<>();
	farm->add_emitter(&E);
	std::vector<ff_node *> w;
	for(unsigned int i=0;i<numWorkers;++i) w.push_back(new Worker<int>(matrixSize, i));

	farm->add_workers(w);
	farm->run_and_wait_end();
	std::cout << "Total time (with matrix): ";
	std::cout << farm->ffTime();
	std::cout << "\n";
	return 0;
}
