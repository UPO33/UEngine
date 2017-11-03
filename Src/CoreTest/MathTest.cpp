#include <d3d12.h>
#include <DirectXMath.h>

#include "../Core/Vector.h"
#include "../Core/Matrix.h"



using namespace UCore;


#define LOGVAR(Var) ULOG_WARN(#Var"\n%", Var)



void MathTest()
{

	return;

	{
		{
			Vec3 zeroVec = Vec3(-0.0f);
			UASSERT(zeroVec.IsNearlyZero());
			UASSERT(!zeroVec.IsNormalized());
			Vec3 n = zeroVec.GetNormalizedSafe();
			UASSERT(n.IsNearlyZero());
			Vec3 divZer = Vec3(2.22f) / Vec3(0.0f);
			UASSERT(!divZer.IsFinite());
			
		}

		{
			Vec3 v0 = Vec3(1, 2, 3);
			UASSERT(v0.GetXXX().IsNearlyEqual(Vec3(1)));
			UASSERT(v0.GetYYY().IsNearlyEqual(Vec3(2)));
			UASSERT(v0.GetZZZ().IsNearlyEqual(Vec3(3)));
		}

		{
			Vec3 x = Vec3(10, 0, 0);
			Vec3 y = Vec3(0, 10, 0);
			UASSERT(Cross(x, y).GetNormalized().IsNearlyEqual(Vec3(0, 0, 1)));
		}
	}

	{
		Matrix4 matTranslation = Matrix4Translation(Vec3(1, 2, 3));
		UASSERT(matTranslation.GetTranslation().IsNearlyEqual(Vec3(1, 2, 3)));
		Matrix4 matScale = Matrix4Scale(3);
		UASSERT(matScale.ExtractScale().IsNearlyEqual(Vec3(3.0)));
		matScale.RemoveScaling();
		UASSERT(matScale.ExtractScale().IsNearlyEqual(Vec3(1.0f)));

		{
			//rotating along y axis is clockwise
			Matrix4 mrx = Matrix4RotationY(90);
			UASSERT(mrx.mColumns[0].IsNearlyEqual(Vec4(0, 0, -1, 0)));
		}
		{
			//rotating along  x axis clock wise
			Matrix4 mry = Matrix4RotationX(90);
			UASSERT(mry.mColumns[2].IsNearlyEqual(Vec4(0, -1, 0, 0)));
		}
		{
			//rotating along z axis clockwise
			Matrix4 mrz = Matrix4RotationZ(180);
			UASSERT(mrz.mColumns[0].IsNearlyEqual(Vec4(-1, 0, 0, 0)));
			UASSERT(mrz.mColumns[1].IsNearlyEqual(Vec4(0, -1, 0, 0)));
		}
		{
			Matrix4 mr = Matrix4::IDENTITY;
			for (size_t i = 0; i < 90; i++)
				mr = mr * Matrix4RotationY(1);
		}

		{
			Matrix4 transform = Matrix4Translation(Vec3(10, 0, 0)) * (Matrix4RotationY(45) * Matrix4RotationY(45))  * Matrix4Scale(2);
			UASSERT(transform.IsUniformScale());
			Vec3 transformedPoint = transform.TransformPoint(Vec3(1, 0, 0));
			UASSERT(transformedPoint.IsNearlyEqual(Vec3(10, 0, -2)));
			transform.InvertAffine();
			transform.Invert();
			transform.InvertAffine();
			UASSERT(transform.TransformPoint(transformedPoint).IsNearlyEqual(Vec3(1, 0, 0)));
			
		}
		
		{
			//////////////////////////////////////////////////////////////////////////
			Matrix4 transform = Matrix4Translation(Vec3(0, 0, 101));
// 			Vec4 v0 = transform.TransformVec4(Vec4(1, 0, 0, 1));
// 			Vec4 v1 = transform.GetTranspose().TransformVec4(Vec4(1, 0, 0, 1));
			Matrix4 view = Matrix4::IDENTITY;

			ULOG_WARN("---------------------");
			Matrix4 projection = Matrix4PerspectiveFOV(45, 1, 10, 100);
			Matrix4 mvp = projection * view * transform;
			//Matrix4 mvp = transform * view * projection;

			Vec4 transformedPoint = mvp.TransformVec4(Vec4(0, 0, 0, 1));
			LOGVAR(transformedPoint);

			LOGVAR(projection);
			Vec3 ndc = Vec3(transformedPoint) / transformedPoint.w;
			LOGVAR(ndc);
			ULOG_WARN("");
			//////////////////////////////////////////////////////////////////////////
		}
		{
			ULOG_WARN("---------------------");
			Matrix4 projection = Matrix4PerspectiveFOV(45, 1, 1, 101);
			LOGVAR(projection);
			Vec4 transformedPoint = projection.TransformVec4(Vec4(0, 0, 10, 1));
			LOGVAR(transformedPoint);
			Vec3 ndc = Vec3(transformedPoint) / transformedPoint.w;
			LOGVAR(ndc);
			

			
		}
		{
			ULOG_WARN("---------------");
			Matrix4 projection;
			DirectX::XMMATRIX m = DirectX::XMMatrixPerspectiveFovLH(45 * DEG2RAD, 1, 3, 100);
			DirectX::XMFLOAT4 pos = {0, 0, 10, 1};
			Vec4 transformedPoint;
			DirectX::XMStoreFloat4((DirectX::XMFLOAT4*) &transformedPoint, DirectX::XMVector4Transform(DirectX::XMLoadFloat4(&pos), DirectX::XMMatrixTranspose(m)));

			//when projected w is z distance from near clip to point 
			auto mt = DirectX::XMMatrixTranspose(m);
			MemCopy(&projection, &mt, sizeof(Matrix4));
			LOGVAR(projection);

			LOGVAR(transformedPoint);
			Vec3 ndc = Vec3(transformedPoint) / transformedPoint.w;
			LOGVAR(ndc);
		}
	};

};

