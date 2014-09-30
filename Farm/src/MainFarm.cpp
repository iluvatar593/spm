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
	if(argc < 3) { printUsage(); return 1;};
	long streamLength = atol(argv[1]);
	int matrixSize = atoi(argv[2]);
	int numWorkers = atoi(argv[3]);
	for(int i = 0; i < argc; i++) {
		printf("%d --> %s\n",i, argv[i]);
	}
	if(streamLength <= 0 || matrixSize <= 0 || numWorkers <= 0) {
		printUsage();
		return 1;
	}

	ff_farm<> farm;
	DoubleEmitter E(streamLength, matrixSize);
	farm.add_emitter(&E);
	std::vector<ff_node *> w;
	for(int i=0;i<numWorkers;++i) w.push_back(new Worker<double>());
	farm.add_workers(w);
	farm.run_and_wait_end();
	std::cout << "Finished\n";
}


