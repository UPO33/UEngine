#include "Bound.h"

namespace UCore
{

template<typename T> T& ElemAtStride(const void* array, unsigned index, unsigned stride)
{
	return *((T*)(((char*)array) + index * stride));
}
AABB AABB::MakeFromPoints(const void* points, unsigned numPoints, unsigned stride)
{
	UASSERT(points && stride > 0 && numPoints >= 2);

	Vec3 vMin = Min(ElemAtStride<Vec3>(points, 0, stride), ElemAtStride<Vec3>(points, 1, stride));
	Vec3 vMax = Max(ElemAtStride<Vec3>(points, 0, stride), ElemAtStride<Vec3>(points, 1, stride));

	for (unsigned i = 2; i < numPoints; i++)
	{
		vMin = Min(vMin, ElemAtStride<Vec3>(points, i, stride));
		vMax = Max(vMax, ElemAtStride<Vec3>(points, i, stride));
	}

	return AABB(vMin, vMax);
}

int AABB::Intersect(const Plane& plane) const
{
	Vec3 vec1, vec2;

	for (unsigned i = 0; i < 3; i++)
	{
		if (plane[i] >= 0)
		{
			vec1[i] = mMin[i];
			vec2[i] = mMax[i];
		}
		else
		{
			vec1[i] = mMax[i];
			vec2[i] = mMax[i];
		}
	}

	if (plane.DotCoord(vec2) > 0)
	{
		return 1;
	}
	if (plane.DotCoord(vec1) > 0)
	{
		return 2;
	}
	return 0;
}

bool AABB::Intersects(const AABB& other) const
{
	UASSERT(false);
	return false;
	//return mMin < other.mMax && other.mMin < mMax;
}

//////////////////////////////////////////////////////////////////////////
Vec3 AABB::GetCorner(bool positiveX, bool positiveY, bool positiveZ) const
{
	Vec3 ret;
	ret.x = positiveX ? mMax.x : mMin.x;
	ret.y = positiveY ? mMax.y : mMin.y;
	ret.z = positiveZ ? mMax.z : mMin.z;
	return ret;
}

//////////////////////////////////////////////////////////////////////////
LineSegment AABB::GetEdge(unsigned edgeIndex) const
{
	UASSERT(edgeIndex < NumEdges());

	switch (edgeIndex)
	{
	case 0: return LineSegment(mMin, Vec3(mMin.x, mMin.y, mMax.z));
	case 1: return LineSegment(mMin, Vec3(mMin.x, mMax.y, mMin.z));
	case 2: return LineSegment(mMin, Vec3(mMax.x, mMin.y, mMin.z));
	case 3: return LineSegment(Vec3(mMin.x, mMin.y, mMax.z), Vec3(mMin.x, mMax.y, mMax.z));
	case 4: return LineSegment(Vec3(mMin.x, mMin.y, mMax.z), Vec3(mMax.x, mMin.y, mMax.z));
	case 5: return LineSegment(Vec3(mMin.x, mMax.y, mMin.z), Vec3(mMin.x, mMax.y, mMax.z));
	case 6: return LineSegment(Vec3(mMin.x, mMax.y, mMin.z), Vec3(mMax.x, mMax.y, mMin.z));
	case 7: return LineSegment(Vec3(mMin.x, mMax.y, mMax.z), mMax);
	case 8: return LineSegment(Vec3(mMax.x, mMin.y, mMin.z), Vec3(mMax.x, mMin.y, mMax.z));
	case 9: return LineSegment(Vec3(mMax.x, mMin.y, mMin.z), Vec3(mMax.x, mMax.y, mMin.z));
	case 10: return LineSegment(Vec3(mMax.x, mMin.y, mMax.z), mMax);
	case 11: return LineSegment(Vec3(mMax.x, mMax.y, mMin.z), mMax);
}
	return LineSegment(Vec3(0.0f), Vec3(0.0f));
}

Vec3 AABB::GetFaceNormal(EBoxFace face) const
{
	static const Vec3 LUT[] = {
		Vec3(1,0,0),
		Vec3(0,1,0),
		Vec3(0,0,1),
		Vec3(-1,0,0),
		Vec3(0,-1,0),
		Vec3(0,0,-1)
	};

	return LUT[(unsigned)face];
}

UCore::Vec3 AABB::GetFaceNormal(unsigned axis, bool positive)
{
	UASSERT(axis < 3);
	Vec3 normal = Vec3(0.0f);
	normal[axis] = positive ? +1 : -1;
	return normal;
}
Vec3 AABB::GetFaceCenter(unsigned axis, bool positive)
{
	UASSERT(axis < 4);
	Vec3 center = GetCenter();
	center[axis] = positive ? mMax[axis] : mMin[axis];
	return center;
}

Vec3 AABB::GetFaceCenter(EBoxFace face) const
{
	Vec3 center = GetCenter();

	switch (face)
	{
	case EBoxFace::PositiveX: center[0] = mMax[0]; break;
	case EBoxFace::PositiveY: center[1] = mMax[1]; break;
	case EBoxFace::PositiveZ: center[2] = mMax[2]; break;

	case EBoxFace::NegativeX: center[0] = mMin[0]; break;
	case EBoxFace::NegativeY: center[1] = mMin[1]; break;
	case EBoxFace::NegativeZ: center[2] = mMin[2]; break;

	default: UASSERT(false);
	};
	
	return center;
}

Vec3 AABB::GetFacePoint(unsigned axis, bool positive, float u, float v) const
{
	return GetFacePoint(EBoxFace(positive ? axis : axis + 3), u, v);
}

Vec3 AABB::GetFacePoint(EBoxFace face, float u, float v) const
{
	UASSERT(0 <= u <= 1.0f);
	UASSERT(0 <= v <= 1.0f);

	Vec3 d = mMax - mMin;

	switch (face)
	{
	case EBoxFace::PositiveX: return Vec3(mMax.x, mMin.y + u * d.y, mMin.z + v * d.z); //+x
	case EBoxFace::PositiveY: return Vec3(mMin.x + u * d.x, mMax.y, mMin.z + v * d.z); //+y
	case EBoxFace::PositiveZ: return Vec3(mMin.x + u * d.x, mMin.y + v * d.y, mMax.z); //+z

	case EBoxFace::NegativeX: return Vec3(mMin.x, mMin.y + u * d.y, mMin.z + v * d.z); //-x
	case EBoxFace::NegativeY: return Vec3(mMin.x + u * d.x, mMin.y, mMin.z + v * d.z); //-y
	case EBoxFace::NegativeZ: return Vec3(mMin.x + u * d.x, mMin.y + v * d.y, mMin.z); //-z
	}

	UASSERT(false);
	return Vec3(0.0f);
}

bool AABB::Contains(const Vec3& point) const
{
	UASSERT(false);
	return false;
}

bool AABB::Contains(const AABB& box) const
{
	UASSERT(false);
	return false;
}

AABB AABB::TransformAffine(const AABB& box, const Matrix4& transform)
{
#if 0
	auto xa = transform.mColumns[0] * box.mMin.GetXXX();
	auto xb = transform.mColumns[0] * box.mMax.GetXXX();

	auto ya = transform.mColumns[1] * box.mMin.GetYYY();
	auto yb = transform.mColumns[1] * box.mMax.GetYYY();

	auto za = transform.mColumns[2] * box.mMin.GetZZZ();
	auto zb = transform.mColumns[2] * box.mMax.GetZZZ();

	auto vMin = Min(xa, xb) + Min(ya, yb) + Min(za, zb) + transform.mColumns[3];
	auto vMax = Max(xa, xb) + Max(ya, yb) + Max(za, zb) + transform.mColumns[3];
	return AABB(vMin, vMax);
#else
	return AABB();
#endif // 
}

//////////////////////////////////////////////////////////////////////////
StringStreamOut& operator<<(StringStreamOut& stream, const AABB& bound)
{
	stream << "Min:";
	stream << bound.mMin;
	stream << " Max:";
	stream << bound.mMax;
		
	return stream;
}
//////////////////////////////////////////////////////////////////////////
BoundingBox BoundingBox::MakeFromPoints(const void* points, unsigned numPoints, unsigned stride)
{
	UASSERT(points && stride > 0);

	if (numPoints == 0) return BoundingBox();

	Vec3 vMin = ElemAtStride<Vec3>(points, 0, stride);
	Vec3 vMax = ElemAtStride<Vec3>(points, 0, stride);

	for (unsigned i = 1; i < numPoints; i++)
	{
		vMin = Min(vMin, ElemAtStride<Vec3>(points, i, stride));
		vMax = Max(vMax, ElemAtStride<Vec3>(points, i, stride));
	}

	return BoundingBox(vMin, vMax);
}
//////////////////////////////////////////////////////////////////////////
BoundingBox BoundingBox::MakeFromPoints(const Vec3* points, unsigned numPoints)
{
	return MakeFromPoints(points, numPoints, sizeof(Vec3));
}
//////////////////////////////////////////////////////////////////////////
StringStreamOut& operator<<(StringStreamOut& stream, const BoundingBox& bound)
{
	if(bound.IsValid())
	{
		stream << "Min:";
		stream << bound.mMin;
		stream << " Max:";
		stream << bound.mMax;
	}
	else
	{
		stream << "INVALID";
	}

	return stream;
}



};