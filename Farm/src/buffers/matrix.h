/*
 * matrix.h
 *
 *  Created on: 02/ott/2014
 *      Author: alessandro
 */

#ifndef MATRIX_H_
#define MATRIX_H_
#include <stdlib.h>
#include <memory>

template <typename NUM>
class matrix {
public:
	matrix(unsigned int rows, unsigned int cols): rows(rows), cols(cols){}
	inline unsigned int getRows() {return rows;}
	inline unsigned int getCols(){return cols;}
private:
	unsigned int rows;
	unsigned int cols;
};

template<typename NUM>
class simple_matrix:public matrix<NUM> {
public:

	//simple constructor
	simple_matrix(unsigned int rows,unsigned int cols):matrix<NUM>(rows, cols){
		//matrix initialization
		M = new NUM*[rows]();
		for(int i = 0; i < rows; i++)
			M[i] = new NUM[rows]();
	}
	//constructor from standard pre-allocated matrix
	simple_matrix(unsigned int rows, unsigned int cols, NUM** m):matrix<NUM>(rows, cols) {
		M = m;
	}
	NUM* operator[](int row) {
		return M[row];
	}

	std::unique_ptr<NUM**> getMatrix() {
		NUM **m = M;
		M = nullptr;
		return m;
	}
	~simple_matrix() {
		printf("Ciao\n");
		for(unsigned int i = 0; i < this->getRows(); i++) delete[] M[i];
		delete[] M;
	}
private:
	NUM** M;
};

#endif /* MATRIX_H_ */
