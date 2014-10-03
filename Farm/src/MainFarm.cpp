/*
 * MainFarm.cpp
 *
 *  Created on: 30/set/2014
 *      Author: alessandro
 */

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <ff/farm.hpp>
#include <ff/pipeline.hpp>
#include "Utils.h"
#include "Worker.hpp"
#include "StreamGenerator.hpp"
#include "Emitter.hpp"
#include "buffers/Buffer.hpp"

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
class EmitterTrial: public ff_node {
public:
	EmitterTrial(unsigned int streamLength):ff_node() {length=streamLength;printf("emitter init\n");}
	~EmitterTrial(){};
	void * svc(void *) {
		for(unsigned int i = 0; i < length; i++) {
			ff_send_out((void*) (i+1));
			printf("I sent out %d\n", i);
		}
		return NULL;
	}
private:
	unsigned int length;
};


class WorkerTrial: public ff_node {
public:
	WorkerTrial():ff_node(){printf("Worker init\n");};
	~WorkerTrial(){};
	void * svc (void * task) {
		printf("Received\n");
		sleep(10);
		return GO_ON;
	}
};
int main(int argc, char* argv[]) {
	/** Check parameters */
	if(argc < 3) { printUsage(); return 1;};
	int streamLength = atoi(argv[1]);
	int matrixSize = atoi(argv[2]);
	int numWorkers = atoi(argv[3]);
	for(int i = 0; i < argc; i++) {
		printf("%d ------> %s\n",i, argv[i]);
	}
	if(streamLength <= 0 || matrixSize <= 0 || numWorkers <= 0) {
		printUsage();
		return 1;
	}
	#if defined(__MIC__)
    	const char worker_mapping[]="1, 5, 9, 13, 17, 21, 25, 29, 33, 37, 41, 45, 49, 53, 57, 61, 65, 69, 73, 77, 81, 85, 89, 93, 97, 101, 105, 109, 113, 117, 121, 125, 129, 133, 137, 141, 145, 149, 153, 157, 161, 165, 169, 173, 177, 181, 185, 189, 193, 197, 201, 205, 209, 213, 217, 221, 225, 229, 233, 0, 2, 6, 10, 14, 18, 22, 26, 30, 34, 38, 42, 46, 50, 54, 58, 62, 66, 70, 74, 78, 82, 86, 90, 94, 98, 102, 106, 110, 114, 118, 122, 126, 130, 134, 138, 142, 146, 150, 154, 158, 162, 166, 170, 174, 178, 182, 186, 190, 194, 198, 202, 206, 210, 214, 218, 222, 226, 230, 234, 237, 3, 7, 11, 15, 19, 23, 27, 31, 35, 39, 43, 47, 51, 55, 59, 63, 67, 71, 75, 79, 83, 87, 91, 95, 99, 103, 107, 111, 115, 119, 123, 127, 131, 135, 139, 143, 147, 151, 155, 159, 163, 167, 171, 175, 179, 183, 187, 191, 195, 199, 203, 207, 211, 215, 219, 223, 227, 231, 235, 238, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144, 148, 152, 156, 160, 164, 168, 172, 176, 180, 184, 188, 192, 196, 200, 204, 208, 212, 216, 220, 224, 228, 232, 236, 239";
    	threadMapper::instance()->setMappingList(worker_mapping);
	#endif
    matrix_buffer<int> *b = new matrix_buffer<int>(100, 100, 100);
    for(int i = 0; i < 100; i++) {
    	int** m = new int*[100]();
    	for(int j = 0; j < 100; j++) {
    		m[j] = new int[100]();
    	}
    	b->add(m);
    }
    printf("BOH!\n");
    delete b;
    printf("FREED\n");
    return 1;

    //ff_pipeline *stream = new ff_pipeline;
    //IntGenerator* gen = new IntGenerator(streamLength, matrixSize);

    //stream->add_stage(gen);
    IntGenerator E(streamLength, matrixSize);
	ff_farm<> * farm = new ff_farm<>(false, 0, 0, true, 100, true);
	farm->add_emitter(&E);
	std::vector<ff_node *> w;
	for(int i=0;i<numWorkers;++i) w.push_back(new Worker<int>());
	farm->add_workers(w);
	farm->run_and_wait_end();
	//farm->offload(&w);
	//stream->add_stage(&farm);
	//stream->run_and_wait_end();
	std::cout << "Total time: ";
	std::cout << farm->ffTime();
	std::cout << "\nFinished\n";
}


