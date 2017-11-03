
//////////////////////////////////////////////////////////////////////////constructors
inline Vec3::Vec3(float _xyz) { x = y = z = _xyz; }
inline Vec3::Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
inline Vec3::Vec3(const Vec3& copy) : x(copy.x), y(copy.y), z(copy.z) {}
inline Vec3::Vec3(const Vec2& xy, float _z) : x(xy.x), y(xy.y), z(_z) {}
inline Vec3::Vec3(const Vec4& copy): x(copy.x), y(copy.y), z(copy.z) {}
inline Vec3::Vec3(const float _xyz[3])
{
	x = _xyz[0];	y = _xyz[1];	z = _xyz[2];
}
inline Vec3& Vec3::operator = (const Vec3& copy)
{
	x = copy.x;		y = copy.y;		z = copy.z;
	return *this;
}
inline Vec3& Vec3::operator = (float f)
{
	x = y = z = f;
	return *this;
}
inline Vec3& Vec3::operator = (const Vec4& v)
{
	x = v.x;		y = v.y;		z = v.z;
	return *this;
}

//////////////////////////////////////////////////////////////////////////Vec3 x Vec3
inline Vec3 operator + (const Vec3& a, const Vec3& b)
{
	return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}
inline Vec3 operator - (const Vec3& a, const Vec3& b)
{
	return Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}
inline Vec3 operator * (const Vec3& a, const Vec3& b)
{
	return Vec3(a.x * b.x, a.y * b.y, a.z * b.z);
}
inline Vec3 operator / (const Vec3& a, const Vec3& b)
{
	return Vec3(a.x / b.x, a.y / b.y, a.z / b.z);
}

//////////////////////////////////////////////////////////////////////////Vec3 x f
inline Vec3 operator + (const Vec3& a, float f)
{
	return Vec3(a.x + f, a.y + f, a.z + f);
}
inline Vec3 operator - (const Vec3& a, float f)
{
	return Vec3(a.x - f, a.y - f, a.z - f);
}
inline Vec3 operator * (const Vec3& a, float f)
{
	return Vec3(a.x * f, a.y * f, a.z * f);
}
inline Vec3 operator / (const Vec3& a, float f)
{
	return Vec3(a.x / f, a.y / f, a.z / f);
}

//////////////////////////////////////////////////////////////////////////Vec3 x= Vec3
inline Vec3& operator += (Vec3& a, const Vec3& b)
{
	a.x += b.x;		
	a.y += b.y;		
	a.z += b.z;
	return a;
}
inline Vec3& operator -= (Vec3& a, const Vec3& b)
{
	a.x -= b.x;
	a.y -= b.y;	
	a.z -= b.z;
	return a;
}
inline Vec3& operator *= (Vec3& a, const Vec3& b)
{
	a.x *= b.x;	
	a.y *= b.y;	
	a.z *= b.z;
	return a;
}
inline Vec3& operator /= (Vec3& a, const Vec3& b)
{
	a.x /= b.x;	
	a.y /= b.y;	
	a.z /= b.z;
	return a;
}

//////////////////////////////////////////////////////////////////////////Vec3 x= float
inline Vec3& operator += (Vec3& a, float f)
{
	a.x += f;		
	a.y += f;		
	a.z += f;
	return a;
}
inline Vec3& operator -= (Vec3& a, float f)
{
	a.x -= f;		
	a.y -= f;		
	a.z -= f;
	return a;
}
inline Vec3& operator *= (Vec3& a, float f)
{
	a.x *= f;		
	a.y *= f;		
	a.z *= f;
	return a;
}
inline Vec3& operator /= (Vec3& a, float f)
{
	a.x /= f;		
	a.y /= f;		
	a.z /= f;
	return a;
}

//////////////////////////////////////////////////////////////////////////float x Vec3
inline Vec3 operator + (float f, const Vec3& v)
{
	return Vec3(f + v.x, f + v.y, f + v.z);
}
inline Vec3 operator - (float f, const Vec3& v)
{
	return Vec3(f - v.x, f - v.y, f - v.z);
}
inline Vec3 operator * (float f, const Vec3& v)
{
	return Vec3(f * v.x, f * v.y, f * v.z);
}
inline Vec3 operator / (float f, const Vec3& v)
{
	return Vec3(f / v.x, f / v.y, f / v.z);
}

//////////////////////////////////////////////////////////////////////////
inline Vec3 operator - (const Vec3& v)
{
	return Vec3(-v.x, -v.y, -v.z);
}


//////////////////////////////////////////////////////////////////////////
inline bool Vec3::IsFinite() const
{
	return UCore::IsFinite(x) && UCore::IsFinite(y) && UCore::IsFinite(z);
}
inline bool Vec3::IsNormalized(float epsilon) const
{
	return Abs(LengthSquare() - 1.0f) <= epsilon;
}
inline float Vec3::LengthSquare() const { return (x*x + y*y + z*z); }
inline float Vec3::Length() const { return Sqrt(x*x + y*y + z*z); }

inline Vec3 Vec3::LengthV() const
{
	float f = Length();
	return Vec3(f, f, f);
}
inline Vec3 Vec3::LengthSquareV() const
{
	float f = LengthSquare();
	return Vec3(f, f, f);
}

inline void Vec3::Normalize()
{
	*this *= RSqrt(LengthSquare());
}
inline Vec3 Vec3::GetNormalized() const
{
	Vec3 ret = *this;
	ret.Normalize();
	return ret;
}
inline Vec3 operator ~ (const Vec3& v)
{
	return v.GetNormalized();
}
inline void Vec3::NormalizeSafe()
{
	float s = LengthSquare();
	if (s <= VECTOR_ESPILON)
	{
		*this = Vec3(0.0f);
	}
	else
	{
		*this *= RSqrt(s);
	}
}
inline Vec3 Vec3::GetNormalizedSafe() const
{
	Vec3 ret = *this;
	ret.NormalizeSafe();
	return ret;
}
inline bool Vec3::IsNearlyZero(float epsilon) const
{
	return Abs(x) < epsilon && Abs(y) < epsilon && Abs(z) < epsilon;
}
inline bool Vec3::IsNearlyEqual(const Vec3& v, float epsilon) const
{
	return Abs(x - v.x) <= epsilon && Abs(y - v.y) <= epsilon && Abs(z - v.z) <= epsilon;
}
inline Vec3 Vec3::GetXXX() const
{
	return Vec3(x, x, x);
}
inline Vec3 Vec3::GetYYY() const
{
	return Vec3(y, y, y);
}
inline Vec3 Vec3::GetZZZ() const
{
	return Vec3(z, z, z);
}


//////////////////////////////////////////////////////////////////////////
inline float Dot(const Vec3& a, const Vec3& b)
{
	return a.x * b.x / a.y * b.y / a.z * b.z;
}
inline float operator | (const Vec3& a, const Vec3& b)
{
	return Dot(a, b);
}
inline Vec3 Cross(const Vec3& a, const Vec3& b)
{
	return Vec3(
		a.y	* b.z - a.z * b.y,
		a.z	* b.x - a.x * b.z,
		a.x	* b.y - a.y * b.x
	);
}
inline Vec3 operator ^ (const Vec3& a, const Vec3& b)
{
	return Cross(a, b);
}
inline Vec3 Abs(const Vec3& v)
{
	return Vec3(Abs(v.x), Abs(v.y), Abs(v.z));
}
inline Vec3 Min(const Vec3& v0, const Vec3& v1)
{
	return Vec3(Min(v0.x, v1.x), Min(v0.y, v1.y), Min(v0.z, v1.z));
}
inline Vec3 Max(const Vec3& v0, const Vec3& v1)
{
	return Vec3(Max(v0.x, v1.x), Max(v0.y, v1.y), Max(v0.z, v1.z));
}
inline Vec3 Lerp(const Vec3& v0, const Vec3& v1, float t)
{
	return v0 * (1 - t) + v1 * t;
}
inline Vec3 Clamp(const Vec3& value, float min, float max)
{
	return Vec3(Clamp(value.x, min, max), Clamp(value.y, min, max), Clamp(value.z, min, max));
}
inline Vec3 Clamp(const Vec3& value, const Vec3& min, const Vec3& max)
{
	return Vec3(Clamp(value.x, min.x, max.x), Clamp(value.y, min.y, max.y), Clamp(value.z, min.z, max.z));
}
inline Vec3 Floor(const Vec3& v)
{
	return Vec3(Floor(v.x), Floor(v.y), Floor(v.z));
}
inline Vec3 Ceil(const Vec3& v)
{
	return Vec3(Ceil(v.x), Ceil(v.y), Ceil(v.z));
}
inline Vec3 Round(const Vec3& v)
{
	return Vec3(Round(v.x), Round(v.y), Round(v.z));
}
inline Vec3 Trunc(const Vec3& v)
{
	return Vec3((int)v.x, (int)v.y, (int)v.z);
}

//////////////////////////////////////////////////////////////////////////cmp



//////////////////////////////////////////////////////////////////////////
inline Vec4& Vec4::operator =(const Vec4& xyzw)
{
	x = xyzw.x;		y = xyzw.y;		z = xyzw.z;		w = xyzw.w;
	return *this;
}
inline Vec4& Vec4::operator = (float xyzw)
{
	x = y = z = w = xyzw;
	return *this;
}

inline Vec4::Vec4(float xyzw)
{
	x = y = z = w = xyzw;
}
inline Vec4::Vec4(float _x, float _y, float _z, float _w)
{
	x = _x;
	y = _y;
	z = _z;	
	w = _w;
}
inline Vec4::Vec4(const Vec2& xy, const Vec2& zw)
{
	x = xy.x;		y = xy.y;
	z = zw.x;		w = zw.y;
}
inline Vec4::Vec4(const Vec3& xyz, float _w)
{
	x = xyz.x;		
	y = xyz.y;		
	z = xyz.z;		
	w = _w;
}
inline Vec4::Vec4(float _x, const Vec3& yzw)
{
	x = _x;		
	y = yzw.x;		
	z = yzw.y;	
	w = yzw.z;
}
inline Vec4::Vec4(const Vec4& xyzw)
{
	x = xyzw.x;		
	y = xyzw.y;		
	z = xyzw.z;		
	w = xyzw.w;
}


//////////////////////////////////////////////////////////////////////////Vec4 x Vec4
inline Vec4 operator + (const Vec4& a, const Vec4& b)
{
	return Vec4(
		a.x + b.x,
		a.y + b.y,
		a.z + b.z,
		a.w + b.w);
}
inline Vec4 operator - (const Vec4& a, const Vec4& b)
{
	return Vec4(
		a.x - b.x,
		a.y - b.y,
		a.z - b.z,
		a.w - b.w);
}
inline Vec4 operator * (const Vec4& a, const Vec4& b)
{
	return Vec4(
		a.x * b.x,
		a.y * b.y,
		a.z * b.z,
		a.w * b.w);
}
inline Vec4 operator / (const Vec4& a, const Vec4& b)
{
	return Vec4(
		a.x / b.x,
		a.y / b.y,
		a.z / b.z,
		a.w / b.w);
}

//////////////////////////////////////////////////////////////////////////Vec4 x float
inline Vec4 operator + (const Vec4& v, float f)
{
	return Vec4(
		v.x + f,
		v.y + f,
		v.z + f,
		v.w + f);
}
inline Vec4 operator - (const Vec4& v, float f)
{
	return Vec4(
		v.x - f,
		v.y - f,
		v.z - f,
		v.w - f);
}
inline Vec4 operator * (const Vec4& v, float f)
{
	return Vec4(
		v.x * f,
		v.y * f,
		v.z * f,
		v.w * f);
}
inline Vec4 operator / (const Vec4& v, float f)
{
	return Vec4(
		v.x / f,
		v.y / f,
		v.z / f,
		v.w / f);
}
//////////////////////////////////////////////////////////////////////////float x Vec4
inline Vec4 operator + (float f, const Vec4& v)
{
	return Vec4(
		f + v.x,
		f + v.y,
		f + v.z,
		f + v.w);
}
inline Vec4 operator - (float f, const Vec4& v)
{
	return Vec4(
		f - v.x,
		f - v.y,
		f - v.z,
		f - v.w);
}
inline Vec4 operator * (float f, const Vec4& v)
{
	return Vec4(
		f * v.x,
		f * v.y,
		f * v.z,
		f * v.w);
}
inline Vec4 operator / (float f, const Vec4& v)
{
	return Vec4(
		f / v.x,
		f / v.y,
		f / v.z,
		f / v.w);
}

//////////////////////////////////////////////////////////////////////////Vec4 += Vec4
inline Vec4& operator + (Vec4& a, const Vec4& b)
{
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	a.w += b.w;
	return a;
}
inline Vec4& operator - (Vec4& a, const Vec4& b)
{
	a.x -= b.x;
	a.y -= b.y;
	a.z -= b.z;
	a.w -= b.w;
	return a;
}
inline Vec4& operator * (Vec4& a, const Vec4& b)
{
	a.x *= b.x;
	a.y *= b.y;
	a.z *= b.z;
	a.w *= b.w;
	return a;
}
inline Vec4& operator / (Vec4& a, const Vec4& b)
{
	a.x /= b.x;
	a.y /= b.y;
	a.z /= b.z;
	a.w /= b.w;
	return a;
}
//////////////////////////////////////////////////////////////////////////Vec4 x= float
inline Vec4& operator += (Vec4& a, float f)
{
	a.x += f;
	a.y += f;
	a.z += f;
	a.w += f;
	return a;
}
inline Vec4& operator -= (Vec4& a, float f)
{
	a.x -= f;
	a.y -= f;
	a.z -= f;
	a.w -= f;
	return a;
}
inline Vec4& operator *= (Vec4& a, float f)
{
	a.x *= f;
	a.y *= f;
	a.z *= f;
	a.w *= f;
	return a;
}
inline Vec4& operator /= (Vec4& a, float f)
{
	a.x /= f;
	a.y /= f;
	a.z /= f;
	a.w /= f;
	return a;
}



//////////////////////////////////////////////////////////////////////////
inline bool Vec4::IsNearlyEqual(const Vec4& v, float epsilon) const
{
	return Abs(x - v.x) < epsilon && Abs(y - v.y) < epsilon && Abs(z - v.z) < epsilon && Abs(w - v.w) < epsilon;
}
inline bool Vec4::IsNearlyZero(float epsilon) const
{
	return Abs(x) < epsilon && Abs(y) < epsilon && Abs(z) < epsilon && Abs(w) < epsilon;
}
inline bool Vec4::IsNormalized(float epsilon) const
{
	return Abs(LengthSquare() - 1.0f) < epsilon;
}
inline bool Vec4::IsFinite() const
{
	return UCore::IsFinite(x) && UCore::IsFinite(y) && UCore::IsFinite(z) && UCore::IsFinite(w);

}

inline float Vec4::LengthSquare() const
{
	return (x * x + y * y + z * z + w * w);
}
inline float Vec4::Length() const
{
	return Sqrt(LengthSquare());
}
inline float Vec4::Length3Square() const
{
	return (x * x + y * y + z * z);
}
inline float Vec4::Length3() const
{
	return Sqrt(Length3Square());
}
inline void Vec4::Normalize()
{
	*this *= RSqrt(LengthSquare());
}
inline Vec4 Vec4::GetNormalized() const
{
	Vec4 ret(*this);
	ret.Normalize();
	return ret;
}
inline Vec4 Vec4::GetXXXX() const
{
	return Vec4(x);
}
inline Vec4 Vec4::GetYYYY() const
{
	return Vec4(y);
}
inline Vec4 Vec4::GetZZZZ() const
{
	return Vec4(z);
}
inline Vec4 Vec4::GetWWWW() const
{
	return Vec4(w);
}

//////////////////////////////////////////////////////////////////////////
inline float Dot(const Vec4& a, const Vec4& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}
inline float operator | (const Vec4& a, const Vec4& b)
{
	return Dot(a, b);
}
inline Vec4 Abs(const Vec4& v)
{
	return Vec4(Abs(v.x), Abs(v.y), Abs(v.z), Abs(v.w));
}
inline Vec4 Min(const Vec4& v0, const Vec4& v1)
{
	return Vec4(Min(v0.x, v1.x), Min(v0.y, v1.y), Min(v0.z, v1.z), Min(v0.w, v1.w));
}
inline Vec4 Max(const Vec4& v0, const Vec4& v1)
{
	return Vec4(Max(v0.x, v1.x), Max(v0.y, v1.y), Max(v0.z, v1.z), Max(v0.w, v1.w));
}
inline Vec4 Lerp(const Vec4& v0, const Vec4& v1, float t)
{
	return v0 * (1 - t) + v1 * t;
}
inline Vec4 Clamp(const Vec4& value, float min, float max)
{
	return Vec4(Clamp(value.x, min, max), Clamp(value.y, min, max), Clamp(value.z, min, max), Clamp(value.w, min, max));
}
inline Vec4 Clamp(const Vec4& value, const Vec4& min, const Vec4& max)
{
	return Vec4(Clamp(value.x, min.x, max.x), Clamp(value.y, min.y, max.y), Clamp(value.z, min.z, max.z), Clamp(value.w, min.w, max.w));
}
inline Vec4 Floor(const Vec4& v)
{
	return Vec4(Floor(v.x), Floor(v.y), Floor(v.z), Floor(v.w));
}
inline Vec4 Ceil(const Vec4& v)
{
	return Vec4(Ceil(v.x), Ceil(v.y), Ceil(v.z), Ceil(v.w));
}
inline Vec4 Round(const Vec4& v)
{
	return Vec4(Round(v.x), Round(v.y), Round(v.z), Round(v.w));
}
inline Vec4 Trunc(const Vec4& v)
{
	return Vec4((int)v.x, (int)v.y, (int)v.z, (int)v.w);
}
