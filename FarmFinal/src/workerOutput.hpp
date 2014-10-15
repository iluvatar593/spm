/*
 * workerOutput.hpp
 *
 *  Created on: 15/ott/2014
 *      Author: alessandro
 */

#ifndef WORKEROUTPUT_HPP_
#define WORKEROUTPUT_HPP_

template <typename NUM>
class workerOutput {
public:
	workerOutput(NUM **restrict output, unsigned int id): output(output), id(id) {}
	~workerOutput(){}
	NUM** output;
	unsigned int id;
};



#endif /* WORKEROUTPUT_HPP_ */
