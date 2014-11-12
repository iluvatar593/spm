/*
 * kernel_mic_double.hpp
 *
 *  Created on: 07/nov/2014
 *      Author: alessandro
 *
 *  Description: very core of the matrix multiplication.
 *  Registers can contain 8 double precision values.
 */

#ifndef KERNEL_MIC_DOUBLE_HPP_
#define KERNEL_MIC_DOUBLE_HPP_

#include <zmmintrin.h>// KNC

#define vZero(c) {(c) = _mm512_setzero_pd();}

template<int sz>
void MMKernel(double *__restrict__ a, double *__restrict__ b, double *__restrict__ c, const int m, const int ldc=0) {
	__m512d c0, c1, c2, c3, c4, c5, c6, c7, c8, c9;
	__m512d c10, c11, c12, c13, c14, c15, c16, c17, c18, c19;
	__m512d c20, c21, c22, c23, c24, c25, c26, c27, c28, c29;
	const int offC = (ldc > 0) ? ldc : m;
	/** Zero all temporary registers of C */
	vZero(c0);	vZero(c1);	vZero(c2);	vZero(c3);
	vZero(c4);	vZero(c5);	vZero(c6);	vZero(c7);
	vZero(c8);	vZero(c9);	vZero(c10);	vZero(c11);
	vZero(c12);	vZero(c13);	vZero(c14);	vZero(c15);
	vZero(c16);	vZero(c17);	vZero(c18);	vZero(c19);
	vZero(c20);	vZero(c21);	vZero(c22);	vZero(c23);
	vZero(c24);	vZero(c25);	vZero(c26);	vZero(c27);
	vZero(c28);				vZero(c29);
	const int offA=30;
	for(int i = 0; i < sz*TILE; i++) {
		const __m512d bv = _mm512_extload_pd(b+i*m, _MM_UPCONV_PD_NONE, _MM_BROADCAST64_NONE, _MM_HINT_NT);

		/** Calculates 8 elements of 30 rows of c (c0...c29) by broadcasting
		 *  element AKI and multiplying it for B0...B8. Uses FMA; A is temporal.
		 * */
		c0 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+0, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c0);
		c1 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+1, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c1);
		c2 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+2, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c2);
		c3 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+3, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c3);
		c4 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+4, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c4);

		c5 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+5, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c5);
		c6 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+6, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c6);
		c7 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+7, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c7);
		c8 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+8, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c8);

		c9 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+9, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c9);
		c10 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+10, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0),bv, c10);
		c11 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+11, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0),bv, c11);
		c12 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+12, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c12);

		c13 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+13, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c13);
		c14 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+14, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c14);
		c15 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+15, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c15);
		c16 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+16, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c16);

		c17 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+17, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c17);
		c18 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+18, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c18);
		c19 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+19, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c19);
		c20 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+20, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c20);

		c21 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+21, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c21);
		c22 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+22, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c22);
		c23 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+23, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c23);
		c24 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+24, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c24);

		c25 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+25, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c25);
		c26 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+26, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c26);
		c27 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+27, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c27);
		c28 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+28, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c28);

		c29 = _mm512_fmadd_pd(_mm512_extload_pd(a+i*offA+29, _MM_UPCONV_PD_NONE, _MM_BROADCAST_1X8, 0), bv, c29);
	}

	_mm512_storenr_pd(c+0*offC, c0);
	_mm512_storenr_pd(c+1*offC, c1);
	_mm512_storenr_pd(c+2*offC, c2);
	_mm512_storenr_pd(c+3*offC, c3);
	_mm512_storenr_pd(c+4*offC, c4);
	_mm512_storenr_pd(c+5*offC, c5);
	_mm512_storenr_pd(c+6*offC, c6);
	_mm512_storenr_pd(c+7*offC, c7);
	_mm512_storenr_pd(c+8*offC, c8);
	_mm512_storenr_pd(c+9*offC, c9);

	_mm512_storenr_pd(c+10*offC, c10);
	_mm512_storenr_pd(c+11*offC, c11);
	_mm512_storenr_pd(c+12*offC, c12);
	_mm512_storenr_pd(c+13*offC, c13);
	_mm512_storenr_pd(c+14*offC, c14);
	_mm512_storenr_pd(c+15*offC, c15);

	_mm512_storenr_pd(c+16*offC, c16);
	_mm512_storenr_pd(c+17*offC, c17);
	_mm512_storenr_pd(c+18*offC, c18);
	_mm512_storenr_pd(c+19*offC, c19);
	_mm512_storenr_pd(c+20*offC, c20);
	_mm512_storenr_pd(c+21*offC, c21);
	_mm512_storenr_pd(c+22*offC, c22);
	_mm512_storenr_pd(c+23*offC, c23);

	_mm512_storenr_pd(c+24*offC, c24);
	_mm512_storenr_pd(c+25*offC, c25);
	_mm512_storenr_pd(c+26*offC, c26);
	_mm512_storenr_pd(c+27*offC, c27);
	_mm512_storenr_pd(c+28*offC, c28);
	_mm512_storenr_pd(c+29*offC, c29);
}



#endif /* KERNEL_MIC_DOUBLE_HPP_ */
