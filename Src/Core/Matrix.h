#pragma once

#include "Vector.h"
#include "Quat.h"






namespace UCore
{
	/*
	Notes:
		A matrix  is orthogonal if:
			The transpose is equal to the inverse.
			By making the matrix from a set of mutually perpendicular basis vectors.
			The determinant and eigenvalues are all +1.
			The matrix represents a pure rotation.
	 
		All Orthogonal Matrices have determinants of 1 or -1 and all rotation matrices have determinants of 1
	 */

	//////////////////////////////////////////////////////////////////////////3x3 column major matrix
	struct UCORE_API Matrix3
	{
		UCLASS(Matrix3)

		static const Matrix3 IDENTITY;
		static const Matrix3 ZERO;

		union
		{
			Vec3 mColumn[3];
			float mElements[3][3];	//[column][row]
		};

		Matrix3() {}

		explicit Matrix3(float f0, float f1, float f2,	//column0
				float f3, float f4, float f5,	//column1
				float f6, float f7, float f8);	//column2

		explicit Matrix3(float all);
		
		explicit Matrix3(const Vec3& c0, const Vec3& c1, const Vec3& c2);
		Matrix3(const Matrix3& copy);
		Matrix3& operator = (const Matrix3& copy);

		void SetIndentity() { *this = IDENTITY; }

		static Matrix3 MakeRotationX(float angleDegree);
		static Matrix3 MakeRotationY(float angleDegree);
		static Matrix3 MakeRotationZ(float angleDegree);

		void TransformVector(const Vec3& v, Vec3& out) const;
		Vec3 TransformVector(const Vec3& v) const
		{
			Vec3 ret;
			TransformVector(v, ret);
			return ret;
		}
		void GetTranspose(Matrix3& out) const;
		void Transpose();

		const Vec3& GetColumn(unsigned index) const;
		Vec3 GetRow(unsigned index) const;
		// If the determinant is not 0, then the matrix is invertible
		float Determinant() const;
		void Invert();

		Matrix3 operator * (const Matrix3& m) const
		{
			Matrix3 ret;
			Mul(*this, m, ret);
			return ret;
		}
		Matrix3& operator *= (const Matrix3& m)
		{
			Matrix3 tmp;
			Mul(*this, m, tmp);
			*this = tmp;
			return *this;
		}

		void MetaSerialize(ByteSerializer&);
		void MetaDeserialize(ByteDeserializer&);

		static void Mul(const Matrix3& a, const Matrix3& b, Matrix3& out);
	};


	////////////////////////////////////////////////////////////////////////
	//4x4 column major Matrix
	// the multiplication is from right to left so R = A * B * C, first apply C then B then A
#if 0
	struct UCORE_API Matrix4
	{
		UCLASS(Matrix4)

		static const Matrix4 IDENTITY;
		static const Matrix4 ZERO;

		union
		{
			Vec4	mColumn[4];
			float	mElements[4][4];	//[column][row]
		};

		Matrix4(){}

		explicit Matrix4(float f0, float f1, float f2, float f3,			//column0
				float f4, float f5, float f6, float f7,			//column1
				float f8, float f9, float f10, float f11,		//column2
				float f12, float f13, float f14, float f15);	//column3

		explicit Matrix4(const Vec4& c0, const Vec4& c1, const Vec4& c2, const Vec4& c3);
		Matrix4(const Matrix4& m);
		Matrix4& operator = (const Matrix4& m);

		static Matrix4 MakeTranslation(float x, float y, float z) { return MakeTranslation(Vec3(x, y, z)); }
		static Matrix4 MakeTranslation(const Vec3& position);

		static Matrix4 MakeScale(float scale) { return MakeScale(Vec3(scale)); }
		static Matrix4 MakeScale(float x, float y, float z) { return MakeScale(Vec3(x, y, z)); }
		static Matrix4 MakeScale(const Vec3& scale);
		
		static Matrix4 MakeRotationX(float angleDegree);
		static Matrix4 MakeRotationY(float angleDegree);
		static Matrix4 MakeRotationZ(float angleDegree);

		//#Note:	Make*****ABC(...) is euivalent with A * B * C;

		static Matrix4 MakeRotationXYZ(float x, float y, float z) { return MakeRotationXYZ(Vec3(x, y, z)); }
		static Matrix4 MakeRotationXYZ(const Vec3& xyz);
		//equivalent of RotationZ
		static Matrix4 MakeRotationZXY(const Vec3& zxy);
		static Matrix4 MakeRotationZXY(float z, float x, float y) { return MakeRotationZXY(Vec3(z, x, y)); }

		static Matrix4 MakeRotationDir(const Vec3& forwrad);

		static Matrix4 MakeRotationQuat(const Quat& rotation);

		//equivalent of TranslationMatrix * ScaleMatrix
		static Matrix4 MakeTranslationScale(const Vec3& translation, const Vec3& scale);
		//equivalent of TranslationMatrix * RotationMatrix * ScaleMatrix
		static Matrix4 MakeTranslationRotatioScale(const Vec3& translation, const Quat& rotation, const Vec3& scale);
		static Matrix4 MakeTranslationRotationXYZScale(const Vec3& translation, const Vec3& rotationXYZ, const Vec3& scale);

		static Matrix4 MakeFrustum(float fLeft, float fRight, float fBottom, float fTop, float fNear, float fFar);
		static Matrix4 MakePerspectiveRH(float fovDegree, float aspect, float zNear, float zFar);
		static Matrix4 MakePerspectiveLH(float fovDegree, float aspect, float zNear, float zFar);
		static Matrix4 MakeOrtho(float left, float right, float bottom, float top, float zNear, float zFar);
		static Matrix4 MakeOrtho(float width, float height, float zNear, float zFar);

		bool HasUniformScale(float epsilon = FLOAT_EPSILON) const;
		Vec3 ExtractScale() const;
		const Vec4& GetColumn(unsigned index) const;
		Vec4 GetRow(unsigned index) const;
		void GetTranspose(Matrix4& out) const;
		Matrix4 GetTranspose() const
		{
			Matrix4 ret;	
			GetTranspose(ret);
			return ret;
		}
		void Transpose();
		//transpose rotation part
		void Transpose3x3();
		void InvertAffine();
		void TransformVec4(const Vec4& v, Vec4& out) const;
		Vec4 TransformVec4(const Vec4& v) const
		{
			Vec4 ret;
			TransformVec4(v, ret);
			return ret;
		}
		void TransformNormal(const Vec3& v, Vec3& out) const;
		Vec3 TransformNormal(const Vec3& v) const
		{
			Vec3 ret;
			TransformNormal(v, ret);
			return ret;
		}
		void TransformPoint(const Vec3& v, Vec3& out) const;
		Vec3 TransformPoint(const Vec3& v) const
		{
			Vec3 ret;
			TransformPoint(v, ret);
			return ret;
		}
		
		static void Mul(const Matrix4& a, const Matrix4& b, Matrix4& out);

		Vec3 GetTranslation() const { return Vec3(mColumn[3]); }
		void SetTranslation(const Vec3& translation);

		void RemoveScaling();

		const Vec3& GetRight() const { return (const Vec3&)mColumn[0]; }
		const Vec3& GetUp() const { return (const Vec3&)mColumn[1]; }
		const Vec3& GetForward() const { return (const Vec3&)mColumn[2]; }

		Vec3 GetRotationEuler() const;
		void Invert3x3();
		void Invert();
		Matrix4 GetInverse() const;
		float Determinant() const;
		Matrix4 operator * (const Matrix4& m) const
		{
			Matrix4 ret;
			Mul(*this, m, ret);
			return ret;
		}
		Matrix4& operator *= (const Matrix4& m)
		{
			Matrix4 tmp;
			Mul(*this, m, tmp);
			*this = tmp;
			return *this;
		}
		bool IsEqual(const Matrix4& other, float epsilon = FLOAT_EPSILON) const;
		//transpose of an orthogonal matrix is equal to its inverse
		//Translation, rotation, and reflection are the only orthogonal transformations
		bool IsOrthogonal(float epsilon = 0.000001f);

		void MetaSerialize(ArchiveStream&);

		Quat GetRotationQuat() const;
		Vec3 GetRotationEuler0() const;
		Vec3 GetRotationEuler1() const;
	};
#endif // _DEBUG

	struct UCORE_API Matrix4
	{
		UCLASS(Matrix4)

		static const Matrix4 IDENTITY;
		static const Matrix4 ZERO;

		union 
		{
			Vec4	mColumns[4];
			float	mElements[4][4];	//[column][row]
		};
		


		Matrix4() {}
		Matrix4(InitZero) 
		{
			mColumns[0] = mColumns[1] = mColumns[2] = mColumns[3] = Vec4(0);
		}
		Matrix4(InitDefault)
		{
			mColumns[0] = Vec4(1, 0, 0, 0);
			mColumns[1] = Vec4(0, 1, 0, 0);
			mColumns[2] = Vec4(0, 0, 1, 0);
			mColumns[3] = Vec4(0, 0, 0, 1);
		}
		explicit Matrix4(float f0, float f1, float f2, float f3,			//column0
			float f4, float f5, float f6, float f7,			//column1
			float f8, float f9, float f10, float f11,		//column2
			float f12, float f13, float f14, float f15)		//column3
		{

		}

		float& operator () (size_t column, size_t row)
		{
			return mColumns[column][row];
		}
		float operator () (size_t column, size_t row) const
		{
			return mColumns[column][row];
		}

		explicit Matrix4(const Vec4& c0, const Vec4& c1, const Vec4& c2, const Vec4& c3)
		{
			mColumns[0] = c0;
			mColumns[1] = c1;
			mColumns[2] = c2;
			mColumns[3] = c3;
		}
		Matrix4(const Matrix4& copy)
		{
			mColumns[0] = copy.mColumns[0];
			mColumns[1] = copy.mColumns[1];
			mColumns[2] = copy.mColumns[2];
			mColumns[3] = copy.mColumns[3];
		}
		Matrix4& operator = (const Matrix4& copy)
		{
			mColumns[0] = copy.mColumns[0];
			mColumns[1] = copy.mColumns[1];
			mColumns[2] = copy.mColumns[2];
			mColumns[3] = copy.mColumns[3];
			return *this;
		}


		bool IsUniformScale(float epsilon = FLOAT_EPSILON) const
		{
			Vec3 scale = ExtractScale();
			return Abs(scale.x - scale.y) < epsilon && Abs(scale.y - scale.z) < epsilon;
		}
		Vec3 ExtractScale() const
		{
			return Vec3(GetColumn(0).Length3(), GetColumn(1).Length3(), GetColumn(2).Length3());
		}
		const Vec4& GetColumn(size_t index) const
		{
			return mColumns[index];
		}
		Vec4 GetRow(size_t index) const
		{
			return Vec4(mColumns[0][index], mColumns[1][index], mColumns[2][index], mColumns[3][index]);
		}
		void GetTranspose(Matrix4& out) const;
		Matrix4 GetTranspose() const
		{
			Matrix4 ret;
			GetTranspose(ret);
			return ret;
		}
		void Transpose();
		//transpose rotation part
		void Transpose3x3();
		void InvertAffine();
		void TransformVec4(const Vec4& v, Vec4& out) const;
		Vec4 TransformVec4(const Vec4& v) const
		{
			Vec4 ret;
			TransformVec4(v, ret);
			return ret;
		}
		void TransformNormal(const Vec3& v, Vec3& out) const;
		Vec3 TransformNormal(const Vec3& v) const
		{
			Vec3 ret;
			TransformNormal(v, ret);
			return ret;
		}
		void TransformPoint(const Vec3& v, Vec3& out) const;
		Vec3 TransformPoint(const Vec3& v) const
		{
			Vec3 ret;
			TransformPoint(v, ret);
			return ret;
		}

		static void Mul(const Matrix4& a, const Matrix4& b, Matrix4& out);

		Vec3 GetTranslation() const { return Vec3(mColumns[3]); }
		void SetTranslation(const Vec3& translation);

		void RemoveScaling()
		{
			mColumns[0].Normalize();
			mColumns[1].Normalize();
			mColumns[2].Normalize();
		}

		const Vec3& GetRight() const { return (const Vec3&)mColumns[0]; }
		const Vec3& GetUp() const { return (const Vec3&)mColumns[1]; }
		const Vec3& GetForward() const { return (const Vec3&)mColumns[2]; }
		
		const Vec3& GetXAxis() const { return (const Vec3&)mColumns[0]; }
		const Vec3& GetYAxis() const { return (const Vec3&)mColumns[1]; }
		const Vec3& GetZAxis() const { return (const Vec3&)mColumns[2]; }

		Vec3 GetRotationEuler() const;
		void Invert3x3();
		void Invert();
		Matrix4 GetInverse() const;
		float Determinant() const;
		Matrix4 operator * (const Matrix4& m) const
		{
			Matrix4 ret;
			Mul(*this, m, ret);
			return ret;
		}
		Matrix4& operator *= (const Matrix4& m)
		{
			Matrix4 tmp;
			Mul(*this, m, tmp);
			*this = tmp;
			return *this;
		}
		bool IsNearlyEqual(const Matrix4& other, float epsilon = FLOAT_EPSILON) const;
		//transpose of an orthogonal matrix is equal to its inverse
		//Translation, rotation, and reflection are the only orthogonal transformations
		bool IsOrthogonal(float epsilon = 0.000001f);

		void MetaSerialize(ByteSerializer&);
		void MetaDeserialize(ByteDeserializer&);

		Quat GetRotationQuat() const;
		Vec3 GetRotationEuler0() const;
		Vec3 GetRotationEuler1() const;
	};

	//////////////////////////////////////////////////////////////////////////
	struct Matrix4Identity : Matrix4
	{
		Matrix4Identity()
		{
			mColumns[0] = Vec4(1, 0, 0, 0);
			mColumns[1] = Vec4(0, 1, 0, 0);
			mColumns[2] = Vec4(0, 0, 1, 0);
			mColumns[3] = Vec4(0, 0, 0, 1);
		}
	};
	//////////////////////////////////////////////////////////////////////////
	struct Matrix4Scale : Matrix4
	{
		Matrix4Scale(float uniformScale)
		{
			mColumns[0] = Vec4(uniformScale, 0, 0, 0);
			mColumns[1] = Vec4(0, uniformScale, 0, 0);
			mColumns[2] = Vec4(0, 0, uniformScale, 0);
			mColumns[3] = Vec4(0, 0, 0, 1);
		}
		Matrix4Scale(float x, float y, float z)
		{
			mColumns[0] = Vec4(x, 0, 0, 0);
			mColumns[1] = Vec4(0, y, 0, 0);
			mColumns[2] = Vec4(0, 0, z, 0);
			mColumns[3] = Vec4(0, 0, 0, 1);
		}
		Matrix4Scale(const Vec3& scale)
			: Matrix4Scale(scale.x, scale.y, scale.z)
		{
		}
	};
	//////////////////////////////////////////////////////////////////////////
	struct Matrix4Translation : Matrix4
	{
		Matrix4Translation(float x, float y, float z)
		{
			mColumns[0] = Vec4(1, 0, 0, 0);
			mColumns[1] = Vec4(0, 1, 0, 0);
			mColumns[2] = Vec4(0, 0, 1, 0);
			mColumns[3] = Vec4(x, y, z, 1);
		}

		Matrix4Translation(const Vec3& translation)
		{
			mColumns[0] = Vec4(1, 0, 0, 0);
			mColumns[1] = Vec4(0, 1, 0, 0);
			mColumns[2] = Vec4(0, 0, 1, 0);
			mColumns[3] = Vec4(translation, 1);
		}
	};
	//////////////////////////////////////////////////////////////////////////
	struct Matrix4RotationX : Matrix4
	{
		Matrix4RotationX(float angleDegree)
		{
			float s, c;
			SinCosDeg(angleDegree, s, c);
			mColumns[0] = Vec4(1, 0, 0, 0);
			mColumns[1] = Vec4(0, c, s, 0);
			mColumns[2] = Vec4(0, -s, c, 0);
			mColumns[3] = Vec4(0, 0, 0, 1);
		}
	};
	//////////////////////////////////////////////////////////////////////////
	struct Matrix4RotationY : Matrix4
	{
		Matrix4RotationY(float angleDegree)
		{
			float s, c;
			SinCosDeg(angleDegree, s, c);
			mColumns[0] = Vec4(c, 0, -s, 0);
			mColumns[1] = Vec4(0, 1, 0, 0);
			mColumns[2] = Vec4(s, 0, c, 0);
			mColumns[3] = Vec4(0, 0, 0, 1);
		}
	};
	//////////////////////////////////////////////////////////////////////////
	struct Matrix4RotationZ : Matrix4
	{
		Matrix4RotationZ(float angleDegree)
		{
			float s, c;
			SinCosDeg(angleDegree, s, c);
			mColumns[0] = Vec4(c, s, 0, 0);
			mColumns[1] = Vec4(-s, c, 0, 0);
			mColumns[2] = Vec4(0, 0, 1, 0);
			mColumns[3] = Vec4(0, 0, 0, 1);
		}
	};
	//////////////////////////////////////////////////////////////////////////
	struct Matrix4PerspectiveFOV : Matrix4
	{
		float ExtractAspectRatio() const
		{
			return mColumns[1].y / mColumns[0].x;
		}
		Matrix4PerspectiveFOV(float filedOfView, float aspectRatio, float zn, float zf)
		{
			/*
			D3DXMatrixPerspectiveFovLH
			xScale     0          0               0
			0        yScale       0               0
			0          0       zf/(zf-zn)         1
			0          0       -zn*zf/(zf-zn)     0
			where:
			yScale = cot(fovY/2)

			xScale = yScale / aspect ratio
			*/


			float    SinFov;
			float    CosFov;
			SinCos(filedOfView * DEG2RAD * 0.5f, SinFov, CosFov);

			float Height = CosFov / SinFov;
			float Width = Height / aspectRatio;
			float fRange = zf / (zf - zn);

#if 0 //transposing maybe
			mColumns[0] = Vec4(Width, 0, 0, 0);
			mColumns[1] = Vec4(0, Height, 0, 0);
			mColumns[2] = Vec4(0, 0, fRange, zn * -fRange);
			mColumns[3] = Vec4(0, 0, 1, 0);
#else
			mColumns[0] = Vec4(Width, 0, 0, 0);
			mColumns[1] = Vec4(0, Height, 0, 0);
			mColumns[2] = Vec4(0, 0, fRange, 1);
			mColumns[3] = Vec4(0, 0, zn * -fRange, 0);
#endif // 
		}
	};
	//////////////////////////////////////////////////////////////////////////
	struct Matrix4Perspective : Matrix4
	{

		Matrix4Perspective(float w, float h, float zn, float zf)
		{
			/*D3DXMatrixPerspectiveLH
			2*zn/w  0       0              0
			0       2*zn/h  0              0
			0       0       zf/(zf-zn)     1
			0       0       zn*zf/(zn-zf)  0

			D3DXMatrixPerspectiveRH
			2*zn/w  0       0              0
			0       2*zn/h  0              0
			0       0       zf/(zn-zf)    -1
			0       0       zn*zf/(zn-zf)  0
			*/

			mColumns[0] = Vec4(2 * zn / w, 0, 0, 0);
			mColumns[1] = Vec4(0, 2 * zn / h, 0, 0);
			mColumns[2] = Vec4(0, 0, zf / (zf - zn), zn * zf / (zn - zf));
			mColumns[3] = Vec4(0, 0, 1, 0);
		}
	};

	//////////////////////////////////////////////////////////////////////////
	struct Matrix4Ortho : Matrix4
	{
		Matrix4Ortho(float width, float height, float zn, float zf)
		{
			float fRange = 1.0f / (zf - zn);

			mColumns[0] = Vec4(2 / width, 0, 0, 0);
			mColumns[1] = Vec4(0, 2 / height, 0, 0);
			mColumns[2] = Vec4(0, 0, fRange, -fRange * zn);
			mColumns[3] = Vec4(0, 0, 0, 1);

		}
		Matrix4Ortho(float left, float right, float top, float bottom, float nearClip, float farClip)
		{
			
		}
	};
	//////////////////////////////////////////////////////////////////////////
	struct Matrix4LookAt : Matrix4
	{
		Matrix4LookAt(const Vec3& position, const Vec3& target, const Vec3& up)
		{

		}
	};
	//////////////////////////////////////////////////////////////////////////
	struct Matrix4RotationQuat : Matrix4
	{
		Matrix4RotationQuat(const Quat& rotation)
		{

		}
	};
	//////////////////////////////////////////////////////////////////////////
	struct UCORE_API Matrix4RotationXYZ : Matrix4
	{
		Matrix4RotationXYZ(const Vec3& xyz);
	};
	//////////////////////////////////////////////////////////////////////////
	struct UCORE_API Matrix4Transformation : Matrix4
	{
		Matrix4Transformation(const Vec3& translation, const Vec3& rotation, const Vec3& scale);
	};
	
	typedef Matrix4 Transform;

	UCORE_API StringStreamOut& operator << (StringStreamOut&, const Matrix4&);
	UCORE_API StringStreamOut& operator << (StringStreamOut&, const Matrix3&);

};