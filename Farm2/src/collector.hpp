/*
 * collector.hpp
 *
 *  Created on: 08/ott/2014
 *      Author: alessandro
 */

#ifndef COLLECTOR_HPP_
#define COLLECTOR_HPP_

#include <ff/node.hpp>

using namespace ff;

template <typename NUM>
class Collector: public ff_node {
public:
	Collector(unsigned int size): ff_node(), size(size){

	}
	~Collector() {

	}
	void *svc(void *task) {
		NUM **matrix = (NUM**) task;
		//for(unsigned int i = 0; i < size; i++)
			//if (matrix[i][i] != 1) printf("Error\n");
		return GO_ON;
	}
private:
	unsigned int size;
};



#endif /* COLLECTOR_HPP_ */
