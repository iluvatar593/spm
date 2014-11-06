//============================================================================
// Name        : FarmIntrinsicFloat.cpp
// Author      : atzoril
// Version     :
// Copyright   : Your copyright notice
// Description : Parallel implementation of a farm for the multiplication of a stream of matrices.
//				Uses floats inside.
//============================================================================

#include <ff/farm.hpp>
#include <stdlib.h>
#include "utils.hpp"
#include "utilsF.hpp"
#include "emitters.hpp"
#include "worker_float.hpp"
using namespace ff;

int main(int argc, const char** argv) {
	if(argc < 6) {
		printUsage();
		return -1;
	}
	bool tryanyway = (argc >= 7 && atoi(argv[6]) == 1);
	/** Parameters assignment */
	int mxw = atoi(argv[1]);
	int numWorkers = atoi(argv[2]);
	int streamLength = mxw * numWorkers;
	int n, oldn, m, oldm, k, oldk;
	n = oldn = atoi(argv[3]);
	k = oldk = atoi(argv[4]);
	m = oldm = atoi(argv[5]);

	/** Parameters checking */
	if(streamLength <= 0) {
		if(isatty(fileno(stdout)))printf("Stream length must be positive \n");
		printUsage();
		return -1;
	}
	if (numWorkers <= 0) {
		if(isatty(fileno(stdout)))printf("Workers must be positive \n");
		printUsage();
		return -1;
	}
	if(n <= 0 || k <= 0 || m <= 0) {
		if(isatty(fileno(stdout)))printf("Matrix dimensions must be greather than 0\n");
		printUsage();
		return -1;
	}
	if (k > TILE*31) {
		if(isatty(fileno(stdout)))printf("Sorry, k is too large! Try again with a value smaller or equal than %d\n", TILE*31);
		printUsage();
		return -1;
	}
	if(k%TILE > 0) {
		k+=TILE-k%TILE;
	}
	if(n%TILE>0) {
		n+=TILE-n%TILE;
	}
	if(m%TILE>0) {
		m+=TILE-m%TILE;
	}

	int bufferSize = calculateBufferSize(sizeof(float), n, k, m, numWorkers, streamLength, tryanyway);
	/** Initialize input buffers */
	if (bufferSize < streamLength) {
		if(isatty(fileno(stdout))) printf("Sorry, there's not enough memory for the specified stream length. There's space for %d couples of matrices\n", bufferSize);
	}
	if(bufferSize == 0) {
		if(isatty(fileno(stdout))) printf("I will try to allocate 6 couples of matrices\n");
		bufferSize = 6;
	}
	bufferSize = (bufferSize < 240) ? bufferSize: 240;
	float **aBuffer, **bBuffer;
	_MM_MALLOC(aBuffer, float**, sizeof(float*)*bufferSize);
	_MM_MALLOC(bBuffer, float**, sizeof(float*)*bufferSize);
	srand(time(NULL));
	/** Initialization of the whole input buffer */
	for(int i = 0; i < bufferSize; i++) {
		/** Allocate & initialize a couple of matrices */
		_MM_MALLOC(aBuffer[i], float*, sizeof(float)*n*k);
		initializeFloatMatrix(aBuffer[i], n, k);
		_MM_MALLOC(bBuffer[i], float*, sizeof(float)*k*m);
		initializeFloatMatrix(bBuffer[i], k, m);
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

	EmitterFloat E(aBuffer, bBuffer, streamLength, bufferSize);
	#if define(__MIC__)
		#define MAXWORKERS 240
	#else
		#define MAXWORKERS 16
	#endif
	ff_farm<> * farm = new ff_farm<>(false, 0, 0, true,MAXWORKERS,true);
	farm->add_emitter(&E);
	farm->set_scheduling_ondemand(0);
	std::vector<ff_node *> w;
	for(int i = 0; i < numWorkers; i++) w.push_back(new WorkerFloat(n, oldn, k, oldk, m, oldm));
	farm->add_workers(w);
	farm->run_and_wait_end();
	printf("Farm\t Float \t %d \t %d \t %d \t %d \t %d \t %f\n", streamLength, numWorkers, n, k, m, farm->ffTime());

}
