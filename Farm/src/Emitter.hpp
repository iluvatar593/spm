/*
 * Emitter.hpp
 *
 *  Created on: 30/set/2014
 *      Author: atzoril
 */

#include <vector>
#include <iostream>
#include <cstdlib>
#include <ff/node.hpp>
#include "Utils.h"
#include "Task.hpp"

using namespace ff;

#ifndef EMITTER_HPP_
#define EMITTER_HPP_

class Emitter: public ff_node {
public:
    Emitter():ff_node() {}
    void* svc(void* task) {
    	return task;
    }
    ~Emitter(){}
};

#endif /* EMITTER_HPP_ */
