//============================================================================
// Name        : MatrixMultiplication.cpp
// Author      : Alessandro
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#define restrict
#define treshold 64
template <typename NUM>
class MatMul {
public:
	MatMul(unsigned int size):msize(size){
		S1 = new NUM*[size/2]();
		S2 = new NUM*[size/2]();
		P1 = new NUM*[size/2]();
		P2 = new NUM*[size/2]();
		P3 = new NUM*[size/2]();
		P4 = new NUM*[size/2]();
		P5 = new NUM*[size/2]();
		P6 = new NUM*[size/2]();
		P7 = new NUM*[size/2]();
		for(int i = 0; i < size/2; i++) {
			S1[i] = new NUM[size/2]();
			S2[i] = new NUM[size/2]();
			P1[i] = new NUM[size/2]();
			P1[i] = new NUM[size/2]();
			P2[i] = new NUM[size/2]();
			P3[i] = new NUM[size/2]();
			P4[i] = new NUM[size/2]();
			P5[i] = new NUM[size/2]();
			P6[i] = new NUM[size/2]();
			P7[i] = new NUM[size/2]();
		}
	};
	~MatMul(){}
	inline void normalMatrixMultiplication(NUM **restrict A, NUM** restrict B, NUM** restrict C, unsigned int size, unsigned int offsetA=0, unsigned int offsetB=0, unsigned int offsetC=0) {
		NUM *restrict Bvector;
		NUM *restrict Cvector;
		for(unsigned int i = 0; i < size; i++) {
			Cvector = &C[i][offsetC];
			for(unsigned int k = 0; k < size; k++) {
				Bvector = &B[k][offsetB];
				register int aik = A[i][k+offsetA];
				for (unsigned int j = 0; j < size; j++)
						Cvector[j] += aik* Bvector[j];
			}
		}
	}
	inline void matrixSum(NUM **restrict A, unsigned int offsetColA, NUM** restrict B, unsigned int offsetColB, NUM**restrict C, unsigned int size) {
			for(int i = 0; i < size; i++){
				NUM *restrict rowA = &A[i][offsetColA], *restrict rowB = &B[i][offsetColB], *restrict rowC = C[i];
				for(int j =0 ; j < size; j++) {
					rowC[j] = rowA[j] + rowB[j];
				}
			}
		}

	inline void matrixSub(NUM **restrict A, unsigned int offsetColA, NUM** restrict B, unsigned int offsetColB, NUM**restrict C, unsigned int size) {
				for(int i = 0; i < size; i++){
					NUM *restrict rowA = &A[i][offsetColA], *restrict rowB = &B[i][offsetColB], *restrict rowC = C[i];
					for(int j = 0; j < size; j++) {
						//if(i == j) printf("%d, %d -- %d %d\n", i, j, rowA[j+offsetColA], rowB[j+offsetColB]);
						rowC[j] = rowA[j] - rowB[j];
					}
				}
			}

	void strassenMatrixMultiplication(NUM **restrict A, NUM** restrict B, NUM**restrict C, unsigned int size, unsigned int offsetA = 0, unsigned int offsetB = 0) {
			unsigned int newsize = size/2;

			//printf("------ Current size is %d, offsetA is %d, offsetB is %d-----\n", size, offsetA, offsetB);
			matrixSum(A, offsetA, &A[newsize], offsetA+newsize, S1, newsize); //S1 has been calculated
			matrixSum(B, offsetB, &B[newsize], offsetB+newsize, S2, newsize); //S2 has been calculated
			//printf("Until now it seems ok!\n");
			//NUM**P1 = new NUM*[newsize]();
			//for(int i = 0; i < newsize; i++) P1[i] = new NUM[newsize]();

			normalMatrixMultiplication(S1, S2, P1, newsize); //P1 = S1*S2
			matrixSum(&A[newsize], offsetA, &A[newsize], offsetA+newsize, S1, newsize); //S3

			//NUM **P2 = new NUM*[newsize]();
			//for(int i = 0; i < newsize; i++) P2[i] = new NUM[newsize](); //Allocation of new space for s2, C can be reused but check.
			//P2 = S3 * B11
			normalMatrixMultiplication(S1, B, P2, newsize); //P2 calculated
			matrixSub(B, offsetB+newsize, &B[newsize], offsetB+newsize, S2, newsize); //S4



			//NUM**P3 = new NUM*[newsize]();
			//for(int i = 0;i < newsize; i++) P3[i] = new NUM[newsize]();
			//printf("Until now it seems ok!\n");
			normalMatrixMultiplication(A, S2, P3, newsize); //P3 calculated, check fi space can be reused
			matrixSub(&B[newsize], offsetB, B, offsetB, S1, newsize); //S5

			//NUM**P4 = new NUM*[newsize]();
			//for(int i = 0;i < newsize; i++) P4[i] = new NUM[newsize]();
			normalMatrixMultiplication(&A[newsize], S1, P4, newsize, newsize);
			matrixSum(A, offsetA, A, offsetA+newsize, S2, newsize); //S6
//			NUM**P5 = new NUM*[newsize]();
//			for(int i = 0;i < newsize; i++) P5[i] = new NUM[newsize]();
			normalMatrixMultiplication(S2, &B[newsize], P5, newsize, 0, newsize);
			matrixSub(&A[newsize], offsetA, A, offsetA, S1, newsize); //S7
			matrixSum(B, offsetB, B, offsetB+newsize, S2, newsize); //S8
//			NUM**P6 = new NUM*[newsize]();
//			for(int i = 0;i < newsize; i++) P6[i] = new NUM[newsize]();
			normalMatrixMultiplication(S1, S2, P6, newsize);
			matrixSub(A, offsetA+newsize, &A[newsize], offsetA+newsize, S1, newsize); //S9
			matrixSum(&B[newsize], offsetB, &B[newsize], offsetB+newsize, S2, newsize); //S10
//			NUM**P7 = new NUM*[newsize]();
//			for(int i = 0; i < newsize; i++) P7[i] = new NUM[newsize]();
			normalMatrixMultiplication(S1, S2, P7,newsize);
			//C11
			for(int i = 0; i < newsize; i++){
				for(int j = 0; j < newsize; j++) {
					C[i][j] = P1[i][j] + P4[i][j] - P5[i][j] + P7[i][j];
				}
			}
			//C12
			for(int i = 0; i < newsize; i++) {
				for(int j = newsize; j < size; j++) {
					C[i][j] = P3[i][j-newsize] + P5[i][j-newsize];
				}
			}
			//C21 (some error in P2 or P4)
			for(int i = newsize; i < size; i++) {
				for(int j = 0; j < newsize; j++) {
					C[i][j] = P2[i-newsize][j] + P4[i-newsize][j];
				}
			}
			//C22
			//printf("Last block...\n");
			for(int i = newsize; i < size; i++) {
				for(int j = newsize; j < size; j++) {
					C[i][j] = P1[i-newsize][j-newsize] - P2[i-newsize][j-newsize] + P3[i-newsize][j-newsize] + P6[i-newsize][j-newsize];
					//printf("C[%d][%d] = %d\n", i, j, C[i][j]);
				}
			}
			cleanUp();
		}
private:
	NUM **S1, **S2, **P1, **P2, **P3, **P4, **P5, **P6, **P7;
	unsigned int msize;
	void cleanUp(){
		for(int i = 0; i < msize/2; i++){
			for(int j = 0; j < msize/2; j++) {
				P1[i][j] = P2[i][j] = P3[i][j] = P4[i][j] = P5[i][j] = P6[i][j] = P7[i][j] = 0;
			}
		}
	}
};
#define size 2048

int main() {

	int **A = new int*[size]();
	int **B = new int*[size]();
	int **C = new int*[size]();
	int **D = new int*[size]();
	for(int i = 0; i < size; i++) {
		A[i] = new int[size]();
		B[i] = new int[size]();
		C[i] = new int[size]();
		D[i] = new int[size]();
		//A[i][i] = 1; B[i][i] = 1;
	}
	for(int i =0; i < size; i++)
		for(int j = 0; j < size; j++) {
			A[i][j] = rand()%100;
			B[i][j] = rand()%100;
		}

	MatMul<int> *m = new MatMul<int>((unsigned int)size);
	m->strassenMatrixMultiplication(A, B, C, size);
	//MatMul<int>::normalMatrixMultiplication(A, B, C, 1024);
	printf("Ended calculation\n");
	/*for(int i = 0; i < 1024; i++) {
		if(C[i][i] != 1) {
			std::cout<<"Error\n";
			std::cout<< "i is " << i << "value is " << C[i][i];
			return -1;
		}
	}*/
	std::cout<<"Verify\n";
	m->normalMatrixMultiplication(A, B, D, size);
	for(int i = 0; i < size; i++) {
		for(int j = 0; j < size; j++)
			if(D[i][j] != C[i][j]) {
				std::cout<<"Error\n";
				std::cout<< "i is " << i<<","<<j << "value is " << C[i][j] << "it should be" << D[i][j];
				return -1;
			}
		}
	std::cout<<"Everything is alright\n";


}

