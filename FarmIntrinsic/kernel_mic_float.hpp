/*
 * kernel_mic_float.hpp
 *
 *  Created on: 07/nov/2014
 *      Author: alessandro
 */

#ifndef KERNEL_MIC_FLOAT_HPP_
#define KERNEL_MIC_FLOAT_HPP_

#include <zmmintrin.h>  // KNC
#define vZeroPS(c) {(c) = _mm512_setzero_ps();}

template<int sz>
void MMKernel(float *__restrict__ a, float *__restrict__ b, float *__restrict__ c, const int m, const int ldc) {
	__m512 c0, c1, c2, c3, c4, c5, c6, c7, c8, c9;
	__m512 c10, c11, c12, c13, c14, c15, c16, c17, c18, c19;
	__m512 c20, c21, c22, c23, c24, c25, c26, c27, c28, c29;
	const int offC = (ldc > 0) ? ldc : m;
	vZeroPS(c0);
	vZeroPS(c1);
	vZeroPS(c2);
	vZeroPS(c3);
	vZeroPS(c4);
	vZeroPS(c5);
	vZeroPS(c6);
	vZeroPS(c7);
	vZeroPS(c8);
	vZeroPS(c9);
	vZeroPS(c10);
	vZeroPS(c11);
	vZeroPS(c12);
	vZeroPS(c13);
	vZeroPS(c14);
	vZeroPS(c15);
	vZeroPS(c16);
	vZeroPS(c17);
	vZeroPS(c18);
	vZeroPS(c19);
	vZeroPS(c20);
	vZeroPS(c21);
	vZeroPS(c22);
	vZeroPS(c23);
	vZeroPS(c24);
	vZeroPS(c25);
	vZeroPS(c26);
	vZeroPS(c27);
	vZeroPS(c28);
	vZeroPS(c29);

	const int offA=30;
	for(int i = 0; i < sz*TILE; i++) {
		const __m512 bv = _mm512_extload_ps(b+i*m, _MM_UPCONV_PS_NONE, _MM_BROADCAST32_NONE, _MM_HINT_NT);
		/** Calculates 16 elements of 30 rows of c (c0...c29) by broadcasting
		 *  element AKI and multiplying it for B0...B15. Uses FMA; A is temporal.
		 * */
		c0 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+0, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c0);
		c1 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+1, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c1);
		c2 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+2, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c2);
		c3 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+3, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c3);
		c4 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+4, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c4);
		c5 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+5, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c5);
		c6 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+6, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c6);
		c7 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+7, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c7);
		c8 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+8, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c8);
		c9 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+9, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c9);
		c10 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+10, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0),bv, c10);
		c11 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+11, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0),bv, c11);
		c12 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+12, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c12);
		c13 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+13, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c13);
		c14 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+14, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c14);
		c15 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+15, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c15);
		c16 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+16, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c16);
		c17 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+17, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c17);
		c18 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+18, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c18);
		c19 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+19, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c19);
		c20 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+20, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c20);
		c21 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+21, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c21);
		c22 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+22, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c22);
		c23 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+23, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c23);
		c24 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+24, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c24);
		c25 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+25, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c25);
		c26 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+26, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c26);
		c27 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+27, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c27);
		c28 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+28, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c28);
		c29 = _mm512_fmadd_ps(_mm512_extload_ps(a+i*offA+29, _MM_UPCONV_PS_NONE, _MM_BROADCAST_1X16, 0), bv, c29);
	}

	_mm512_storenr_ps(c+0*offC, c0);
	_mm512_storenr_ps(c+1*offC, c1);
	_mm512_storenr_ps(c+2*offC, c2);
	_mm512_storenr_ps(c+3*offC, c3);
	_mm512_storenr_ps(c+4*offC, c4);
	_mm512_storenr_ps(c+5*offC, c5);
	_mm512_storenr_ps(c+6*offC, c6);
	_mm512_storenr_ps(c+7*offC, c7);
	_mm512_storenr_ps(c+8*offC, c8);
	_mm512_storenr_ps(c+9*offC, c9);
	_mm512_storenr_ps(c+10*offC, c10);
	_mm512_storenr_ps(c+11*offC, c11);
	_mm512_storenr_ps(c+12*offC, c12);
	_mm512_storenr_ps(c+13*offC, c13);
	_mm512_storenr_ps(c+14*offC, c14);
	_mm512_storenr_ps(c+15*offC, c15);
	_mm512_storenr_ps(c+16*offC, c16);
	_mm512_storenr_ps(c+17*offC, c17);
	_mm512_storenr_ps(c+18*offC, c18);
	_mm512_storenr_ps(c+19*offC, c19);
	_mm512_storenr_ps(c+20*offC, c20);
	_mm512_storenr_ps(c+21*offC, c21);
	_mm512_storenr_ps(c+22*offC, c22);
	_mm512_storenr_ps(c+23*offC, c23);
	_mm512_storenr_ps(c+24*offC, c24);
	_mm512_storenr_ps(c+25*offC, c25);
	_mm512_storenr_ps(c+26*offC, c26);
	_mm512_storenr_ps(c+27*offC, c27);
	_mm512_storenr_ps(c+28*offC, c28);
	_mm512_storenr_ps(c+29*offC, c29);
}




#endif /* KERNEL_MIC_FLOAT_HPP_ */
