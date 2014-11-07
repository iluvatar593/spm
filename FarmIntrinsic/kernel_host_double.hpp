/*
 * kernel_host_double.hpp
 *
 *  Created on: 07/nov/2014
 *      Author: alessandro
 */

#ifndef KERNEL_HOST_DOUBLE_HPP_
#define KERNEL_HOST_DOUBLE_HPP_

#define vZero(c) {(c) = _mm256_setzero_pd();}

template<int sz>
void MMKernel(double *__restrict__ a, double *__restrict__ b, double *__restrict__ c, const int m, const int ldc=0) {
	__m256d c0, c1, c2, c3, c4, c5, c6, c7;
	__m256d temp0, temp1, temp2, temp3, temp4, temp5, temp6;

	const int offC = (ldc > 0) ? ldc : m;
	vZero(c0);
	vZero(c1);
	vZero(c2);
	vZero(c3);
	vZero(c4);
	vZero(c5);
	vZero(c6);
	vZero(c7);

	const int offA=8;
	for(int i = 0; i < sz*TILE; i++) {
		const __m256d bv = _mm256_load_pd(b+i*m);
		temp0 = _mm256_set1_pd(a[i*offA+0]);
		temp1 = _mm256_set1_pd(a[i*offA+1]);
		temp2 = _mm256_set1_pd(a[i*offA+2]);
		temp3 = _mm256_set1_pd(a[i*offA+3]);
		temp4 = _mm256_set1_pd(a[i*offA+4]);
		temp5 = _mm256_set1_pd(a[i*offA+5]);
		temp6 = _mm256_set1_pd(a[i*offA+6]);
		temp0 = _mm256_mul_pd(temp0, bv);
		temp1 = _mm256_mul_pd(temp1, bv);
		temp2 = _mm256_mul_pd(temp2, bv);
		temp3 = _mm256_mul_pd(temp3, bv);
		temp4 = _mm256_mul_pd(temp4, bv);
		temp5 = _mm256_mul_pd(temp5, bv);
		temp6 = _mm256_mul_pd(temp6, bv);
		c0 = _mm256_add_pd(temp0, c0);
		temp0 = _mm256_set1_pd(a[i*offA+7]);
		c1 = _mm256_add_pd(temp1, c1);
		c2 = _mm256_add_pd(temp2, c2);
		c3 = _mm256_add_pd(temp3, c3);
		temp0 = _mm256_mul_pd(temp0, bv);
		c4 = _mm256_add_pd(temp4, c4);
		c5 = _mm256_add_pd(temp5, c5);
		c6 = _mm256_add_pd(temp6, c6);
		c7 = _mm256_add_pd(temp0, c7);
	}
	_mm256_stream_pd(c+0*offC, c0);
	_mm256_stream_pd(c+1*offC, c1);
	_mm256_stream_pd(c+2*offC, c2);
	_mm256_stream_pd(c+3*offC, c3);
	_mm256_stream_pd(c+4*offC, c4);
	_mm256_stream_pd(c+5*offC, c5);
	_mm256_stream_pd(c+6*offC, c6);
	_mm256_stream_pd(c+7*offC, c7);
}




#endif /* KERNEL_HOST_DOUBLE_HPP_ */
