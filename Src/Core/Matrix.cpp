#include "Matrix.h"
#include "ByteSerializer.h"

namespace UCore
{
	const Matrix3 Matrix3::IDENTITY(Vec3(1, 0, 0), Vec3(0, 1, 0), Vec3(0, 0, 1));
	const Matrix3 Matrix3::ZERO(Vec3::ZERO, Vec3::ZERO, Vec3::ZERO);

	const Matrix4 Matrix4::IDENTITY(Vec4(1, 0, 0, 0), Vec4(0, 1, 0, 0), Vec4(0, 0, 1, 0), Vec4(0, 0, 0, 1));
	const Matrix4 Matrix4::ZERO(Vec4::Zero, Vec4::Zero, Vec4::Zero, Vec4::Zero);



	void Matrix4::GetTranspose(Matrix4& out) const
	{
		UASSERT(this != &out);
		out.mElements[0][0] = mElements[0][0];
		out.mElements[0][1] = mElements[1][0];
		out.mElements[0][2] = mElements[2][0];
		out.mElements[0][3] = mElements[3][0];

		out.mElements[1][0] = mElements[0][1];
		out.mElements[1][1] = mElements[1][1];
		out.mElements[1][2] = mElements[2][1];
		out.mElements[1][3] = mElements[3][1];

		out.mElements[2][0] = mElements[0][2];
		out.mElements[2][1] = mElements[1][2];
		out.mElements[2][2] = mElements[2][2];
		out.mElements[2][3] = mElements[3][2];

		out.mElements[3][0] = mElements[0][3];
		out.mElements[3][1] = mElements[1][3];
		out.mElements[3][2] = mElements[2][3];
		out.mElements[3][3] = mElements[3][3];
	}

	void Matrix4::Transpose()
	{
		Swap(mElements[0][1], mElements[1][0]);
		Swap(mElements[0][2], mElements[2][0]);
		Swap(mElements[0][3], mElements[3][0]);
		Swap(mElements[1][2], mElements[2][1]);
		Swap(mElements[1][3], mElements[3][1]);
		Swap(mElements[2][3], mElements[3][2]);
	}

	void Matrix4::Transpose3x3()
	{
		Swap(mElements[0][1], mElements[1][0]);
		Swap(mElements[0][2], mElements[2][0]);
		Swap(mElements[1][2], mElements[2][1]);
	}

	void Matrix4::TransformVec4(const Vec4& v, Vec4& out) const
	{
		UASSERT(&v != &out);
 		out = GetColumn(0) * v.x + GetColumn(1) * v.y + GetColumn(2) * v.z + GetColumn(3) * v.w;

		//out.x = (mElements[0][0] * v.x) + (mElements[1][0] * v.y) + (mElements[2][0] * v.z) + (mElements[3][0] * v.w);
		//out.y = (mElements[0][1] * v.x) + (mElements[1][1] * v.y) + (mElements[2][1] * v.z) + (mElements[3][1] * v.w);
		//out.z = (mElements[0][2] * v.x) + (mElements[1][2] * v.y) + (mElements[2][2] * v.z) + (mElements[3][2] * v.w);
		//out.w = (mElements[0][3] * v.x) + (mElements[1][3] * v.y) + (mElements[2][3] * v.z) + (mElements[3][3] * v.w);

	}
	
	void Matrix4::TransformNormal(const Vec3& v, Vec3& out) const
	{
		UASSERT(&v != &out);

		out.x = (mElements[0][0] * v.x) + (mElements[1][0] * v.y) + (mElements[2][0] * v.z);
		out.y = (mElements[0][1] * v.x) + (mElements[1][1] * v.y) + (mElements[2][1] * v.z);
		out.z = (mElements[0][2] * v.x) + (mElements[1][2] * v.y) + (mElements[2][2] * v.z);
	}

	void Matrix4::TransformPoint(const Vec3& v, Vec3& out) const
	{
		UASSERT(&v != &out);

		out = Vec3((GetColumn(0) * v.x + GetColumn(1) * v.y + GetColumn(2) * v.z) + GetColumn(3));

		//out.x = (mElements[0][0] * v.x) + (mElements[1][0] * v.y) + (mElements[2][0] * v.z) + (mElements[3][0]);
		//out.y = (mElements[0][1] * v.x) + (mElements[1][1] * v.y) + (mElements[2][1] * v.z) + (mElements[3][1]);
		//out.z = (mElements[0][2] * v.x) + (mElements[1][2] * v.y) + (mElements[2][2] * v.z) + (mElements[3][2]);
	}

	void Matrix4::Mul(const Matrix4& a, const Matrix4& b, Matrix4& out)
	{
		out.mElements[0][0] = a.GetRow(0) | b.GetColumn(0);
		out.mElements[0][1] = a.GetRow(1) | b.GetColumn(0);
		out.mElements[0][2] = a.GetRow(2) | b.GetColumn(0);
		out.mElements[0][3] = a.GetRow(3) | b.GetColumn(0);

		out.mElements[1][0] = a.GetRow(0) | b.GetColumn(1);
		out.mElements[1][1] = a.GetRow(1) | b.GetColumn(1);
		out.mElements[1][2] = a.GetRow(2) | b.GetColumn(1);
		out.mElements[1][3] = a.GetRow(3) | b.GetColumn(1);

		out.mElements[2][0] = a.GetRow(0) | b.GetColumn(2);
		out.mElements[2][1] = a.GetRow(1) | b.GetColumn(2);
		out.mElements[2][2] = a.GetRow(2) | b.GetColumn(2);
		out.mElements[2][3] = a.GetRow(3) | b.GetColumn(2);

		out.mElements[3][0] = a.GetRow(0) | b.GetColumn(3);
		out.mElements[3][1] = a.GetRow(1) | b.GetColumn(3);
		out.mElements[3][2] = a.GetRow(2) | b.GetColumn(3);
		out.mElements[3][3] = a.GetRow(3) | b.GetColumn(3);
	}

	void Matrix4::SetTranslation(const Vec3& translation)
	{
		mColumns[3].x = translation.x;
		mColumns[3].y = translation.y;
		mColumns[3].z = translation.z;
	}

	inline float getCofactor(float m0, float m1, float m2,
		float m3, float m4, float m5,
		float m6, float m7, float m8)
	{
		return m0 * (m4 * m8 - m5 * m7) -
			m1 * (m3 * m8 - m5 * m6) +
			m2 * (m3 * m7 - m4 * m6);
	}

	void Matrix4::Invert3x3()
	{
		float determinant;
		float tmp[9];

		tmp[0] = mElements[1][1] * mElements[2][2] - mElements[1][2] * mElements[2][1];
		tmp[1] = mElements[0][2] * mElements[2][1] - mElements[0][1] * mElements[2][2];
		tmp[2] = mElements[0][1] * mElements[1][2] - mElements[0][2] * mElements[1][1];

		tmp[3] = mElements[1][2] * mElements[2][0] - mElements[1][0] * mElements[2][2];
		tmp[4] = mElements[0][0] * mElements[2][2] - mElements[0][2] * mElements[2][0];
		tmp[5] = mElements[0][2] * mElements[1][0] - mElements[0][0] * mElements[1][2];

		tmp[6] = mElements[1][0] * mElements[2][1] - mElements[1][1] * mElements[2][0];
		tmp[7] = mElements[0][1] * mElements[2][0] - mElements[0][0] * mElements[2][1];
		tmp[8] = mElements[0][0] * mElements[1][1] - mElements[0][1] * mElements[1][0];

		// check determinant if it is 0
		determinant = mElements[0][0] * tmp[0] + mElements[0][1] * tmp[3] + mElements[0][2] * tmp[6];
		if (Abs(determinant) <= FLOAT_EPSILON)
		{
			ULOG_ERROR("determinant was zero, rotation part is set to Identity");

			mElements[0][0] = 1;
			mElements[0][1] = 0;
			mElements[0][2] = 0;

			mElements[1][0] = 0;
			mElements[1][1] = 1;
			mElements[1][2] = 0;

			mElements[2][0] = 0;
			mElements[2][1] = 0;
			mElements[2][2] = 1;
		}

		float invDeterminant = 1.0f / determinant;

		mElements[0][0] = invDeterminant * tmp[0];
		mElements[0][1] = invDeterminant * tmp[1];
		mElements[0][2] = invDeterminant * tmp[2];

		mElements[1][0] = invDeterminant * tmp[3];
		mElements[1][1] = invDeterminant * tmp[4];
		mElements[1][2] = invDeterminant * tmp[5];

		mElements[2][0] = invDeterminant * tmp[6];
		mElements[2][1] = invDeterminant * tmp[7];
		mElements[2][2] = invDeterminant * tmp[8];
	}

	void Matrix4::InvertAffine()
	{
		Invert3x3();

		// -R^-1 * T
		float x = mElements[3][0];
		float y = mElements[3][1];
		float z = mElements[3][2];

		//translation   =   r.Coulmn0 * x + r.Column1 * y  +  r.Column2 * z
		mElements[3][0] = -(mElements[0][0] * x + mElements[1][0] * y + mElements[2][0] * z);
		mElements[3][1] = -(mElements[0][1] * x + mElements[1][1] * y + mElements[2][1] * z);
		mElements[3][2] = -(mElements[0][2] * x + mElements[1][2] * y + mElements[2][2] * z);

		// last row should be unchanged (0,0,0,1)
		//mElements[0][3] = mElements[1][3] = mElements[2][3] = 0.0f; mElements[3][3] = 1.0f;
	}

	void Matrix4::Invert()
	{
		// get cofactors of minor matrices
		float cofactor0 = getCofactor(mElements[1][1], mElements[1][2], mElements[1][3], mElements[2][1], mElements[2][2], mElements[2][3], mElements[3][1], mElements[3][2], mElements[3][3]);
		float cofactor1 = getCofactor(mElements[1][0], mElements[1][2], mElements[1][3], mElements[2][0], mElements[2][2], mElements[2][3], mElements[3][0], mElements[3][2], mElements[3][3]);
		float cofactor2 = getCofactor(mElements[1][0], mElements[1][1], mElements[1][3], mElements[2][0], mElements[2][1], mElements[2][3], mElements[3][0], mElements[3][1], mElements[3][3]);
		float cofactor3 = getCofactor(mElements[1][0], mElements[1][1], mElements[1][2], mElements[2][0], mElements[2][1], mElements[2][2], mElements[3][0], mElements[3][1], mElements[3][2]);

		// get determinant
		float determinant = mElements[0][0] * cofactor0 - mElements[0][1] * cofactor1 + mElements[0][2] * cofactor2 - mElements[0][3] * cofactor3;
		if (Abs(determinant) <= FLOAT_EPSILON)
		{
			ULOG_ERROR("Determinant is zero, matrix is set to IDENTITY");
			*this = IDENTITY;
			return;
		}

		// get rest of cofactors for adj(M)
		float cofactor4 = getCofactor(mElements[0][1], mElements[0][2], mElements[0][3], mElements[2][1], mElements[2][2], mElements[2][3], mElements[3][1], mElements[3][2], mElements[3][3]);
		float cofactor5 = getCofactor(mElements[0][0], mElements[0][2], mElements[0][3], mElements[2][0], mElements[2][2], mElements[2][3], mElements[3][0], mElements[3][2], mElements[3][3]);
		float cofactor6 = getCofactor(mElements[0][0], mElements[0][1], mElements[0][3], mElements[2][0], mElements[2][1], mElements[2][3], mElements[3][0], mElements[3][1], mElements[3][3]);
		float cofactor7 = getCofactor(mElements[0][0], mElements[0][1], mElements[0][2], mElements[2][0], mElements[2][1], mElements[2][2], mElements[3][0], mElements[3][1], mElements[3][2]);

		float cofactor8 = getCofactor(mElements[0][1], mElements[0][2], mElements[0][3], mElements[1][1], mElements[1][2], mElements[1][3], mElements[3][1], mElements[3][2], mElements[3][3]);
		float cofactor9 = getCofactor(mElements[0][0], mElements[0][2], mElements[0][3], mElements[1][0], mElements[1][2], mElements[1][3], mElements[3][0], mElements[3][2], mElements[3][3]);
		float cofactor10 = getCofactor(mElements[0][0], mElements[0][1], mElements[0][3], mElements[1][0], mElements[1][1], mElements[1][3], mElements[3][0], mElements[3][1], mElements[3][3]);
		float cofactor11 = getCofactor(mElements[0][0], mElements[0][1], mElements[0][2], mElements[1][0], mElements[1][1], mElements[1][2], mElements[3][0], mElements[3][1], mElements[3][2]);

		float cofactor12 = getCofactor(mElements[0][1], mElements[0][2], mElements[0][3], mElements[1][1], mElements[1][2], mElements[1][3], mElements[2][1], mElements[2][2], mElements[2][3]);
		float cofactor13 = getCofactor(mElements[0][0], mElements[0][2], mElements[0][3], mElements[1][0], mElements[1][2], mElements[1][3], mElements[2][0], mElements[2][2], mElements[2][3]);
		float cofactor14 = getCofactor(mElements[0][0], mElements[0][1], mElements[0][3], mElements[1][0], mElements[1][1], mElements[1][3], mElements[2][0], mElements[2][1], mElements[2][3]);
		float cofactor15 = getCofactor(mElements[0][0], mElements[0][1], mElements[0][2], mElements[1][0], mElements[1][1], mElements[1][2], mElements[2][0], mElements[2][1], mElements[2][2]);

		// build inverse matrix = adj(M) / det(M)
		// adjugate of M is the transpose of the cofactor matrix of M
		float invDeterminant = 1.0f / determinant;
		mElements[0][0] = invDeterminant * cofactor0;
		mElements[0][1] = -invDeterminant * cofactor4;
		mElements[0][2] = invDeterminant * cofactor8;
		mElements[0][3] = -invDeterminant * cofactor12;

		mElements[1][0] = -invDeterminant * cofactor1;
		mElements[1][1] = invDeterminant * cofactor5;
		mElements[1][2] = -invDeterminant * cofactor9;
		mElements[1][3] = invDeterminant * cofactor13;

		mElements[2][0] = invDeterminant * cofactor2;
		mElements[2][1] = -invDeterminant * cofactor6;
		mElements[2][2] = invDeterminant * cofactor10;
		mElements[2][3] = -invDeterminant * cofactor14;

		mElements[3][0] = -invDeterminant * cofactor3;
		mElements[3][1] = invDeterminant * cofactor7;
		mElements[3][2] = -invDeterminant * cofactor11;
		mElements[3][3] = invDeterminant * cofactor15;
	}

	Matrix4 Matrix4::GetInverse() const
	{
		Matrix4 ret = *this;
		ret.Invert();
		return ret;
	}

	float Matrix4::Determinant() const
	{
		return	
			mElements[0][0] * getCofactor(mElements[1][1], mElements[1][2], mElements[1][3], mElements[2][1], mElements[2][2], mElements[2][3], mElements[3][1], mElements[3][2], mElements[3][3]) -
			mElements[0][1] * getCofactor(mElements[1][0], mElements[1][2], mElements[1][3], mElements[2][0], mElements[2][2], mElements[2][3], mElements[3][0], mElements[3][2], mElements[3][3]) +
			mElements[0][2] * getCofactor(mElements[1][0], mElements[1][1], mElements[1][3], mElements[2][0], mElements[2][1], mElements[2][3], mElements[3][0], mElements[3][1], mElements[3][3]) -
			mElements[0][3] * getCofactor(mElements[1][0], mElements[1][1], mElements[1][2], mElements[2][0], mElements[2][1], mElements[2][2], mElements[3][0], mElements[3][1], mElements[3][2]);
	}

	bool Matrix4::IsNearlyEqual(const Matrix4& other, float epsilon) const
	{
		for (size_t i = 0; i < 4; i++)
			for (size_t j = 0; j < 4; j++)
				if (Abs(mElements[i][j] - other.mElements[i][j]) > epsilon)
					return false;
		return true;
	}

	void Matrix4::MetaSerialize(ByteSerializer& ser)
	{
		ser.Bytes(this, sizeof(Matrix4));
	}
	void Matrix4::MetaDeserialize(ByteDeserializer& ser)
	{
		ser.Bytes(this, sizeof(Matrix4));
	}

	Quat Matrix4::GetRotationQuat() const
	{
		return Quat::IDENTITY;
		//#TODO
	}

	Matrix3& Matrix3::operator=(const Matrix3& copy)
	{
		mElements[0][0] = copy.mElements[0][0];
		mElements[0][1] = copy.mElements[0][1];
		mElements[0][2] = copy.mElements[0][2];

		mElements[1][0] = copy.mElements[1][0];
		mElements[1][1] = copy.mElements[1][1];
		mElements[1][2] = copy.mElements[1][2];

		mElements[2][0] = copy.mElements[2][0];
		mElements[2][1] = copy.mElements[2][1];
		mElements[2][2] = copy.mElements[2][2];
		return *this;
	}

	Matrix3::Matrix3(const Matrix3& copy)
	{
		mElements[0][0] = copy.mElements[0][0];
		mElements[0][1] = copy.mElements[0][1];
		mElements[0][2] = copy.mElements[0][2];

		mElements[1][0] = copy.mElements[1][0];
		mElements[1][1] = copy.mElements[1][1];
		mElements[1][2] = copy.mElements[1][2];

		mElements[2][0] = copy.mElements[2][0];
		mElements[2][1] = copy.mElements[2][1];
		mElements[2][2] = copy.mElements[2][2];
	}

	Matrix3::Matrix3(float all)
	{
		mElements[0][0] = all;
		mElements[0][1] = all;
		mElements[0][2] = all;

		mElements[1][0] = all;
		mElements[1][1] = all;
		mElements[1][2] = all;

		mElements[2][0] = all;
		mElements[2][1] = all;
		mElements[2][2] = all;
	}

	Matrix3::Matrix3(float f0, float f1, float f2,
		float f3, float f4, float f5,
		float f6, float f7, float f8)
	{
		mElements[0][0] = f0;
		mElements[0][1] = f1;
		mElements[0][2] = f2;

		mElements[1][0] = f3;
		mElements[1][1] = f4;
		mElements[1][2] = f5;

		mElements[2][0] = f6;
		mElements[2][1] = f7;
		mElements[2][2] = f8;
	}
	Matrix3::Matrix3(const Vec3& c0, const Vec3& c1, const Vec3& c2)
	{
		mColumn[0] = c0;		
		mColumn[1] = c1;
		mColumn[2] = c2;
	}

	

	Matrix3 Matrix3::MakeRotationX(float angleDegree)
	{
		float s, c;
		SinCosDeg(angleDegree, s, c);
		return Matrix3(Vec3(1, 0, 0), Vec3(0, c, s), Vec3(0, -s, c));

	}
	Matrix3 Matrix3::MakeRotationY(float angleDegree)
	{
		float s, c;
		SinCosDeg(angleDegree, s, c);
		return Matrix3(Vec3(c, 0, -s), Vec3(0, 1, 0), Vec3(s, 0, c));
	}

	Matrix3 Matrix3::MakeRotationZ(float angleDegree)
	{
		float s, c;
		SinCosDeg(angleDegree, s, c);
		return Matrix3(Vec3(c, s, 0), Vec3(-s, c, 0), Vec3(0, 0, 1));
	}

	void Matrix3::TransformVector(const Vec3& v, Vec3& out) const
	{
		UASSERT(&v != &out);

// 		out = GetColumn(0) * v.mX + GetColumn(1) * v.mY + GetColumn(2) * v.mZ;

		out.x = (mElements[0][0] * v.x) + (mElements[1][0] * v.y) + (mElements[2][0] * v.z);
		out.y = (mElements[0][1] * v.x) + (mElements[1][1] * v.y) + (mElements[2][1] * v.z);
		out.z = (mElements[0][2] * v.x) + (mElements[1][2] * v.y) + (mElements[2][2] * v.z);
	}

	void Matrix3::GetTranspose(Matrix3& out) const
	{
		UASSERT(this != &out);
		out.mElements[0][0] = mElements[0][0];
		out.mElements[0][1] = mElements[1][0];
		out.mElements[0][2] = mElements[2][0];

		out.mElements[1][0] = mElements[0][1];
		out.mElements[1][1] = mElements[1][1];
		out.mElements[1][2] = mElements[2][1];

		out.mElements[2][0] = mElements[0][2];
		out.mElements[2][1] = mElements[1][2];
		out.mElements[2][2] = mElements[2][2];
	}

	void Matrix3::Transpose()
	{
		/*
		0	3	6
		1	4	7
		2	5	8

		std::swap(m[1],  m[3]);
		std::swap(m[2],  m[6]);
		std::swap(m[5],  m[7]);
		*/

		Swap(mElements[0][1], mElements[1][0]);
		Swap(mElements[0][2], mElements[2][0]);
		Swap(mElements[1][2], mElements[2][1]);
	}

	const Vec3& Matrix3::GetColumn(unsigned index) const
	{
		UASSERT(index < 3);
		return mColumn[index];
	}

	Vec3 Matrix3::GetRow(unsigned index) const
	{
		UASSERT(index < 3);
		return Vec3(mElements[0][index], mElements[1][index], mElements[2][index]);
	}

	void Matrix3::MetaSerialize(ByteSerializer& ser)
	{
		ser.Bytes(this, sizeof(Matrix3));
	}
	void Matrix3::MetaDeserialize(ByteDeserializer& ser)
	{
		ser.Bytes(this, sizeof(Matrix3));
	}
	void Matrix3::Mul(const Matrix3& a, const Matrix3& b, Matrix3& out)
	{
		out.mElements[0][0] = a.GetRow(0) | b.GetColumn(0);
		out.mElements[0][1] = a.GetRow(1) | b.GetColumn(0);
		out.mElements[0][2] = a.GetRow(2) | b.GetColumn(0);

		out.mElements[1][0] = a.GetRow(0) | b.GetColumn(1);
		out.mElements[1][1] = a.GetRow(1) | b.GetColumn(1);
		out.mElements[1][2] = a.GetRow(2) | b.GetColumn(1);

		out.mElements[2][0] = a.GetRow(0) | b.GetColumn(2);
		out.mElements[2][1] = a.GetRow(1) | b.GetColumn(2);
		out.mElements[2][2] = a.GetRow(2) | b.GetColumn(2);
	}

	float Matrix3::Determinant() const
	{
		return	mElements[0][0] * (mElements[1][1] * mElements[2][2] - mElements[1][2] * mElements[2][1]) -
			mElements[0][1] * (mElements[1][0] * mElements[2][2] - mElements[1][2] * mElements[2][0]) +
			mElements[0][2] * (mElements[1][0] * mElements[2][1] - mElements[1][1] * mElements[2][0]);
	}

	void Matrix3::Invert()
	{
		float determinant;
		float tmp[9];

		tmp[0] = mElements[1][1] * mElements[2][2] - mElements[1][2] * mElements[2][1];
		tmp[1] = mElements[0][2] * mElements[2][1] - mElements[0][1] * mElements[2][2];
		tmp[2] = mElements[0][1] * mElements[1][2] - mElements[0][2] * mElements[1][1];
		tmp[3] = mElements[1][2] * mElements[2][0] - mElements[1][0] * mElements[2][2];
		tmp[4] = mElements[0][0] * mElements[2][2] - mElements[0][2] * mElements[2][0];
		tmp[5] = mElements[0][2] * mElements[1][0] - mElements[0][0] * mElements[1][2];
		tmp[6] = mElements[1][0] * mElements[2][1] - mElements[1][1] * mElements[2][0];
		tmp[7] = mElements[0][1] * mElements[2][0] - mElements[0][0] * mElements[2][1];
		tmp[8] = mElements[0][0] * mElements[1][1] - mElements[0][1] * mElements[1][0];

		// check determinant if it is 0
		determinant = mElements[0][0] * tmp[0] + mElements[0][1] * tmp[3] + mElements[0][2] * tmp[6];
		if (Abs(determinant) <= FLOAT_EPSILON)
		{
			SetIndentity();
			return;
		}

		float invDeterminant = 1.0f / determinant;
		mElements[0][0] = invDeterminant * tmp[0];
		mElements[0][1] = invDeterminant * tmp[1];
		mElements[0][2] = invDeterminant * tmp[2];
		mElements[1][0] = invDeterminant * tmp[3];
		mElements[1][1] = invDeterminant * tmp[4];
		mElements[1][2] = invDeterminant * tmp[5];
		mElements[2][0] = invDeterminant * tmp[6];
		mElements[2][1] = invDeterminant * tmp[7];
		mElements[2][2] = invDeterminant * tmp[8];
	}






	UCORE_API StringStreamOut& operator<<(StringStreamOut& stream, const Matrix4& matrix)
	{
		stream << matrix.GetRow(0);
		stream << "\n";
		stream << matrix.GetRow(1);
		stream << "\n";
		stream << matrix.GetRow(2);
		stream << "\n";
		stream << matrix.GetRow(3);
		return stream;
	}

	UCORE_API StringStreamOut& operator<<(StringStreamOut& stream, const Matrix3& matrix)
	{
		stream << matrix.GetRow(0);
		stream << "\n";
		stream << matrix.GetRow(1);
		stream << "\n";
		stream << matrix.GetRow(2);
		stream << "\n";
		return stream;
	}

	//the rotation is applied to x then y then z
	Matrix4RotationXYZ::Matrix4RotationXYZ(const Vec3& xyz)
		: Matrix4(Matrix4RotationZ(xyz.z) * Matrix4RotationY(xyz.y) * Matrix4RotationX(xyz.x))
	{
		
	}
	//first scale or rotation should be applied then translation
	Matrix4Transformation::Matrix4Transformation(const Vec3& translation, const Vec3& rotation, const Vec3& scale)
		: Matrix4(Matrix4Translation(translation) * Matrix4RotationXYZ(rotation) * Matrix4Scale(scale))
	{

	}

};

