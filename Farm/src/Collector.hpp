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
		printf("Iter: %d \n", ++ciao);
		if(m == NULL) return NULL;
		//delete m;
		return GO_ON;
		simple_matrix<NUM> * matrix = (simple_matrix<NUM>*) m;
		//Check if it is the identity
		for(unsigned int i = 0; i < matrix->getRows(); i++)
			if((*matrix)[i][i] != 1) {printf("ERROR!\n"); return GO_ON;}
		printf("Correct\n");
		delete matrix;
		return GO_ON;
	}

private:
	int ciao = 0;
};



#endif /* COLLECTOR_HPP_ */
