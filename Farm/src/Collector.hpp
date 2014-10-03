/*
 * Collector.hpp
 *
 *  Created on: 03/ott/2014
 *      Author: alessandro
 */

#ifndef COLLECTOR_HPP_
#define COLLECTOR_HPP_
#include <ff/node.hpp>
template<typename NUM>
class Collector: public ff_node {
public:
	void *svc(void* m) {

		if(m == NULL) return NULL;
		simple_matrix<NUM> * matrix = (simple_matrix<NUM>*) m;
		//Check if it is the identity
		for(unsigned int i = 0; i < matrix->getRows(); i++)
			if((*matrix)[i][i] != 1) printf("ERROR!\n");
		delete matrix;
		return GO_ON;
	}
};



#endif /* COLLECTOR_HPP_ */
