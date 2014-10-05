//============================================================================
// Name        : Test.cpp
// Author      : Alessandro
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "MatrixMultiplication.h"
#include <chrono>

#define start_time() \
	auto start = std::chrono::high_resolution_clock::now();

#define elapsed_time(STRING) \
	auto elapsed = std::chrono::high_resolution_clock::now() - start; \
	long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count(); \
	printf(#STRING":%lld\n", microseconds);

#define size 1024
#define NORMAL
int main() {
	#ifdef LINEAR
	int *A=new int[1024*1024]();
	int *B = new int[1024*1024]();
	int *C = new int[1024*1024]();
	for(int i = 0; i < 1024; i++) A[i*size+i] = B[i*size+i] = 1;
	start_time();
	for(int i = 0; i < size; i++)
		for(int k = 0; k < size; k++)
			for(int j = 0; j < size; j++)
				C[i*size+j] += A[i*size+k]*B[k*size+j];
	elapsed_time("Ciao");
	#endif
	#ifdef NORMAL
	int **A= new int*[1024]();
	int **B = new int*[1024]();
	int **C = new int*[1024]();

	for(int i = 0; i < 1024; i++) {
		A[i] = new int[1024]();
		B[i] = new int[1024]();
		C[i] = new int[1024]();
		A[i][i] = B[i][i] = 1;
		//A[i*size+i] = B[i*size+i] = 1;
	}
	start_time();
	for(int i = 0; i < size; i++)
		for(int k = 0; k < size; k++)
			for(int j = 0; j < size; j++)
				C[i][j] += A[i][k]*B[k][j];
	elapsed_time("Normal");
	#endif


}
