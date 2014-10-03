/*
 * stream.hpp
 *
 *  Created on: 03/ott/2014
 *      Author: alessandro
 */

#ifndef STREAM_HPP_
#define STREAM_HPP_

#include "Buffer.hpp"
template<typename NUM>
class stream: public matrix_buffer {
public:
	stream(unsigned int streamLength, unsigned int size, unsigned int rows, unsigned int cols=rows):matrix_buffer(size, rows, cols),start(0),streamLength(streamLength),offset(2){};
	~stream(unsigned int size, unsigned int rows, unsigned int cols){};
	FarmTask<NUM>* getNext() {

		return nullptr;
	}
private:
	unsigned int start, streamLength,offset;
};

//class returning tasks.






#endif /* STREAM_HPP_ */
