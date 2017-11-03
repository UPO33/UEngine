
//////////////////////////////////////////////////////////////////////////constructors
inline Vec3::Vec3(float xyzw)
{
	_mm_store_ps((float*)this, _mm_set1_ps(xyzw));
}
inline Vec3::Vec3(float _x, float _y, float _z)
{
	_mm_store_ps((float*)this, _mm_setr_ps(_x, _y, _z, 0));
}
inline Vec3::Vec3(const Vec3& copy)
{
	_mm_store_ps((float*)this, _mm_load_ps(copy));
}
inline Vec3::Vec3(const float xyz[3])
{
	_mm_store_ps((float*)this, _mm_loadu_ps(xyz));
}
inline Vec3::Vec3(const Vec2& _xy, float _z)
{
	_mm_store_ps((float*)this, _mm_setr_ps(_xy.x, _xy.y, _z, 0));
}
//////////////////////////////////////////////////////////////////////////assignments
inline Vec3& Vec3::operator =(const Vec3& copy)
{
	_mm_store_ps((float*)this, _mm_load_ps(copy));
	return *this;
}
inline Vec3& Vec3::operator =(float xyzw)
{
	_mm_store_ps((float*)this, _mm_set1_ps(xyzw));
	return *this;
}
inline Vec3& Vec3::operator =(const Vec4& copy)
{
	_mm_store_ps((float*)this, _mm_load_ps(copy));
	return *this;
}
//////////////////////////////////////////////////////////////////////////
inline bool Vec3::IsNearlyZero(float epsilon) const
{
	return (_mm_movemask_ps(_mm_cmple_ps(_mm_abs_ps(_mm_load_ps((float*)this)), _mm_set1_ps(epsilon))) & 0b0111) == 0b0111;
}
inline bool Vec3::IsNearlyEqual(const Vec3& v, float epsilon) const
{
	__m128 xmm = _mm_abs_ps(_mm_sub_ps(_mm_load_ps(*this), _mm_load_ps(v)));
	return (_mm_movemask_ps(_mm_cmple_ps(xmm, _mm_set1_ps(epsilon))) & 0b0111) == 0b0111;
}
float Vec3::Length() const
{
	return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(_mm_load_ps((float*)this), _mm_load_ps((float*)this), _DP_XYZ_X)));
}
inline float Vec3::RcpLength() const
{
	return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_dp_ps(_mm_load_ps((float*)this), _mm_load_ps((float*)this), _DP_XYZ_X)));
}
inline float Vec3::LengthSquare() const
{
	return _mm_cvtss_f32(_mm_dp_ps(_mm_load_ps((float*)this), _mm_load_ps((float*)this), _DP_XYZ_X));
}
inline float Vec3::LengthV() const
{
	return Vec3(_mm_sqrt_ps(_mm_dp_ps(_mm_load_ps((float*)this), _mm_load_ps((float*)this), _DP_XYZ_ALL)));
}
inline float Vec3::LengthSquareV() const
{
	return Vec3(_mm_dp_ps(_mm_load_ps((float*)this), _mm_load_ps((float*)this), _DP_XYZ_ALL));
}
inline Vec3 Vec3::GetNormalized() const
{
	__m128 xmmThis = _mm_load_ps((float*)this);
	__m128 xmmRLen = _mm_rsqrt_ss(_mm_dp_ps(xmmThis, xmmThis, _DP_XYZ_X));
	return Vec3(_mm_mul_ps(xmmThis, _mm_shuffle_ps(xmmRLen, xmmRLen, _MM_SHUFFLE(0, 0, 0, 0))));
}

inline void Vec3::Normalize()
{
	__m128 xmmThis = _mm_load_ps((float*)this);
	__m128 xmmRLen = _mm_rsqrt_ss(_mm_dp_ps(xmmThis, xmmThis, _DP_XYZ_X));
	_mm_store_ps((float*)this, _mm_mul_ps(xmmThis, _mm_shuffle_ps(xmmRLen, xmmRLen, _MM_SHUFFLE(0, 0, 0, 0))));
}
inline void Vec3::NormalizeSafe()
{
	__m128 xmmThis = _mm_load_ps((float*)this);
	__m128 xmmLenSq = _mm_dp_ps(xmmThis, xmmThis, _DP_XYZ_ALL);
	//if the length of vector is greater that FLOAT_EPSILON the compare result will be 0xFFffFFff and we finally and it with the result
	__m128 xmmCmpRes = _mm_cmpge_ps(xmmLenSq, _mm_set1_ps(FLOAT_EPSILON));
	__m128 xmmRLen = _mm_rsqrt_ss(xmmLenSq);
	__m128 xmmNorm = _mm_mul_ps(xmmThis, _mm_shuffle_ps(xmmRLen, xmmRLen, _MM_SHUFFLE(0, 0, 0, 0)));
	_mm_store_ps((float*)this, _mm_and_ps(xmmNorm, xmmCmpRes));
}
inline Vec3 Vec3::GetNormalizedSafe() const
{
	__m128 xmmThis = _mm_load_ps((float*)this);
	__m128 xmmLenSq = _mm_dp_ps(xmmThis, xmmThis, _DP_XYZ_ALL);
	//if the length of vector is greater that FLOAT_EPSILON the compare result will be 0xFFffFFff and we finally and it with the result
	__m128 xmmCmpRes = _mm_cmpge_ps(xmmLenSq, _mm_set1_ps(FLOAT_EPSILON));
	__m128 xmmRLen = _mm_rsqrt_ss(xmmLenSq);
	__m128 xmmNorm = _mm_mul_ps(xmmThis, _mm_shuffle_ps(xmmRLen, xmmRLen, _MM_SHUFFLE(0, 0, 0, 0)));
	return Vec3(_mm_and_ps(xmmNorm, xmmCmpRes));
}
inline Vec3 Vec3::GetXXX() const
{
	auto xmmThis = _mm_load_ps(*this);
	return Vec3(_mm_shuffle_ps(xmmThis, xmmThis, _MM_SHUFFLE(0, 0, 0, 0)));
}
inline Vec3 Vec3::GetYYY() const
{
	auto xmmThis = _mm_load_ps(*this);
	return Vec3(_mm_shuffle_ps(xmmThis, xmmThis, _MM_SHUFFLE(1, 1, 1, 1)));
}
inline Vec3 Vec3::GetZZZ() const
{
	auto xmmThis = _mm_load_ps(*this);
	return Vec3(_mm_shuffle_ps(xmmThis, xmmThis, _MM_SHUFFLE(2, 2, 2, 2)));
}

//////////////////////////////////////////////////////////////////////////Vec3 x Vec3
inline Vec3 operator + (const Vec3& a, const Vec3& b)
{
	return Vec3(_mm_add_ps(_mm_load_ps(a), _mm_load_ps(b)));
}
inline Vec3 operator - (const Vec3& a, const Vec3& b)
{
	return Vec3(_mm_sub_ps(_mm_load_ps(a), _mm_load_ps(b)));
}
inline Vec3 operator * (const Vec3& a, const Vec3& b)
{
	return Vec3(_mm_mul_ps(_mm_load_ps(a), _mm_load_ps(b)));
}
inline Vec3 operator / (const Vec3& a, const Vec3& b)
{
	return Vec3(_mm_div_ps(_mm_load_ps(a), _mm_load_ps(b)));
}

//////////////////////////////////////////////////////////////////////////Vec3 x float
inline Vec3 operator + (const Vec3& a, float f)
{
	return Vec3(_mm_add_ps(_mm_load_ps(a), _mm_set1_ps(f)));
}
inline Vec3 operator - (const Vec3& a, float f)
{
	return Vec3(_mm_sub_ps(_mm_load_ps(a), _mm_set1_ps(f)));
}
inline Vec3 operator * (const Vec3& a, float f)
{
	return Vec3(_mm_mul_ps(_mm_load_ps(a), _mm_set1_ps(f)));
}
inline Vec3 operator / (const Vec3& a, float f)
{
	return Vec3(_mm_div_ps(_mm_load_ps(a), _mm_set1_ps(f)));
}
//////////////////////////////////////////////////////////////////////////float x Vec3
inline Vec3 operator + (float f, const Vec3& v)
{
	return Vec3(_mm_add_ps(_mm_set1_ps(f), _mm_load_ps(v)));
}
inline Vec3 operator - (float f, const Vec3& v)
{
	return Vec3(_mm_sub_ps(_mm_set1_ps(f), _mm_load_ps(v)));
}
inline Vec3 operator * (float f, const Vec3& v)
{
	return Vec3(_mm_mul_ps(_mm_set1_ps(f), _mm_load_ps(v)));
}
inline Vec3 operator / (float f, const Vec3& v)
{
	return Vec3(_mm_div_ps(_mm_set1_ps(f), _mm_load_ps(v)));
}

//////////////////////////////////////////////////////////////////////////Vec3 x= Vec3
inline Vec3& operator += (Vec3& a, const Vec3& b)
{
	_mm_store_ps(a, _mm_add_ps(_mm_load_ps(a), _mm_load_ps(b)));
	return a;
}
inline  Vec3& operator -= (Vec3& a, const Vec3& b)
{
	_mm_store_ps(a, _mm_sub_ps(_mm_load_ps(a), _mm_load_ps(b)));
	return a;
}
inline Vec3& operator *= (Vec3& a, const Vec3& b)
{
	_mm_store_ps(a, _mm_mul_ps(_mm_load_ps(a), _mm_load_ps(b)));
	return a;
}
inline Vec3& operator /= (Vec3& a, const Vec3& b)
{
	_mm_store_ps(a, _mm_div_ps(_mm_load_ps(a), _mm_load_ps(b)));
	return a;
}
//////////////////////////////////////////////////////////////////////////Vec3 x= float
inline Vec3& operator += (Vec3& a, float f)
{
	_mm_store_ps(a, _mm_add_ps(_mm_load_ps(a), _mm_set1_ps(f)));
	return a;
}
inline  Vec3& operator -= (Vec3& a, float f)
{
	_mm_store_ps(a, _mm_sub_ps(_mm_load_ps(a), _mm_set1_ps(f)));
	return a;
}
inline Vec3& operator *= (Vec3& a, float f)
{
	_mm_store_ps(a, _mm_mul_ps(_mm_load_ps(a), _mm_set1_ps(f)));
	return a;
}
inline Vec3& operator /= (Vec3& a, float f)
{
	_mm_store_ps(a, _mm_div_ps(_mm_load_ps(a), _mm_set1_ps(f)));
	return a;
}
//////////////////////////////////////////////////////////////////////////
inline Vec3 operator - (const Vec3& v)
{
	return Vec3(_mm_negate_ps(_mm_load_ps(v)));
}
inline float Dot(const Vec3& a, const Vec3& b)
{
	return _mm_cvtss_f32(_mm_dp_ps(_mm_load_ps(a), _mm_load_ps(b), _DP_XYZ_X));
}
//dot product
inline float operator | (const Vec3& a, const Vec3& b)
{
	return Dot(a, b);
}
//get normalized
inline Vec3 operator ~ (const Vec3& v)
{
	return v.GetNormalized();
}
//cross product
//(swapping the position of any two arguments negates the result): If AxB == C then BxA == -C. 
inline Vec3 Cross(const Vec3& a, const Vec3& b)
{
	//thats the algorithm:
	//V1.y*V2.z - V1.z*V2.y
	//V1.z*V2.x - V1.x*V2.z
	//V1.x*V2.y - V1.y*V2.x

	//ب ترتیت این مدلی می زارم تو ثبات ی شافل کمتر استفاده میشه 
	//z = V1.x*V2.y - V1.y*V2.x
	//x = V1.y*V2.z - V1.z*V2.y
	//y = V1.z*V2.x - V1.x*V2.z

	__m128 xmmA = _mm_load_ps(a);
	__m128 xmmB = _mm_load_ps(b);
	__m128 xmmB2 = _mm_shuffle_ps(xmmB, xmmB, _MM_SHUFFLE(3, 0, 2, 1));	//y z x
	__m128 xmmA2 = _mm_shuffle_ps(xmmA, xmmA, _MM_SHUFFLE(3, 0, 2, 1)); //y z x
	__m128 xmmMul = _mm_sub_ps(_mm_mul_ps(xmmA, xmmB2), _mm_mul_ps(xmmA2, xmmB));
	return Vec3(_mm_shuffle_ps(xmmMul, xmmMul, _MM_SHUFFLE(0, 0, 2, 1)));
}
//cross product
inline Vec3 operator ^ (const Vec3& a, const Vec3& b)
{
	return Cross(a, b);
}

//////////////////////////////////////////////////////////////////////////
inline bool operator > (const Vec3& a, const Vec3& b)
{
	return (_mm_movemask_ps(_mm_cmpgt_ps(_mm_load_ps(a), _mm_load_ps(b))) & 0b0111) == 0b0111;
}
inline bool operator >= (const Vec3& a, const Vec3& b)
{
	return (_mm_movemask_ps(_mm_cmpge_ps(_mm_load_ps(a), _mm_load_ps(b))) & 0b0111) == 0b0111;
}
inline bool operator < (const Vec3& a, const Vec3& b)
{
	return (_mm_movemask_ps(_mm_cmplt_ps(_mm_load_ps(a), _mm_load_ps(b))) & 0b0111) == 0b0111;
}
inline bool operator <= (const Vec3& a, const Vec3& b)
{
	return (_mm_movemask_ps(_mm_cmple_ps(_mm_load_ps(a), _mm_load_ps(b))) & 0b0111) == 0b0111;
}
inline bool operator == (const Vec3& a, const Vec3& b)
{
	return (_mm_movemask_ps(_mm_cmpeq_ps(_mm_load_ps(a), _mm_load_ps(b))) & 0b0111) == 0b0111;
}
inline bool operator != (const Vec3& a, const Vec3& b)
{
	return (_mm_movemask_ps(_mm_cmpneq_ps(_mm_load_ps(a), _mm_load_ps(b))) & 0b0111) == 0b0111;
}
//////////////////////////////////////////////////////////////////////////
inline Vec3 Abs(const Vec3& v)
{
	return Vec3(_mm_abs_ps(_mm_load_ps(v)));
}
inline Vec3 Min(const Vec3& v0, const Vec3& v1)
{
	return Vec3(_mm_min_ps(_mm_load_ps(v0), _mm_load_ps(v1)));
}
inline Vec3 Max(const Vec3& v0, const Vec3& v1)
{
	return Vec3(_mm_max_ps(_mm_load_ps(v0), _mm_load_ps(v1)));
}
inline Vec3 Lerp(const Vec3& v0, const Vec3& v1, float t)
{
	return v0 * (1 - t) + v1 * t;
}
inline Vec3 Clamp(const Vec3& value, float min, float max)
{
	return Vec3(_mm_clamp_ps(_mm_load_ps(value), _mm_set1_ps(min), _mm_set1_ps(max)));
}
inline Vec3 Clamp(const Vec3& value, const Vec3& min, const Vec3& max)
{
	return Vec3(_mm_clamp_ps(_mm_load_ps(value), _mm_load_ps(min), _mm_load_ps(max)));
}
inline Vec3 Trunc(const Vec3& v)
{
	//return Vec3(_mm_cvtepi32_ps(_mm_cvttps_epi32(_mm_load_ps(v))));
	return Vec3(_mm_round_ps(_mm_load_ps(v), _MM_FROUND_TRUNC));
}
inline Vec3 Floor(const Vec3& v)
{
	return Vec3(_mm_floor_ps(_mm_load_ps(v)));
}
inline Vec3 Ceil(const Vec3& v)
{
	return Vec3(_mm_ceil_ps(_mm_load_ps(v)));
}
inline Vec3 Round(const Vec3& v)
{
	return Vec3(_mm_round_ps(_mm_load_ps(v), _MM_FROUND_NINT));
}
