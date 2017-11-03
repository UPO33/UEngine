#pragma once

#include "Base.h"
#include "String.h"
#ifdef UCOMPILER_MSVC
#include "SIMD.h"
#endif


namespace UCore
{
	static const float VECTOR_ESPILON = FLOAT_EPSILON;
	static const float VECTOR_COMPARISON_DELTA = 0.000001f;

	//////////////////////////////////////////////////////////////////////////
	struct Vec2;
	struct Vec3;
	struct Vec4;
	struct Color;
	struct Color32;

	template <typename T> struct TVec2
	{
		typedef T Type;

		T x, y;

		inline TVec2() {}
		explicit inline TVec2(T xy) { x = y = xy; }
		inline TVec2(T _x, T _y) { x = _x;		y = _y; }
		inline TVec2(const TVec2& v) { x = v.x; y = v.y; }

		inline T& operator [] (size_t i) { return ((T*)this)[i]; }
		inline const T& operator [] (size_t i) const { return ((T*)this)[i]; }

		inline TVec2& operator = (T xy) { x = y = xy;  return *this; }
		inline TVec2& operator = (const T& c) { x = c.mX;	y = c.mY;   return *this; }

		inline TVec2 operator + (const TVec2& v) const { return TVec2(x + v.x, y + v.y); }
		inline TVec2 operator - (const TVec2& v) const { return TVec2(x - v.x, y - v.y); }
		inline TVec2 operator * (const TVec2& v) const { return TVec2(x * v.x, y * v.y); }
		inline TVec2 operator / (const TVec2& v) const { return TVec2(x / v.x, y / v.y); }

		inline TVec2 operator + (T n) const { return TVec2(x + n, y + n); }
		inline TVec2 operator - (T n) const { return TVec2(x - n, y - n); }
		inline TVec2 operator * (T n) const { return TVec2(x * n, y * n); }
		inline TVec2 operator / (T n) const { return TVec2(x / n, y / n); }


		inline TVec2& operator += (const TVec2& v) { x += v.x;	y += v.y;		return *this; }
		inline TVec2& operator -= (const TVec2& v) { x -= v.x;	y -= v.y;		return *this; }
		inline TVec2& operator *= (const TVec2& v) { x *= v.x;	y *= v.y;		return *this; }
		inline TVec2& operator /= (const TVec2& v) { x /= v.x;	y /= v.y;		return *this; }

		inline TVec2& operator += (T n) { x += n;	y += n;	return *this; }
		inline TVec2& operator -= (T n) { x -= n;	y -= n;	return *this; }
		inline TVec2& operator *= (T n) { x *= n;	y *= n;	return *this; }
		inline TVec2& operator /= (T n) { x /= n;	y /= n;	return *this; }


		bool operator > (T n) const { return x > n && y > n; }
		bool operator >= (T n) const { return x >= n && y >= n; }
		bool operator < (T n) const { return x < n && y < n; }
		bool operator <= (T n) const { return x <= n && y <= n; }

		bool operator > (const TVec2& v) const { return x > v.x && y > v.y; }
		bool operator >= (const TVec2& v) const { return x >= v.x && y >= v.y; }
		bool operator < (const TVec2& v) const { return x < v.x && y < v.y; }
		bool operator <= (const TVec2& v) const { return x <= v.x && y <= v.y; }

		bool operator == (const TVec2& v) const { return x == v.x && y == v.y; }
		bool operator != (const TVec2& v) const { return !this->operator==(v); }

		TVec2 operator - () const { return TVec2(-x, -y); }
	};

	typedef TVec2<int> Vec2I;


	//////////////////////////////////////////////////////////////////////////
	struct UCORE_API Vec2
	{
		UCLASS(Vec2)

		static const Vec2 ZERO;
		static const Vec2 ONE;

		float x, y;

		inline Vec2() {}
		explicit inline Vec2(float xy) { x = y = xy; }
		inline Vec2(float _x, float _y) { x = _x;	y = _y; }
		inline Vec2(const Vec2& xy) { x = xy.x;		y = xy.y; }
		inline Vec2(const Vec3&);
		inline Vec2(const Vec2I& v) : x((float)v.x), y((float)v.y) {}

		inline float& operator [] (size_t i) { return ((float*)this)[i]; }
		inline const float& operator [] (size_t i) const { return ((float*)this)[i]; }

		inline Vec2& operator = (const Vec2& xy) { x = xy.x;	y = xy.y;	return *this; }
		inline Vec2& operator = (float xy) { x = y = xy;  return *this; }
		inline Vec2& operator = (const Vec3&);

		inline Vec2 operator + (const Vec2& v) const { return Vec2(x + v.x, y + v.y); }
		inline Vec2 operator - (const Vec2& v) const { return Vec2(x - v.x, y - v.y); }
		inline Vec2 operator * (const Vec2& v) const { return Vec2(x * v.x, y * v.y); }
		inline Vec2 operator / (const Vec2& v) const { return Vec2(x / v.x, y / v.y); }

		inline Vec2 operator + (float s) const { return Vec2(x + s, y + s); }
		inline Vec2 operator - (float s) const { return Vec2(x - s, y - s); }
		inline Vec2 operator * (float s) const { return Vec2(x * s, y * s); }
		inline Vec2 operator / (float s) const { return Vec2(x / s, y / s); }


		inline Vec2& operator += (const Vec2& v) { x += v.x;	y += v.y;		return *this; }
		inline Vec2& operator -= (const Vec2& v) { x -= v.x;	y -= v.y;		return *this; }
		inline Vec2& operator *= (const Vec2& v) { x *= v.x;	y *= v.y;		return *this; }
		inline Vec2& operator /= (const Vec2& v) { x /= v.x;	y /= v.y;		return *this; }

		inline Vec2& operator += (float s) { x += s;	y += s;	return *this; }
		inline Vec2& operator -= (float s) { x -= s;	y -= s;	return *this; }
		inline Vec2& operator *= (float s) { x *= s;	y *= s;	return *this; }
		inline Vec2& operator /= (float s) { x /= s;	y /= s;	return *this; }

		friend Vec2 operator * (float f, const Vec2& v)
		{
			return v * f;
		}
		
		explicit inline operator Vec2I() const
		{
			return Vec2I((int)x, (int)y);
		}

		bool operator > (float s) const { return x > s && y > s; }
		bool operator >= (float s) const { return x >= s && y >= s; }
		bool operator < (float s) const { return x < s && y < s; }
		bool operator <= (float s) const { return x <= s && y <= s; }

		bool operator > (const Vec2& v) const { return x > v.x && y > v.y; }
		bool operator >= (const Vec2& v) const { return x >= v.x && y >= v.y; }
		bool operator < (const Vec2& v) const { return x < v.x && y < v.y; }
		bool operator <= (const Vec2& v) const { return x <= v.x && y <= v.y; }


		bool IsNearlyEqual(const Vec2& v, float epsilon = FLOAT_EPSILON) const
		{
			return Abs(x - v.x) < epsilon && Abs(y - v.y) < epsilon;
		}
		bool IsFinite() const
		{
			return UCore::IsFinite(x) && UCore::IsFinite(y);
		}
		bool IsZero(float epsilon = FLOAT_EPSILON) const
		{
			return Abs(x) < epsilon && Abs(y) < epsilon;
		}
		bool IsNormalized(float epsilon = FLOAT_EPSILON) const
		{
			return Abs(LengthSq() - 1.0f) < epsilon;
		}
		//negate
		Vec2 operator - () const { return Vec2(-x, -y); }
		//dot product
		float operator | (const Vec2& v) const { return x * v.x + y * v.y; }

		float LengthSq() const { return x * x + y * y; }
		float Length() const { return Sqrt(x * x + y *y); }

		void Normalize()
		{
			*this *= RSqrt(LengthSq());
		}
		void NormalizeSafe(const Vec2& errorValue = Vec2(0))
		{
			float len = LengthSq();
			if (len <= VECTOR_ESPILON)
			{
				*this = errorValue;
			}
			else
			{
				*this *= RSqrt(len);
			}
		}
		Vec2 GetNormalized() const
		{
			Vec2 ret = *this;
			ret.Normalize();
			return ret;
		}
		Vec2 GetNormalizedSafe(const Vec2& errorValue = Vec2(0)) const
		{
			Vec2 ret = *this;
			ret.NormalizeSafe(errorValue);
			return ret;
		}
		void StoreTo(float* out) const
		{
			out[0] = x;
			out[1] = y;
		}
		//get normalized
		Vec2 operator ~ () const { return GetNormalizedSafe(); }


		void MetaSerialize(ByteSerializer&);
		void MetaDeserialize(ByteDeserializer&);
	};
	
	inline Vec2 URotatePoint(Vec2 point, float angleRadian)
	{
		float fSin, fCos;
		SinCos(angleRadian, fSin, fCos);
		return Vec2(
			point.x * fCos - point.y * fSin,
			point.x * fSin + point.y * fCos);
	}
	inline Vec2 URotatePoint(Vec2 point, float angleRadian, Vec2 origin)
	{
		float fSin, fCos;
		SinCos(angleRadian, fSin, fCos);
		point -= origin;
		float xx = point.x * fCos - point.y * fSin;
		float yy = point.x * fSin + point.y * fCos;
		return Vec2(xx + origin.x, yy + origin.y);
	}
	inline float Dot(const Vec2& a, const Vec2& b)
	{
		return a | b;
	}
	inline Vec2 Abs(const Vec2& v)
	{
		return Vec2(Abs(v.x), Abs(v.y));
	}
	inline Vec2 Min(const Vec2& v0, const Vec2& v1)
	{
		return Vec2(Min(v0.x, v1.x), Min(v0.y, v1.y));
	}
	inline Vec2 Max(const Vec2& v0, const Vec2& v1)
	{
		return Vec2(Max(v0.x, v1.x), Max(v0.y, v1.y));
	}
	inline Vec2 Lerp(const Vec2& v0, const Vec2& v1, float t)
	{
		return v0 * (1 - t) + v1 * t;
	}
	inline Vec2 Clamp(const Vec2& value, float min, float max)
	{
		return Vec2(Clamp(value.x, min, max), Clamp(value.y, min, max));
	}
	inline Vec2 Clamp(const Vec2& value, const Vec2& min, const Vec2& max)
	{
		return Vec2(Clamp(value.x, min.x, max.x), Clamp(value.y, min.y, max.y));
	}
	inline Vec2 Floor(const Vec2& v)
	{
		return Vec2(Floor(v.x), Floor(v.y));
	}
	inline Vec2 Ceil(const Vec2& v)
	{
		return Vec2(Ceil(v.x), Ceil(v.y));
	}
	inline Vec2 Round(const Vec2& v)
	{
		return Vec2(Round(v.x), Round(v.y));
	}
	inline Vec2 Trunc(const Vec2& v)
	{
		return Vec2((int)v.x, (int)v.y);
	}

#if defined(USE_SSE) || defined(USE_ARM_NEON)
#define UVEC3_ALIGN 16
#else
#define UVEC3_ALIGN 4
#endif

	//////////////////////////////////////////////////////////////////////////
	struct UCORE_API alignas(UVEC3_ALIGN) Vec3
	{
		UCLASS(Vec3)


		static const Vec3 ZERO;
		static const Vec3 ONE;


		float x, y, z;
#if defined(USE_SSE) || defined(USE_ARM_NEON)
		float unused;
#endif

		inline Vec3() {}

		explicit inline Vec3(float xyz);
		inline Vec3(float x, float y, float z);
		inline Vec3(const Vec3& copy);
		inline Vec3(const Vec2& xy, float z);
		inline Vec3(const Vec4& v);
		inline Vec3(const float xyz[3]);

		inline Vec3& operator = (const Vec3& copy);
		inline Vec3& operator = (float f);
		inline Vec3& operator = (const Vec4& v);

		inline float& operator [] (size_t i) { return ((float*)this)[i]; }
		inline const float operator [] (size_t i) const { return ((float*)this)[i]; }

		void StoreTo(float* out) const
		{
			out[0] = x;
			out[1] = y;
			out[2] = z;
		}

		inline bool IsFinite() const;
		inline bool IsNearlyZero(float epsilon = FLOAT_EPSILON) const;
		inline bool IsNearlyEqual(const Vec3& v, float epsilon = FLOAT_EPSILON) const;
		inline bool IsNormalized(float epsilon = FLOAT_EPSILON) const;

		inline float LengthSquare() const;
		inline float Length() const;

		inline Vec3 LengthV() const;
		inline Vec3 LengthSquareV() const;

		inline Vec3 GetXXX() const;
		inline Vec3 GetYYY() const;
		inline Vec3 GetZZZ() const;

		inline void Normalize();

		inline Vec3 GetNormalized() const;
		inline void NormalizeSafe();

		inline Vec3 GetNormalizedSafe() const;

		void MetaSerialize(ByteSerializer&);
		void MetaDeserialize(ByteDeserializer&);
	};


	

	//////////////////////////////////////////////////////////////////////////
	struct UCORE_API alignas(16) Vec4
	{
		UCLASS(Vec4)

		static const Vec4 Zero;
		static const Vec4 One;

		union
		{
			struct
			{
				float x, y, z, w;
			};
			struct
			{
				float r, g, b, a;
			};
		};


		inline Vec4() {}
	
		//////////////////////////////////////////////////////////////////////////
		inline Vec4& operator = (const Vec4& xyzw);
		inline Vec4& operator = (float xyzw);

		//////////////////////////////////////////////////////////////////////////
		explicit inline Vec4(float xyzw);
		inline Vec4(float _x, float _y, float _z, float _w);
		inline Vec4(const Vec2& xy, const Vec2& zw);
		inline Vec4(const Vec3& xyz, float _w);
		inline Vec4(float _x, const Vec3& yzw);
		inline Vec4(const Vec4& xyzw);


		void StoreTo(float* out) const
		{
			out[0] = x;
			out[1] = y;
			out[2] = z;
			out[3] = w;
		}

		inline float& operator [] (size_t i) { return ((float*)this)[i]; }
		inline const float operator [] (size_t i) const { return ((float*)this)[i]; }

		inline operator float* () { return (float*)this; }
		inline operator const float* () const { return (const float*)this; }



		inline bool IsNearlyEqual(const Vec4& v, float epsilon = FLOAT_EPSILON) const;
		inline bool IsNearlyZero(float epsilon = FLOAT_EPSILON) const;

		inline bool IsNormalized(float epsilon = FLOAT_EPSILON) const;
		inline bool IsFinite() const;
		inline float LengthSquare() const;
		inline float Length() const;
		inline float Length3Square() const;
		inline float Length3() const;
		inline void Normalize();
		inline Vec4 GetNormalized() const;

		inline Vec4 GetXXXX() const;
		inline Vec4 GetYYYY() const;
		inline Vec4 GetZZZZ() const;
		inline Vec4 GetWWWW() const;
			 
		void MetaSerialize(ByteSerializer&);
		void MetaDeserialize(ByteDeserializer&);
	};


	//#TODO remove inheritance
	struct UCORE_API Color : public Vec4
	{
		UCLASS(Color)

		static const Color WHITE;
		static const Color RED;
		static const Color GREEN;
		static const Color BLUE;
		static const Color YELLOW;
		static const Color BLACK;
		static const Color PURPLE;
		static const Color PINK;

		inline Color(){}
		explicit inline Color(float _rgba) { r = g = b = a = _rgba; }
		explicit inline Color(float _rgb, float _a) { r = g = b = _rgb;	a = _a; }
		inline Color(float _r, float _g, float _b, float _a) { r = _r;  g = _g;  b = _b;  a = _a; }
		inline Color(const Color32& color);

		void ToString(char* outBuffer, unsigned bufferSize) const;
		String ToString() const;

		bool operator == (const Color& other) const
		{
			return r == other.r && g == other.g && b == other.b && a == other.a;
		}
		bool operator != (const Color& other) const
		{
			return !this->operator==(other);
		}
		void MetaSerialize(ByteSerializer&);
		void MetaDeserialize(ByteDeserializer&);
	};

	//////////////////////////////////////////////////////////////////////////
	struct UCORE_API Color32
	{
		UCLASS(Color32)

		union
		{
			uint32  mColor;
			uint8	mRGBA[4];
		};

		Color32() { mColor = 0; }
		explicit Color32(uint8 rgba) 
		{
			mRGBA[0] = rgba;	mRGBA[1] = rgba;	mRGBA[2] = rgba;	mRGBA[3] = rgba;
		}
		Color32(uint8 r, uint8 g, uint8 b, uint8 a = 255)
		{
			mRGBA[0] = r;	mRGBA[1] = g;	mRGBA[2] = b;	mRGBA[3] = a;
		}
		Color32(const Color& color);

		unsigned GetR() const { return mRGBA[0]; }
		unsigned GetG() const { return mRGBA[1]; }
		unsigned GetB() const { return mRGBA[2]; }
		unsigned GetA() const { return mRGBA[3]; }

		static const Color32 BLACK;
		static const Color32 WHITE;
		static const Color32 RED;
		static const Color32 GREEN;
		static const Color32 BLUE;
		static const Color32 YELLOW;

		void ToString(char* outBuffer, unsigned bufferSize) const;
		String ToString() const;

		void MetaSerialize(ByteSerializer&);
		void MetaDeserialize(ByteDeserializer&);

	};

	//////////////////////////////////////////////////////////////////////////Vec2
	inline Vec2::Vec2(const Vec3& v3)
	{
		x = v3.x;			y = v3.y;
	}
	Vec2& Vec2::operator=(const Vec3& v3)
	{
		x = v3.x;		y = v3.y;		return *this;
	}


	Color::Color(const Color32& color)
	{
		const float s = 1.0f / 255.0f;
		r = color.GetR() * s;
		g = color.GetG() * s;
		b = color.GetB() * s;
		a = color.GetA() * s;
	}








	UCORE_API StringStreamOut& operator << (StringStreamOut&, const Vec2&);
	UCORE_API StringStreamOut& operator << (StringStreamOut&, const Vec3&);
	UCORE_API StringStreamOut& operator << (StringStreamOut&, const Vec4&);
	UCORE_API StringStreamOut& operator << (StringStreamOut&, const Color&);
	UCORE_API StringStreamOut& operator << (StringStreamOut&, Color32);
	UCORE_API StringStreamOut& operator << (StringStreamOut&, Vec2I);

#ifdef USE_SSE
#include "VectorSSE.inl"
#elif defined(USE_ARM_NEON)
#include "VectorARMNeon.inl"
#else
#include "Vector.inl"
#endif

};