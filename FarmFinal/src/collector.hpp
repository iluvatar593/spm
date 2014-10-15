/*
 * collector.hpp
 *
 *  Created on: 15/ott/2014
 *      Author: alessandro
 */

#ifndef COLLECTOR_HPP_
#define COLLECTOR_HPP_

#include <ff/node.hpp>
#include "workerOutput.hpp"
using namespace ff;

template<typename NUM>
class Collector: public ff_node {
public:
	Collector(unsigned int numWorkers, unsigned int size, NUM*** outB):ff_node(), numWorkers(numWorkers), size(size), out(outB) {
		rcvMatrix = new bool[numWorkers]();
	}
	~Collector() {}

	void *svc(void *matrix) {
		if(matrix == NULL) return NULL;
		workerOutput<NUM> * o = (workerOutput<NUM> *) matrix;
		if(!rcvMatrix[o->id]) { //received first chunk.
			for(unsigned int i = 0; i < size/2; i++)
				memcpy(out[numWorkers+o->id][i], o->output[i], sizeof(NUM)*size);
			rcvMatrix[o->id] = true;
		} else { //received second chunk
			for(unsigned int i = 0; i < size/2; i++)
				memcpy(out[numWorkers+o->id][i+size/2], o->out[i], sizeof(NUM)* size);
			//return out[numWorkers+o->id] --> returns the calcualted matrix
			rcvMatrix[o->id] = false;
		}
		return GO_ON;
	}
private:
	unsigned int numWorkers, size;
	NUM***out;
	bool *rcvMatrix;
};




#endif /* COLLECTOR_HPP_ */
