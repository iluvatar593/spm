/*
 * Utils.h
 *
 *  Created on: 01/ott/2014
 *      Author: alessandro
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <chrono>

#define start_time() \
	auto start = std::chrono::high_resolution_clock::now();

#define elapsed_time(STRING) \
	auto elapsed = std::chrono::high_resolution_clock::now() - start; \
	long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count(); \
	printf("#STRING:%lld\n", microseconds);


typedef struct {
	int a, b, c, d, e;
} task_f;




#endif /* UTILS_H_ */
