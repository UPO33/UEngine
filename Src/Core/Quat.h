#pragma once

#include "Vector.h"

namespace UCore
{
	/*
		cumulatedQuat = thirdQuat * secondQuat * fistQuat;
	*/
	struct UCORE_API alignas(16) Quat
	{
		float x, y, z, w;

		static const Quat IDENTITY;

		Quat() {}
		explicit Quat(float x, float y, float z, float w) { x = x; y = y; z = z; w = w; }
		explicit Quat(const Vec3& axisNormalized, float angleDegree);

		
		Quat operator * (const Quat&) const;
		Quat& operator *= (const Quat&);

		bool IsFinite() const
		{
			return ((const Vec4*)this)->IsFinite();
		}
		bool IsInvertible(float epsilon = FLOAT_EPSILON) const
		{
			return MagnitudeSquare() > epsilon && IsFinite();
		}
		
		void Conjugate();
		Quat GetConjugate() const;
		void Invert();
		Quat GetInverse() const;

		float MagnitudeSquare() const { return ((Vec4*)this)->LengthSquare(); }
		float Magnitude() const { return ((Vec4*)this)->Length(); }

		void Normalize() { return ((Vec4*)this)->Normalize(); }
		Vec3 Rotate(const Vec3& v) const;
		Vec3 RotateInv(const Vec3& v) const;

		//the rotation is applied from right, for instance MakeRotationABC first apply C then B then A
		//angle are in degree
		static Quat MakeRotationXYZ(float x, float y, float z);
		static Quat MakeRotationZYX(float z, float y, float x);
		
		static Quat MakeRotationX(float angleDegree);
		static Quat MakeRotationY(float angleDegree);
		static Quat MakeRotationZ(float angleDegree);

		void GetAxisAngle(Vec3& outAxis, float& outAngleDegree);

		bool IsSimilar(const Quat& r, float epsilon = FLOAT_EPSILON) const;

		Vec3 operator * (const Vec3& v) const { return Rotate(v); }
		explicit operator float*() const { return (float*)this; }
	};

	inline float Dot(const Quat& q0, const Quat& q1)
	{
		return Dot((const Vec4&)q0, (const Vec4&)q1);
	}

	UCORE_API StringStreamOut& operator << (StringStreamOut&, const Quat&);
};