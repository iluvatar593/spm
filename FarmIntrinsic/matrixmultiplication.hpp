/*
 * matrixmultiplication_float.hpp
 *
 *  Created on: 05/nov/2014
 *      Author: atzoril
 */

#ifndef MATRIXMULTIPLICATION_FLOAT_HPP_
#define MATRIXMULTIPLICATION_FLOAT_HPP_

#include <mmintrin.h>   // MMX?
#include <xmmintrin.h>  // SSE
#include <pmmintrin.h>  // SSE2
#include <emmintrin.h>  // SSE3
#include <immintrin.h>  // AVX

#include "utils.hpp"

#if defined(__MIC__)
	#include "kernel_mic_float.hpp"
	#include "kernel_mic_double.hpp"
#else
	#include "kernel_host_float.hpp"
	#include "kernel_host_double.hpp"
#endif

template<typename NUM>
void Kernel(NUM *__restrict__ a, NUM *__restrict__ b, NUM *__restrict__ c, NUM m, int sz, const int ldc=0) {
	if(sz < 16)
	switch(sz) {
		case 0: printf("This size is unsupported \n"); exit(-1);
		case 1: MMKernel<1>(a, b, c, m, ldc); break;
		case 2: MMKernel<2>(a, b, c, m, ldc); break;
		case 3: MMKernel<3>(a, b, c, m, ldc); break;
		case 4: MMKernel<4>(a, b, c, m, ldc); break;
		case 5: MMKernel<5>(a, b, c, m, ldc); break;
		case 6: MMKernel<6>(a, b, c, m, ldc); break;
		case 7: MMKernel<7>(a, b, c, m, ldc); break;
		case 8: MMKernel<8>(a, b, c, m, ldc); break;
		case 9: MMKernel<9>(a, b, c, m, ldc); break;
		case 10: MMKernel<10>(a, b, c, m, ldc); break;
		case 11: MMKernel<11>(a, b, c, m, ldc); break;
		case 12: MMKernel<12>(a, b, c, m, ldc); break;
		case 13: MMKernel<13>(a, b, c, m, ldc); break;
		case 14: MMKernel<14>(a, b, c, m, ldc); break;
		case 15: MMKernel<15>(a, b, c, m, ldc); break;
		default: break;
	}
	else
	switch(sz) {
		case 16: MMKernel<16>(a, b, c, m, ldc); break;
		case 17: MMKernel<17>(a, b, c, m, ldc); break;
		case 18: MMKernel<18>(a, b, c, m, ldc); break;
		case 19: MMKernel<19>(a, b, c, m, ldc); break;
		case 20: MMKernel<20>(a, b, c, m, ldc); break;
		case 21: MMKernel<21>(a, b, c, m, ldc); break;
		case 22: MMKernel<22>(a, b, c, m, ldc); break;
		case 23: MMKernel<23>(a, b, c, m, ldc); break;
		case 24: MMKernel<24>(a, b, c, m, ldc); break;
		case 25: MMKernel<25>(a, b, c, m, ldc); break;
		case 26: MMKernel<26>(a, b, c, m, ldc); break;
		case 27: MMKernel<27>(a, b, c, m, ldc); break;
		case 28: MMKernel<28>(a, b, c, m, ldc); break;
		case 29: MMKernel<29>(a, b, c, m, ldc); break;
		case 30: MMKernel<30>(a, b, c, m, ldc); break;
		case 31: MMKernel<31>(a, b, c, m, ldc); break;
		default: printf("This size is unsupported \n"); exit(-1);
	}
}

#endif /* MATRIXMULTIPLICATION_FLOAT_HPP_ */
