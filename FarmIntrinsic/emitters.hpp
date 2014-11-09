/*
 * emitters.hpp
 *
 *  Created on: 05/nov/2014
 *      Author: alessandro
 */

#ifndef EMITTER_HPP_
#define EMITTER_HPP_

#include <ff/node.hpp>
#include "utils.hpp"
#include "utilsF.hpp"
#include "utilsD.hpp"

using namespace ff;
/**
 * EmitterDouble: generates a stream of tasks representing a matrix multiplication.
 * If the buffer is limited, generates all possible combinations of couples.
 *
 * After generating the full stream, it terminates and deallocates the input buffers.
 */
class EmitterDouble : public ff_node {
public:
	EmitterDouble(double **__restrict__ aBuff, double **__restrict__ bBuff, int streamLength, int bufferSize):
		aBuff(aBuff), bBuff(bBuff), streamLength(streamLength), bufferSize(bufferSize), aOffset(0), bOffset(0){}

	~EmitterDouble() {
		for(int i = 0; i < bufferSize; i++) {
			_MM_FREE(aBuff[i]);
			_MM_FREE(bBuff[i]);
		}
		_MM_FREE(aBuff);
		_MM_FREE(bBuff);
	}
	void * svc(void *) {
		for(int s = 0; s < streamLength; s++) {
			taskDouble_t *t;
			_MM_MALLOC(t, taskDouble_t *, sizeof(taskDouble_t));
			t->a = aBuff[(s+aOffset)%bufferSize];
			t->b = bBuff[(s+bOffset)%bufferSize];
			t->num = s;
			/** Generates all combinations of matrices in the buffer size */
			if(s > 0 && s % bufferSize == 0) {
				aOffset++;
				if (aOffset%bufferSize == 0) bOffset++;
			}
			ff_send_out(t);
		}
		return NULL;
	}

private:
	double **__restrict__ aBuff, **__restrict__ bBuff;
	int streamLength, bufferSize, aOffset, bOffset;
};

/**
 * EmitterFloat: generates a stream of tasks representing a matrix multiplication.
 * If the buffer is limited, generates all possible combinations of couples.
 *
 * After generating the full stream, it terminates and deallocates the input buffers.
 */
class EmitterFloat : public ff_node {
public:
	EmitterFloat(float **__restrict__ aBuff, float **__restrict__ bBuff, int streamLength, int bufferSize):
		aBuff(aBuff), bBuff(bBuff), streamLength(streamLength), bufferSize(bufferSize), aOffset(0), bOffset(0), ff_node(){
	}

	~EmitterFloat() {
		for(int i = 0; i < bufferSize; i++) {
			_MM_FREE(aBuff[i]);
			_MM_FREE(bBuff[i]);
		}
		_MM_FREE(aBuff);
		_MM_FREE(bBuff);
	}
	void * svc(void *) {
		for(int s = 0; s < streamLength; s++) {
			taskFloat_t *t;
			_MM_MALLOC(t, taskFloat_t *, sizeof(taskFloat_t));
			t->a = aBuff[(s+aOffset)%bufferSize];
			t->b = bBuff[(s+bOffset)%bufferSize];
			t->num = s;
			/** Generates all combinations of matrices in the buffer size */
			if(s > 0 && s % bufferSize == 0) {
				aOffset++;
				if (aOffset%bufferSize == 0) bOffset++;
			}
			ff_send_out(t);
		}
		return NULL;
	}

private:
	float **__restrict__ aBuff, **__restrict__ bBuff;
	int streamLength, bufferSize, aOffset, bOffset;
};



#endif /* EMITTER_HPP_ */
