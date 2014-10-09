/*
 * parallel_initializer.hpp
 *	Use this to avoid waiting for a sequential initialization of the stream.
 *  Created on: 09/ott/2014
 *      Author: alessandro
 */

#ifndef PARALLEL_INITIALIZER_HPP_
#define PARALLEL_INITIALIZER_HPP_

#include <ff/farm.hpp>
#include "input_buffer.hpp";
template<typename NUM>
void initializeInParallel(input_buffer<NUM> *a, input_buffer<NUM>*b, unsigned int matrixSize, unsigned int bufferSize) {

}




#endif /* PARALLEL_INITIALIZER_HPP_ */
