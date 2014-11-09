/*
 * kernel_host_float.hpp
 *
 *  Created on: 07/nov/2014
 *      Author: alessandro
 */

#ifndef KERNEL_HOST_FLOAT_HPP_
#define KERNEL_HOST_FLOAT_HPP_

#include <cstdio>

#define vZeroPS(c) {(c) = _mm256_setzero_ps();}

template<int sz>
void MMKernel(float *__restrict__ a, float *__restrict__ b, float *__restrict__ c, const int m, const int ldc=0) {
	__m256 c0, c1, c2, c3, c4, c5, c6, c7;
	__m256 temp0, temp1, temp2, temp3, temp4, temp5, temp6;

	const int offC = (ldc > 0) ? ldc : m;
	vZeroPS(c0);
	vZeroPS(c1);
	vZeroPS(c2);
	vZeroPS(c3);
	vZeroPS(c4);
	vZeroPS(c5);
	vZeroPS(c6);
	vZeroPS(c7);

	const int offA=8;
	for(int i = 0; i < sz*TILE; i++) {
		const __m256 bv = _mm256_load_ps(b+i*m);
		temp0 = _mm256_set1_ps(a[i*offA+0]);
		temp1 = _mm256_set1_ps(a[i*offA+1]);
		temp2 = _mm256_set1_ps(a[i*offA+2]);
		temp3 = _mm256_set1_ps(a[i*offA+3]);
		temp4 = _mm256_set1_ps(a[i*offA+4]);
		temp5 = _mm256_set1_ps(a[i*offA+5]);
		temp6 = _mm256_set1_ps(a[i*offA+6]);

		temp0 = _mm256_mul_ps(temp0, bv);
		temp1 = _mm256_mul_ps(temp1, bv);
		temp2 = _mm256_mul_ps(temp2, bv);
		temp3 = _mm256_mul_ps(temp3, bv);
		temp4 = _mm256_mul_ps(temp4, bv);
		temp5 = _mm256_mul_ps(temp5, bv);
		temp6 = _mm256_mul_ps(temp6, bv);
		c0 = _mm256_add_ps(temp0, c0);
		temp0 = _mm256_set1_ps(a[i*offA+7]);
		c1 = _mm256_add_ps(temp1, c1);
		c2 = _mm256_add_ps(temp2, c2);
		c3 = _mm256_add_ps(temp3, c3);
		temp0 = _mm256_mul_ps(temp0, bv);
		c4 = _mm256_add_ps(temp4, c4);
		c5 = _mm256_add_ps(temp5, c5);
		c6 = _mm256_add_ps(temp6, c6);
		c7 = _mm256_add_ps(temp0, c7);

	}

	_mm256_stream_ps(c+0*offC, c0);
	_mm256_stream_ps(c+1*offC, c1);
	_mm256_stream_ps(c+2*offC, c2);
	_mm256_stream_ps(c+3*offC, c3);
	_mm256_stream_ps(c+4*offC, c4);
	_mm256_stream_ps(c+5*offC, c5);
	_mm256_stream_ps(c+6*offC, c6);
	_mm256_stream_ps(c+7*offC, c7);

}


#endif /* KERNEL_HOST_FLOAT_HPP_ */
