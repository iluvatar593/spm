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
#include <assert.h>
#include <stdlib.h>
#include <sys/mman.h>
#define HUGE_PAGE_SIZE (2*1024*1024)
#define ALIGN_TO_PAGE_SIZE(x) \
		(((x) + HUGE_PAGE_SIZE - 1) / HUGE_PAGE_SIZE * HUGE_PAGE_SIZE)

#if defined(__MIC__)
#else
#define restrict
#endif

template<typename NUM>
class task {
	NUM**A;
	NUM**B;
	NUM**C;
	unsigned int start;
	unsigned int end;
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
	//printf("Buffer size is %ld\n", buffSize);
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

inline void *malloc_huge_pages(size_t size)
{
	if (size < HUGE_PAGE_SIZE) {
		return malloc(size);
	}
	// Use 1 extra page to store allocation metadata
	// (libhugetlbfs is more efficient in this regard)
	size_t real_size = ALIGN_TO_PAGE_SIZE(size + HUGE_PAGE_SIZE);

	char *ptr = (char *)mmap(NULL, real_size, PROT_READ | PROT_WRITE,
	MAP_PRIVATE | MAP_ANONYMOUS |
	MAP_POPULATE | MAP_HUGETLB, -1, 0);
	if (ptr == MAP_FAILED) {
	// The mmap() call failed. Try to malloc instead
	ptr = (char *)malloc(real_size);
	if (ptr == NULL) return NULL;
	real_size = 0;
	}
	// Save real_size since mmunmap() requires a size parameter
	*((size_t *)ptr) = real_size;
	// Skip the page with metadata
	return ptr + HUGE_PAGE_SIZE;
}
inline void free_huge_pages(void *ptr)
{
	if (ptr == NULL) return;
	// Jump back to the page with metadata
	void *real_ptr = (char *)ptr - HUGE_PAGE_SIZE;
	// Read the original allocation size
	size_t real_size = *((size_t *)real_ptr);
	assert(real_size % HUGE_PAGE_SIZE == 0);
	if (real_size != 0)
	// The memory was allocated via mmap()
	// and must be deallocated via munmap()
	munmap(real_ptr, real_size);
	else
	// The memory was allocated via malloc()
	// and must be deallocated via free()
	free(real_ptr);
}


unsigned int roundUp(int numToRound, int multiple)
{
 if(multiple == 0)
 {
  return numToRound;
 }

 int remainder = numToRound % multiple;
 if (remainder == 0)
  return numToRound;
 return numToRound + multiple - remainder;
}


#endif /* UTILS_HPP_ */
