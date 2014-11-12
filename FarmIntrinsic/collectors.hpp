/*
 * collectors.hpp
 *
 *  Created on: 06/nov/2014
 *      Author: alessandro
 */

#ifndef COLLECTORS_HPP_
#define COLLECTORS_HPP_


#include <ff/node.hpp>
#include <stdio.h>
#include <stdlib.h>

using namespace ff;

//This class represents what the collector sends in output.
template<typename NUM>
class FarmOutput {
public:
	FarmOutput(NUM* matrix, int num) : matrix(matrix), num(num) {}
	~FarmOutput();
	NUM * matrix;
	int num;
};

/*
 * DummyCollector (used as default)
 *
 *  - counts how many matrix-halves were received
 *  - checks if every matrix is received
 *  - checks if every stream element is received
 *
 */
template<typename NUM>
class DummyCollector : public ff_node {
public:
	DummyCollector(int numWorkers, int n, int oldn, int k, int oldk, int m, int oldm, int streamLength): ff_node(),
		numWorkers(numWorkers), n(n), oldn(oldn), k(k), oldk(k), m(m), oldm(oldm), streamLength(streamLength){
		received = new bool[numWorkers]();
		count = 0;
	}
	~DummyCollector() {
		delete[] received;
	}

	void *svc(void *mt) {
		/** Contains id of the sender, id of the matrix and the elements*/
		if(mt == NULL) return NULL;
		workerOutput_t<NUM> *matrix = (workerOutput_t<NUM>*) mt;
		//count how many chunks have been received successfully
		if(oldn > n/2) received[matrix->worker] = !received[matrix->worker];
		if(!received[matrix->worker]) count++;
		return GO_ON;
	}
	/** On end, check that all matrix chunks have been received correctly. */
	void svc_end() {
		for(int i = 0; i < numWorkers; i++) {
			if(received[i]) printf("A chunk is missing from worker %d\n", i);
		}
		if (count != streamLength) printf("Received %d matrices instead of %d\n", count, streamLength);
	}
private:
	bool *received;
	int numWorkers, n, oldn, k, oldk, m, oldm, streamLength;
	int count;
};

/*
 * SamplerCollector
 *
 *  - randomly selects the id of a worker
 *  - locally copy (and send out) the matrices calculated by the selected worker
 *
 */
template<typename NUM>
class SamplerCollector : public ff_node {
public:
	SamplerCollector(int numWorkers, int n, int oldn, int k, int oldk, int m, int oldm, int streamLength, bool strass=false) : ff_node(),
	numWorkers(numWorkers), n(n), oldn(oldn), k(k), oldk(k), m(m), oldm(oldm), streamLength(streamLength){
			received = new bool[numWorkers]();
			if(strass)
				for(int i = 0; i < numWorkers; i++)
					received[i] = true;
			randomWorker = rand()%numWorkers;
			_MM_MALLOC(matrixSample, NUM*, sizeof(NUM)*oldn*oldm);
	}
	~SamplerCollector() {
		delete[] received;
		_MM_FREE(matrixSample);
	}

	void *svc(void * mt) {
		if(mt == NULL) return NULL;
		workerOutput_t<NUM> *matrix = (workerOutput_t<NUM>*) mt;
		int wId = matrix->worker;
		int elementId = matrix->id;
		//count how many chunks have been received successfully
		received[wId] = !received[wId];
		NUM *__restrict__ rcvd = matrix->matrixChunk;
		if(wId == randomWorker) { //Sampling process!
			if(!received[wId] || oldn <= n/2) { //copy in first chunk!
				for(int i = 0; i < MIN(oldn, n/2); i++) {
					for(int j = 0; j < oldm; j++) {
						matrixSample[i*oldm+j] = rcvd[i*m+j];
					}
				}
				if(oldn <= n/2) return new FarmOutput<NUM>(matrixSample, elementId);
			} else {
				for(int i = n/2; i < oldn; i++) {
					for(int j = 0; j < oldm; j++) {
						matrixSample[i*oldm+j] = rcvd[i*m+j];
					}
				}
				return new FarmOutput<NUM>(matrixSample, elementId);
			}
		}
		return GO_ON;
	}
private:
	bool *received;
	int numWorkers, n, oldn, k, oldk, m, oldm;
	int streamLength, randomWorker;
	NUM *matrixSample;
};

/*
 * PedanticCollector (slows down the entire farm! => better if used only in debugging mode)
 *
 *  - copies every matrix received!
 *
 */
template<typename NUM>
class PedanticCollector : public ff_node {
public:
	PedanticCollector(int n, int oldn, int m, int oldm) : ff_node(),
	n(n), oldn(oldn), m(m), oldm(oldm){
			_MM_MALLOC(m1, NUM*, sizeof(NUM)*oldn*oldm);
			_MM_MALLOC(m2, NUM*, sizeof(NUM)*oldn*oldm);
	}
	~PedanticCollector() {
		_MM_FREE(m1);
		_MM_FREE(m2);
	}

	void *svc(void * mt) {
		if(mt == NULL) return NULL;
		workerOutput_t<NUM> *matrix = (workerOutput_t<NUM>*) mt;
		NUM *__restrict__ tmp = matrix->matrixChunk;
		int elementId = matrix->id;

		NUM* __restrict__ outm;
		if (first)
			outm = m1;
		else
			outm = m2;

		for(int i=0; i<oldn; i++){
			for(int j=0; j<oldm; j++){
				outm[i*oldm+j] = tmp[i*m+j];
			}
		}

		first = !first;
		return new FarmOutput<NUM>(outm, elementId);
	}
private:
	int n, oldn, m, oldm;
	NUM * m1, *m2;
	bool first = true;
};

#endif /* COLLECTORS_HPP_ */
