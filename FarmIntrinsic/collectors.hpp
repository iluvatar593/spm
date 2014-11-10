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

template<typename NUM>
class FarmOutput {
public:
	FarmOutput(NUM* matrix, int num) : matrix(matrix), num(num) {}
	~FarmOutput();
	NUM * matrix;
	int num;
};


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
		start_time()
		/** Contains id of the sender, id of the matrix and the elements*/
		if(mt == NULL) return NULL;
		workerOutput_t<NUM> *matrix = (workerOutput_t<NUM>*) mt;
		//count how many chunks have been received successfulyy
		if(oldn > n/2) received[matrix->worker] = !received[matrix->worker];
		if(!received[matrix->worker]) count++;
		elapsed_time(Collector);
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
		//count how many chunks have been received successfulyy
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


template<typename NUM>
class PedanticCollector : public ff_node {
public:
	PedanticCollector(int numWorkers, int n, int oldn, int m, int oldm) : ff_node(),
	numWorkers(numWorkers), n(n), oldn(oldn), k(k), oldk(k), m(m), oldm(oldm){
			_MM_MALLOC(m1, NUM*, sizeof(NUM)*n*m);
			_MM_MALLOC(m2, NUM*, sizeof(NUM)*n*m);
			_MM_MALLOC(matrixSample, NUM**, sizeof(NUM*)*numWorkers*2);
			for(int i = 0; i < 2*numWorkers; i++) _MM_MALLOC(matrixSample[i], NUM*, sizeof(NUM)*oldn*oldm);
			for(int i = 0; i < numWorkers; i++) offset[i] = 0;
	}
	~PedanticCollector() {
		delete[] received;
		for(int i = 0; i < numWorkers; i++) _MM_FREE(matrixSample[i]);
		_MM_FREE(matrixSample);
	}

	void *svc(void * mt) {
		if(mt == NULL) return NULL;
		workerOutput_t<NUM> *matrix = (workerOutput_t<NUM>*) mt;
		int wId = matrix->worker;
		int elementId = matrix->id;

		NUM* m;
		if first
			m = m1;
		else
			m = m2;

		m = memcpy(matrix->matrixChunk);

		NUM *__restrict__ rcvd = matrix->matrixChunk;
		if( oldn <= n/2) { //copy in first chunk!
			for(int i = 0; i < MIN(oldn, n/2); i++) {
					for(int j = 0; j < oldm; j++) {
						matrixSample[2*wId+offset[wId]][i*oldm+j] = rcvd[i*m+j];
					}
			}
			if(oldn<=n/2) {
				return new FarmOutput<NUM>(matrixSample[wId], elementId);
			}
		} else {
			for(int i = n/2; i < oldn; i++) {
				for(int j = 0; j < oldm; j++) {
					matrixSample[wId][i*oldm+j] = rcvd[i*m+j];
				}
			}
			return new FarmOutput<NUM>(matrixSample[wId], elementId);
		}


		return GO_ON;
	}
private:
	int numWorkers, n, oldn, m, oldm;
	NUM **matrixSample;
	NUM * m1, *m2;
};

#endif /* COLLECTORS_HPP_ */
