#include "Quat.h"



namespace UCore
{
	const Quat Quat::IDENTITY(0, 0, 0, 1);

	Quat::Quat(const Vec3& axisNormalized, float angleDegree)
	{
		UASSERT(axisNormalized.IsFinite() && axisNormalized.IsNormalized());
		
		float fSin, fCos;
		SinCosDeg(angleDegree * 0.5f, fSin, fCos);
		x = axisNormalized.x * fSin;
		y = axisNormalized.y * fSin;
		z = axisNormalized.z * fSin;
		w = fCos;
	}

	Vec3 Quat::Rotate(const Vec3& v) const
	{
		const float vx = 2.0f*v.x;
		const float vy = 2.0f*v.y;
		const float vz = 2.0f*v.z;
		const float w2 = w*w - 0.5f;
		const float dot2 = (x*vx + y*vy + z*vz);
		return Vec3
		(
			(vx*w2 + (y * vz - z * vy)*w + x*dot2),
			(vy*w2 + (z * vx - x * vz)*w + y*dot2),
			(vz*w2 + (x * vy - y * vx)*w + z*dot2)
		);
	}

	Vec3 Quat::RotateInv(const Vec3& v) const
	{
		const float vx = 2.0f*v.x;
		const float vy = 2.0f*v.y;
		const float vz = 2.0f*v.z;
		const float w2 = w*w - 0.5f;
		const float dot2 = (x*vx + y*vy + z*vz);
		return Vec3
		(
			(vx*w2 - (y * vz - z * vy)*w + x*dot2),
			(vy*w2 - (z * vx - x * vz)*w + y*dot2),
			(vz*w2 - (x * vy - y * vx)*w + z*dot2)
		);
	}



	Quat Quat::MakeRotationXYZ(float x, float y, float z)
	{
		return Quat(Vec3(1, 0, 0), x) * Quat(Vec3(0, 1, 0), y) * Quat(Vec3(0, 0, 1), z);
	}
	Quat Quat::MakeRotationZYX(float z, float y, float x)
	{
		return Quat(Vec3(0, 0, 1), z) * Quat(Vec3(0, 1, 0), y) * Quat(Vec3(1, 0, 0), x);
	}

	Quat Quat::MakeRotationX(float angleDegree)
	{
		return Quat(Vec3(1, 0, 0), angleDegree);
	}

	Quat Quat::MakeRotationY(float angleDegree)
	{
		return Quat(Vec3(0, 1, 0), angleDegree);
	}

	Quat Quat::MakeRotationZ(float angleDegree)
	{
		return Quat(Vec3(0, 0, 1), angleDegree);
	}

	void Quat::GetAxisAngle(Vec3& outAxis, float& outAngleDegree)
	{
		float rcpSin = RSqrt(1 - w*w);
		outAxis.x = x * rcpSin;
		outAxis.y = y * rcpSin;
		outAxis.z = z * rcpSin;

		outAngleDegree = ACos(w) * (2 * RAD2DEG);
	}

	bool Quat::IsSimilar(const Quat& r, float epsilon) const
	{
		return Abs(Dot(*this, r) - 1.0) < epsilon;
	}

	Quat Quat::GetConjugate() const
	{
		return Quat(-x, -y, -z, w);
	}

	void Quat::Invert()
	{
		UASSERT(IsInvertible());
		float rLengthSq = 1.0f / MagnitudeSquare();

		x = -x * rLengthSq;
		y = -y * rLengthSq;
		z = -z * rLengthSq;
		w = w * rLengthSq;
	}

	Quat Quat::GetInverse() const
	{
		Quat ret(*this);
		ret.Invert();
		return ret;
	}

	

	void Quat::Conjugate()
	{
		x = -x;
		y = -y;
		z = -z;
	}

	Quat& Quat::operator*=(const Quat& q2)
	{
		float xx =  x * q2.w + y * q2.z - z * q2.y + w * q2.x;
		float yy = -x * q2.z + y * q2.w + z * q2.x + w * q2.y;
		float zz =  x * q2.y - y * q2.x + z * q2.w + w * q2.z;
		float ww = -x * q2.x - y * q2.y - z * q2.z + w * q2.w;
		x = xx;
		y = yy;
		z = zz;
		w = ww;
		return *this;
	}

	Quat Quat::operator*(const Quat& q2) const
	{
		Quat ret = *this;
		ret *= q2;
		return ret;
	}


	UCORE_API StringStreamOut& operator<<(StringStreamOut& stream, const Quat& q)
	{
		stream << "{" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << "}";
		return stream;
	}
};