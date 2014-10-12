/*
 * utils.hpp
 *
 *  Created on: 09/ott/2014
 *      Author: alessandro
 */

#ifndef UTILS_HPP_
#define UTILS_HPP_


#include <chrono>
#include <stdio.h>

#if defined(__MIC__)
#else
#define restrict
#endif

template <typename NUM>
struct pointerTask {
	NUM **restrict A;
	NUM **restrict B;
};

template <typename NUM>
struct LpointerTask {
	NUM *restrict A;
	NUM *restrict B;
};

/** Timing macro (for now, one for scope). Call start_time() for initializing;
 * elapsed_time(STRING) prints the string with the time spent in the procedure (in uS).
 * */
#define start_time() \
	auto start = std::chrono::high_resolution_clock::now();
/** Shows the elapsed time. See start_time for usage*/
#define elapsed_time(STRING) \
	auto elapsed = std::chrono::high_resolution_clock::now() - start; \
	long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count(); \
	printf(#STRING":%lld\n", microseconds);

#define SPARE_MEMORY 4*1024*1024*1024 //4gb
/** Buffer size */
unsigned inline int calculateBufferSize(size_t s, unsigned int numworkers, unsigned int msize, unsigned int slength) {
	unsigned long squaresize = msize*msize*s;
	long workersMemory = squaresize*2*numworkers;
	long totM = (long) SPARE_MEMORY;
	long spareMemory = (totM - workersMemory);
	unsigned long buffSize = spareMemory/(squaresize*2);
	printf("Buffer size is %ld\n", buffSize);
	return (slength < ((unsigned int) buffSize)) ? slength : (unsigned int)buffSize;
}

/** Show usage */
inline void printUsage() {
	std::cout << "MainFarm usage:\n";
	std::cout << "./MainFarm lengthOfStream MatrixSize WorkerNumber [-Aalgorithm] -S[scheduling]\n";
	std::cout << "algorithm is one between IKJ, Strassen and DC (divide and conquer). If not specified IKJ is used.\n";
	std::cout << "scheduling works only on the mic; -Smic or blank is the default which schedules on the cores" <<
			" in a round robin fashion. \n -Ssystem relies on the OS scheduling\n";

}

#endif /* UTILS_HPP_ */
