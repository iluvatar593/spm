//============================================================================
// Name        : FarmWithMatrixParallelism.cpp
// Author      : Alessandro
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <ff/farm.hpp>
#include "worker.hpp"
#include "emitter.hpp"
#include "parallel_initializer.hpp"
using namespace ff;

#define TYPE float


int main(int argc, char **argv) {
	if(argc <= 4) {
		printUsage();
		return 1;
	}
	unsigned int streamLength = atoi(argv[1]);
	unsigned int matrixSize = atoi(argv[2]);
	unsigned int chunkSize = atoi(argv[3]);
	unsigned int numWorkers = atoi(argv[4]);
	if(streamLength == 0 || matrixSize == 0 || chunkSize == 0 || numWorkers == 0) {
		printUsage();
		return 1;
	}
	unsigned int bufferSize = 40; //todo: insert calculation
	input_buffer<TYPE> *A = new input_buffer<TYPE>(bufferSize);
	input_buffer<TYPE> *B = new input_buffer<TYPE>(bufferSize);
	initializeInParallel(A, B, matrixSize, bufferSize, 8);
	#if defined(__MIC__)
		if(argc <= 5 || (argc >= 6 && std::string(argv[5]) == "-SMic")) {
			const char worker_mapping[]="1, 5, 9, 13, 17, 21, 25, 29, 33, 37, 41, 45, 49, 53, 57, 61, 65, 69, 73, 77, 81, 85, 89, 93, 97, 101, 105, 109, 113, 117, 121, 125, 129, 133, 137, 141, 145, 149, 153, 157, 161, 165, 169, 173, 177, 181, 185, 189, 193, 197, 201, 205, 209, 213, 217, 221, 225, 229, 233, 0, 2, 6, 10, 14, 18, 22, 26, 30, 34, 38, 42, 46, 50, 54, 58, 62, 66, 70, 74, 78, 82, 86, 90, 94, 98, 102, 106, 110, 114, 118, 122, 126, 130, 134, 138, 142, 146, 150, 154, 158, 162, 166, 170, 174, 178, 182, 186, 190, 194, 198, 202, 206, 210, 214, 218, 222, 226, 230, 234, 237, 3, 7, 11, 15, 19, 23, 27, 31, 35, 39, 43, 47, 51, 55, 59, 63, 67, 71, 75, 79, 83, 87, 91, 95, 99, 103, 107, 111, 115, 119, 123, 127, 131, 135, 139, 143, 147, 151, 155, 159, 163, 167, 171, 175, 179, 183, 187, 191, 195, 199, 203, 207, 211, 215, 219, 223, 227, 231, 235, 238, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144, 148, 152, 156, 160, 164, 168, 172, 176, 180, 184, 188, 192, 196, 200, 204, 208, 212, 216, 220, 224, 228, 232, 236, 239";
			threadMapper::instance()->setMappingList(worker_mapping);
		}
	#endif
	Emitter<TYPE> E(A, B, bufferSize, streamLength, matrixSize, chunkSize);
	ff_farm<> * farm = new ff_farm<>(false, 0, 0, true,240,true);
	farm->add_emitter(&E);
	farm->set_scheduling_ondemand(0);
	std::vector<ff_node *> w;
	for(unsigned int i = 0; i < numWorkers; i++) w.push_back(new Worker<TYPE>(matrixSize));
	farm->add_workers(w);
	farm->run_and_wait_end();
	std::cout << "FARM:\t"<< numWorkers << "\t" << chunkSize << "\t" << streamLength << "\t";
	std::cout << farm->ffTime();
	std::cout << "\n";
}
