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
#include "collector.hpp"
#include "parallel_initializer.hpp"
//type to use for multiplication
#define type double
#define initializers 8
using namespace ff;

int main(int argc, char* argv[]) {
	/** Check parameters */
	if(argc < 3) { printUsage(); return 1;};
	unsigned int mxw = atoi(argv[1]);
	unsigned int matrixSize = atoi(argv[2]);
	unsigned int numWorkers = atoi(argv[3]);
	unsigned int streamLength = mxw*numWorkers;
	unsigned int bufferSize = calculateBufferSize(sizeof(int), numWorkers, matrixSize, streamLength);
	bool executeStrassen = (argc >= 4 && atoi(argv[4]) == 1);
	for(int i = 0; i < argc; i++) {
		printf("%d ------> %s\n",i, argv[i]);
	}
	if(streamLength <= 0 || matrixSize <= 0 || numWorkers <= 0) {
		printUsage();
		return 1;
	}
	input_buffer<type> *A = new input_buffer<type>(bufferSize);
	input_buffer<type> *B = new input_buffer<type>(bufferSize);
	type ***C = new type**[numWorkers*2]();
	//initializeInParallel(A, B, matrixSize, bufferSize, initializers);
	for(unsigned int m = 0; m < bufferSize; m++) {
		type **matrixA, **matrixB;
		matrixA = new type*[matrixSize]();
		matrixB = new type*[matrixSize]();
		for(unsigned int i = 0; i < matrixSize; i++){
			matrixA[i] = new type[matrixSize]();
			matrixB[i] = new type[matrixSize]();
			matrixA[i][i] = matrixB[i][i] = 1;
		}
		A->add(matrixA);
		B->add(matrixB);
	}
	for(unsigned int m = 0; m < numWorkers*2; m++) {
		C[m] = new type*[matrixSize]();
		for(unsigned int i = 0; i < matrixSize; i++) C[m][i] = new type[matrixSize]();
	}

	#if defined(__MIC__)
    	const char worker_mapping[]="1, 5, 9, 13, 17, 21, 25, 29, 33, 37, 41, 45, 49, 53, 57, 61, 65, 69, 73, 77, 81, 85, 89, 93, 97, 101, 105, 109, 113, 117, 121, 125, 129, 133, 137, 141, 145, 149, 153, 157, 161, 165, 169, 173, 177, 181, 185, 189, 193, 197, 201, 205, 209, 213, 217, 221, 225, 229, 233, 0, 2, 6, 10, 14, 18, 22, 26, 30, 34, 38, 42, 46, 50, 54, 58, 62, 66, 70, 74, 78, 82, 86, 90, 94, 98, 102, 106, 110, 114, 118, 122, 126, 130, 134, 138, 142, 146, 150, 154, 158, 162, 166, 170, 174, 178, 182, 186, 190, 194, 198, 202, 206, 210, 214, 218, 222, 226, 230, 234, 237, 3, 7, 11, 15, 19, 23, 27, 31, 35, 39, 43, 47, 51, 55, 59, 63, 67, 71, 75, 79, 83, 87, 91, 95, 99, 103, 107, 111, 115, 119, 123, 127, 131, 135, 139, 143, 147, 151, 155, 159, 163, 167, 171, 175, 179, 183, 187, 191, 195, 199, 203, 207, 211, 215, 219, 223, 227, 231, 235, 238, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144, 148, 152, 156, 160, 164, 168, 172, 176, 180, 184, 188, 192, 196, 200, 204, 208, 212, 216, 220, 224, 228, 232, 236, 239";
    	threadMapper::instance()->setMappingList(worker_mapping);
	#endif

	Emitter<type> E(A, B, bufferSize, streamLength, matrixSize);
	ff_farm<> * farm = new ff_farm<>();
	farm->add_emitter(&E);
	Collector<type> Coll(matrixSize);
	farm->add_collector(&Coll);
	farm->set_scheduling_ondemand(0);
	std::vector<ff_node *> w;
	if (executeStrassen)
		for(unsigned int i=0;i<numWorkers;++i) w.push_back(new StrassenWorker<type>(matrixSize, i, C));
	else
		for(unsigned int i=0;i<numWorkers;++i) w.push_back(new Worker<type>(matrixSize, i, C));


	farm->add_workers(w);
	start_time();
	farm->run_and_wait_end();
	elapsed_time(Old Total time);
	std::cout << "Total ffTime (with matrix): ";
	std::cout << farm->ffTime();
	std::cout << "\n";
	return 0;
}
