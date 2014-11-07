//============================================================================
// Name        : FarmIntrinsicStrassen.cpp
// Author      : atzoril
// Version     :
// Copyright   : Your copyright notice
// Description : Parallel implementation of a farm for the multiplication of a stream of matrices using the Strassen decomposition.
//============================================================================

#include <ff/farm.hpp>
#include <stdlib.h>
#include "utils.hpp"
#include "emitters.hpp"
#include "worker_strassen.hpp"
#include "collectors.hpp"

using namespace ff;

template<typename NUM>
void initializeMatrix(NUM *matrix, int rows, int cols) {
	for(int i = 0; i < rows; i++) {
		for(int j = 0; j < cols; j++) {
			matrix[i*cols+j] = ((NUM) (rand()%50)/(rand()%20+1.1));
		}
	}
}

int main(int argc, const char** argv) {
	if(argc < 4) {
		printUsage_S();
		return -1;
	}
	bool tryanyway = (argc >= 5 && atoi(argv[4]) == 1);
	/** Parameters assignment */
	int mxw = atoi(argv[1]);
	int numWorkers = atoi(argv[2]);
	int streamLength = mxw * numWorkers;
	int size, oldsize;
	size = oldsize = atoi(argv[3]);

	/** Parameters checking */
	if(streamLength <= 0) {
		if(isatty(fileno(stdout)))printf("Stream length must be positive \n");
		printUsage_S();
		return -1;
	}
	if (numWorkers <= 0) {
		if(isatty(fileno(stdout)))printf("Workers must be positive \n");
		printUsage_S();
		return -1;
	}
	if(size <= 0) {
		if(isatty(fileno(stdout)))printf("Matrix dimensions must be greather than 0\n");
		printUsage_S();
		return -1;
	}
	if (size > 2*TILE*31) {
		if(isatty(fileno(stdout)))printf("Sorry, size is too large! Try again with a value smaller or equal to %d\n", 2*TILE*31);
		printUsage_S();
		return -1;
	}
	if(size%(2*TILE) > 0) {
		size+=(2*TILE)-size%(2*TILE);
	}

	int bufferSize = calculateBufferSize(sizeof(double), size, size, size, numWorkers, streamLength, tryanyway);
	/** Initialize input buffers */
	if (bufferSize < streamLength) {
		if(isatty(fileno(stdout))) printf("Sorry, there's not enough memory for the specified stream length. There's space for %d couples of matrices\n", bufferSize);
	}
	if(bufferSize == 0) {
		if(isatty(fileno(stdout))) printf("I will try to allocate %d couples of matrices\n", MCOUPLES);
		bufferSize = MCOUPLES;
	}
	bufferSize = (bufferSize < 240) ? bufferSize: 240;
	TYPE **aBuffer, **bBuffer;
	_MM_MALLOC(aBuffer, TYPE**, sizeof(TYPE*)*bufferSize);
	_MM_MALLOC(bBuffer, TYPE**, sizeof(TYPE*)*bufferSize);
	srand(time(NULL));
	/** Initialization of the whole input buffer */
	for(int i = 0; i < bufferSize; i++) {
		/** Allocate & initialize a couple of matrices */
		_MM_MALLOC(aBuffer[i], TYPE*, sizeof(TYPE)*size*size);
		initializeMatrix<TYPE>(aBuffer[i], size, size);
		_MM_MALLOC(bBuffer[i], TYPE*, sizeof(TYPE)*size*size);
		initializeMatrix<TYPE>(bBuffer[i], size, size);
		int progress = (i*100)/bufferSize;
		if(progress % 10 == 0){
			if(isatty(fileno(stdout))) {
				printf("\rInitialization: %d %%", progress);
				fflush(stdout);
			}
		}
	}
	if(isatty(fileno(stdout)))printf("\rInitialization: 100%%\n");

	/** Mic mapping */
	#if defined(__MIC__)
		const char worker_mapping[]="1, 5, 9, 13, 17, 21, 25, 29, 33, 37, 41, 45, 49, 53, 57, 61, 65, 69, 73, 77, 81, 85, 89, 93, 97, 101, 105, 109, 113, 117, 121, 125, 129, 133, 137, 141, 145, 149, 153, 157, 161, 165, 169, 173, 177, 181, 185, 189, 193, 197, 201, 205, 209, 213, 217, 221, 225, 229, 233, 0, 2, 6, 10, 14, 18, 22, 26, 30, 34, 38, 42, 46, 50, 54, 58, 62, 66, 70, 74, 78, 82, 86, 90, 94, 98, 102, 106, 110, 114, 118, 122, 126, 130, 134, 138, 142, 146, 150, 154, 158, 162, 166, 170, 174, 178, 182, 186, 190, 194, 198, 202, 206, 210, 214, 218, 222, 226, 230, 234, 237, 3, 7, 11, 15, 19, 23, 27, 31, 35, 39, 43, 47, 51, 55, 59, 63, 67, 71, 75, 79, 83, 87, 91, 95, 99, 103, 107, 111, 115, 119, 123, 127, 131, 135, 139, 143, 147, 151, 155, 159, 163, 167, 171, 175, 179, 183, 187, 191, 195, 199, 203, 207, 211, 215, 219, 223, 227, 231, 235, 238, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144, 148, 152, 156, 160, 164, 168, 172, 176, 180, 184, 188, 192, 196, 200, 204, 208, 212, 216, 220, 224, 228, 232, 236, 239";
		threadMapper::instance()->setMappingList(worker_mapping);
	#endif
	if(isatty(fileno(stdout)))printf("Starting\n");

	Emitter<TYPE> E(aBuffer, bBuffer, streamLength, bufferSize);
	ff_node * C;

	if (argc >= 6 && std::string(argv[5]) == "sampler") {
		if(isatty(fileno(stdout))) printf("Sampler collector selected\n");
		C = new SamplerCollector<double>(numWorkers, size, size, size, size, size, size, streamLength);
	} else if(argc >= 6 && std::string(argv[5]) == "pedantic") {
		if(isatty(fileno(stdout))) printf("Pedantic collector selected\n");
		C = new PedanticCollector<double>(numWorkers, size, size, size, size, size, size, streamLength);
	} else { //default
		if(isatty(fileno(stdout))) printf("Dummy collector selected\n");
		C = new DummyCollector<double>(numWorkers, size, size, size, size, size, size, streamLength);
	}
	printf("TILE IS %d\n", TILE);
	printf("ALIGNMENT IS %d\n", ALIGNMENT);
	ff_farm<> * farm = new ff_farm<>(false, 0, 0, true, MAXWORKERS, true);
	farm->add_emitter(&E);
	farm->set_scheduling_ondemand(0);
	std::vector<ff_node *> w;
	for(int i = 0; i < numWorkers; i++) w.push_back(new WorkerStrassen<TYPE>(size, oldsize, i));
	farm->add_workers(w);
	farm->add_collector(C);
	farm->run_and_wait_end();
	printf("Strassen\t %d \t %d \t %d \t %f\n", streamLength, numWorkers, size, farm->ffTime());


}
