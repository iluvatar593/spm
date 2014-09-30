//============================================================================
// Name        : Test.cpp
// Author      : Alessandro
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "MatrixMultiplication.h"
using namespace std;

int main() {
	long A[16], B[16];
	for(int i = 0; i < 16; i++) {
		if(i == 0 || i == 5 || i == 10 || i == 15) A[i] = B[i] = 1;
		else A[i] = B[i] = 0;
	}
	MatrixMultiplication <long>matmul;
	matmul.multiply(A, B);

}
