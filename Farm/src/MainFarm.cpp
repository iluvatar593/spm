/*
 * MainFarm.cpp
 *
 *  Created on: 30/set/2014
 *      Author: alessandro
 */

#include <iostream>
#include <stdlib.h>
#include <ff/farm.hpp>
#include "Worker.hpp"
#include "Emitter.hpp"
#include "DoubleEmitter.h"

/**
 * Arguments are:
 * 1 -> length of the stream
 * 2 -> size of the matrix (for now it is square)
 * 3 -> number of workers
 */
using namespace ff;
static inline void printUsage() {
	std::cout << "MainFarm usage:\n";
	std::cout << "./MainFarm lengthOfStream MatrixSize WorkerNumber\n";
}

int main(int argc, char* argv[]) {
	/** Check parameters */
	if(argc < 3) { printUsage(); return 1;}
	long streamLength = atoi(argv[0]);
	unsigned int matrixSize = atoi(argv[1]);
	int numWorkers = atoi(argv[2]);
	if(streamLength <= 0 || matrixSize <= 0 || numWorkers <= 0) { printUsage(); return 1;}
	DoubleEmitter *e = new DoubleEmitter(streamLength, matrixSize);
	ff_farm<> farm(true, matrixSize*matrixSize+numWorkers);
	farm.add_emitter(e);
	std::vector<ff_node *> w;
	Worker<int> worker();
	for(int i=0;i<numWorkers;++i) w.push_back(new Worker<int>());
	farm.add_workers(w);
	    // Now run the accelator asynchronusly
	    farm.run_then_freeze();
}


