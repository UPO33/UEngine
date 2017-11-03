#pragma once

#include <immintrin.h>

typedef __m128	XMMF;
typedef __m128i XMMI;
typedef __m128d XMMD;
typedef __m256  YMMF;
typedef __m256d YMMD;
typedef __m256i YMMI;

inline XMMF operator * (XMMF a, XMMF b) { return _mm_mul_ps(a, b); }
inline XMMF operator / (XMMF a, XMMF b) { return _mm_div_ps(a, b); }
inline XMMF operator + (XMMF a, XMMF b) { return _mm_add_ps(a, b); }
inline XMMF operator - (XMMF a, XMMF b) { return _mm_sub_ps(a, b); }

inline XMMF operator * (XMMF a, const float b) { return _mm_mul_ps(a, _mm_set_ps1(b)); }
inline XMMF operator / (XMMF a, const float b) { return _mm_div_ps(a, _mm_set_ps1(b)); }
inline XMMF operator + (XMMF a, const float b) { return _mm_add_ps(a, _mm_set_ps1(b)); }
inline XMMF operator - (XMMF a, const float b) { return _mm_sub_ps(a, _mm_set_ps1(b)); }

inline XMMF operator * (const float a, XMMF b) { return _mm_mul_ps(_mm_set_ps1(a), b); }
inline XMMF operator / (const float a, XMMF b) { return _mm_div_ps(_mm_set_ps1(a), b); }
inline XMMF operator + (const float a, XMMF b) { return _mm_add_ps(_mm_set_ps1(a), b); }
inline XMMF operator - (const float a, XMMF b) { return _mm_sub_ps(_mm_set_ps1(a), b); }

inline XMMF XMMFLoad(const float* inAligned)			{ return _mm_load_ps(inAligned); }
inline XMMF XMMFLoadU(const float* in)					{ return _mm_loadu_ps(in); }
inline void XMMFStore(float* outAligned, XMMF value)	{ _mm_store_ps(outAligned, value); }
inline void XMMFStoreU(float* out, XMMF value)			{ _mm_storeu_ps(out, value); }


//zero the w part
inline __m128 _mm_zerow_ps(__m128 x)
{
	alignas(16) static const unsigned Mask[] = { 0, 0xFFffFFff, 0xFFffFFff, 0xFFffFFff };
	return _mm_and_ps(x, _mm_load_ps((const float*)Mask));
}
//flipping the sign of -
inline __m128 _mm_negate_ps(__m128 x) 
{
	//return _mm_sub_ps(_mm_setzero_ps(), x);
	return _mm_xor_ps(x, _mm_set_ps1(-0.0f)); 
}

inline __m128 _mm_abs_ps(__m128 x) { return _mm_andnot_ps(_mm_set_ps1(-0.f), x); }

inline __m128d _mm_abs_pd(__m128d x) { return _mm_andnot_pd(_mm_set1_pd(-0.0), x); }
inline __m128 _mm_clamp_ps(__m128 value, __m128 min, __m128 max)
{
	return _mm_min_ps(_mm_max_ps(value, min), max);
}

//http://www.tommesani.com/index.php/simd/62-sse-shuffle.html
#define _MM_SHUFFLE_REVERSE		_MM_SHUFFLE(0,1,2,3)
#define _MM_SHUFFLE_SAME		_MM_SHUFFLE(3,2,1,0)
#define _MM_SHUFFLE_ALLA		_MM_SHUFFLE(0,0,0,0)
#define _MM_SHUFFLE_ALLB		_MM_SHUFFLE(1,1,1,1)
#define _MM_SHUFFLE_ALLC		_MM_SHUFFLE(2,2,2,2)
#define _MM_SHUFFLE_ALLD		_MM_SHUFFLE(3,3,3,3)

//dot product of x y z and store to X
#define _DP_XYZ_X 0b01110001
//dot product of x y z and store to all
#define _DP_XYZ_ALL 0b01111111


//#note _mm_move_mask:
//r := sign(a3)<<3 | sign(a2)<<2 | sign(a1)<<1 | sign(a0)
#define _MAKE_MOVEMASK_SIGN(D,C,B,A) ((D << 3) | (C << 2) | (B << 1) | A)

//#note _mm_dp_ps:
//4 high bits : which elements should be summed
//4 low bits: which output slot should contain the result

namespace UCore
{

};