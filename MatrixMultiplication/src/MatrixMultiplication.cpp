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
#define restrict
#define treshold 128
template <typename NUM>
class MatMul {
public:
	MatMul(){};
	~MatMul(){}
	static inline void normalMatrixMultiplication(NUM **restrict A, NUM** restrict B, NUM** restrict C, unsigned int size, unsigned int offsetA=0, unsigned int offsetB=0) {
			NUM *restrict Bvector;
			NUM *restrict Cvector;
			for(unsigned int i = 0; i < size; i++) {
				Cvector = C[i];
				for(unsigned int k = 0; k < size; k++) {
					Bvector = B[k];
					register int aik = A[i][k+offsetA];
					for (unsigned int j = 0; j < size; j++)
						Cvector[j] += aik* Bvector[j+offsetB];
				}
			}
		}
	static inline void matrixSum(NUM **restrict A, unsigned int offsetColA, NUM** restrict B, unsigned int offsetColB, NUM**restrict C, unsigned int size) {
			for(int i = 0; i < size; i++){
				NUM *restrict rowA = A[i], *restrict rowB = B[i], *restrict rowC = C[i];
				for(int j =0 ; j < size; j++) {
					rowC[j] = rowA[j+offsetColA] + rowB[j+offsetColB];
				}
			}
		}

	static inline void matrixSub(NUM **restrict A, unsigned int offsetColA, NUM** restrict B, unsigned int offsetColB, NUM**restrict C, unsigned int size) {
				for(int i = 0; i < size; i++){
					NUM *restrict rowA = A[i], *restrict rowB = B[i], *restrict rowC = C[i];
					for(int j = 0; j < size; j++) {
						//if(i == j) printf("%d, %d -- %d %d\n", i, j, rowA[j+offsetColA], rowB[j+offsetColB]);
						rowC[j] = rowA[j+offsetColA] - rowB[j+offsetColB];
					}
				}
			}

	static void strassenMatrixMultiplication(NUM **restrict A, NUM** restrict B, NUM**restrict C, unsigned int size, unsigned int offsetA = 0, unsigned int offsetB = 0) {
			if(size <= treshold){ //Stop recursion.
				normalMatrixMultiplication(A, B, C, size, offsetA, offsetB);
				return;
			}
			unsigned int newsize = size/2;
			NUM **S1 = new NUM*[newsize]();
			NUM **S2 = new NUM*[newsize]();
			for(int i = 0; i < newsize; i++) {
				S1[i] = new NUM[newsize]();
				S2[i] = new NUM[newsize]();
			}
			//printf("------ Current size is %d, offsetA is %d, offsetB is %d-----\n", size, offsetA, offsetB);
			matrixSum(A, offsetA, &A[newsize], offsetA+newsize, S1, newsize); //S1 has been calculated
			matrixSum(B, offsetB, &B[newsize], offsetB+newsize, S2, newsize); //S2 has been calculated
			//printf("Until now it seems ok!\n");
			NUM**P1 = new NUM*[newsize]();
			for(int i = 0; i < newsize; i++) P1[i] = new NUM[newsize]();

			strassenMatrixMultiplication(S1, S2, P1, newsize); //P1 = S1*S2
			//printf("Until now it seems ok!\n");
			//Fin qui tutto bene
			matrixSum(&A[newsize], offsetA, &A[newsize], offsetA+newsize, S1, newsize); //S3

			NUM **P2 = new NUM*[newsize]();
			for(int i = 0; i < newsize; i++) P2[i] = new NUM[newsize](); //Allocation of new space for s2, C can be reused but check.

			strassenMatrixMultiplication(B, S1, P2, newsize); //P2 calculated
			//printf("Until now it seems ok!\n");
			//printf("P2 calculated \n"); //fin qui tutto bene
			matrixSub(B, offsetB+newsize, &B[newsize], offsetB+newsize, S2, newsize); //S4
			//if(S2[0][0] >= 0) printf("Error in S4\n");
			//printf("S4 calculated \n");
			NUM**P3 = new NUM*[newsize]();
			for(int i = 0;i < newsize; i++) P3[i] = new NUM[newsize]();
			//printf("Until now it seems ok!\n");
			strassenMatrixMultiplication(A, S2, P3, newsize); //P3 calculated, check fi space can be reused
			//printf("Until now it seems ok!\n");
			//	printf("P3 calculated \n");
			matrixSub(&B[newsize], offsetB, B, offsetB, S1, newsize); //S5

			NUM**P4 = new NUM*[newsize]();
			for(int i = 0;i < newsize; i++) P4[i] = new NUM[newsize]();
			normalMatrixMultiplication(&A[newsize], S1, P4, newsize, newsize);
			//printf("Until now it seems ok!\n");
			//if(P4[0][0] >= 0) printf("Error in P4 %d\n", P4[0][0]);
			matrixSum(A, offsetA, A, offsetA+newsize, S2, newsize); //S6
			//printf("Until now it seems ok!\n");
			//if(S2[0][0] != 1) printf("Error in S6 %d\n", S2[0][0]);
			NUM**P5 = new NUM*[newsize]();
			for(int i = 0;i < newsize; i++) P5[i] = new NUM[newsize]();
			normalMatrixMultiplication(S2, &B[newsize], P5, newsize, 0, newsize);
			//printf("Until now it seems ok!\n");
			//if(P5[0][0] != 1) printf("Error in P5 %d\n", P5[0][0]);
			matrixSub(&A[newsize], offsetA, A, offsetA, S1, newsize); //S7
			//if(S1[0][0] >= 0) printf("Error in S7 %d\n", S1[0][0]);
			matrixSum(B, offsetB, B, offsetB+newsize, S2, newsize); //S8
			//if(S2[0][0] != 1) printf("Error in S8 %d\n", S2[0][0]);
			NUM**P6 = new NUM*[newsize]();
			for(int i = 0;i < newsize; i++) P6[i] = new NUM[newsize]();
			strassenMatrixMultiplication(S1, S2, P6, newsize);
			//if(P6[0][0] >= 0) printf("Error in P6 %d\n", P6[0][0]);
			matrixSub(A, offsetA+newsize, &A[newsize], offsetA+newsize, S1, newsize); //S9
			//if(S1[0][0] >= 0) printf("Error in S9%d\n", S1[0][0]);
			matrixSum(&B[newsize], offsetB, &B[newsize], offsetB+newsize, S2, newsize); //S10
			//if(S2[0][0] != 1) printf("Error in s10 %d \n", S2[0][0]);
			NUM**P7 = new NUM*[newsize]();
			for(int i = 0; i < newsize; i++) P7[i] = new NUM[newsize]();
			strassenMatrixMultiplication(S1, S2, P7,newsize);
			//if (P7[0][0] != -1) printf("Error in P7 %d\n", P7[0][0]);
			//printf("Until now it seems ok!\n");
			//Recombine all this stuff
			//C11
			for(int i = 0; i < newsize; i++){
				for(int j = 0; j < newsize; j++) {
					C[i][j] = P1[i][j] + P4[i][j] - P5[i][j] + P7[i][j];
				}
			}
			for(int i = 0; i < newsize; i++) {
				for(int j = newsize; j < size; j++) {
					C[i][j] = P3[i][j-newsize] + P5[i][j-newsize];
				}
			}
			for(int i = newsize; i < size; i++) {
				for(int j = 0; j < newsize; j++) {
					C[i][j] = P2[i-newsize][j] + P4[i-newsize][j];
				}
			}
			//printf("Last block...\n");
			for(int i = newsize; i < size; i++) {
				for(int j = newsize; j < size; j++) {
					C[i][j] = P1[i-newsize][j-newsize] - P2[i-newsize][j-newsize] + P3[i-newsize][j-newsize] + P6[i-newsize][j-newsize];
					//printf("C[%d][%d] = %d\n", i, j, C[i][j]);
				}
			}

			for(int i = 0; i < newsize; i++) {
				delete[] S1[i];
				delete[] S2[i];
				delete[] P1[i];
				delete[] P2[i];
				delete[] P3[i];
				delete[] P4[i];
				delete[] P5[i];
				delete[] P6[i];
				delete[] P7[i];
			}
			delete[] S1;
			delete[] S2;
			delete[] P1;
			delete[] P2;
			delete[] P3;
			delete[] P4;
			delete[] P5;
			delete[] P6;
			delete[] P7;
		}
};

int main() {
	/** Matrix initialization */
	int **A = new int*[1024]();
	int **B = new int*[1024]();
	int **C = new int*[1024]();
	for(int i = 0; i < 1024; i++) {
		A[i] = new int[1024]();
		B[i] = new int[1024]();
		C[i] = new int[1024]();
		A[i][i] = 1; B[i][i] = 1;
	}

	MatMul<int>::strassenMatrixMultiplication(A, B, C, 1024);
	//MatMul<int>::normalMatrixMultiplication(A, B, C, 1024);
	printf("Ended calculation\n");
	for(int i = 0; i < 1024; i++) {
		if(C[i][i] != 1) {
			std::cout<<"Error\n";
			std::cout<< "i is " << i << "value is " << C[i][i];
			return -1;
		}
	}
	std::cout<<"Everything is alright\n";

}
