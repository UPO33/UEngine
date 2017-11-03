#pragma once

#include "Plane.h"
#include "Matrix.h"

namespace UCore
{
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
struct UCORE_API LineSegment
{
	Vec3 mPointA, mPointB;

	LineSegment(){}
	LineSegment(const Vec3& a, const Vec3& b) : mPointA(a), mPointB(b){}
};

//////////////////////////////////////////////////////////////////////////
enum class EBoxFace
{
	PositiveX,
	PositiveY,
	PositiveZ,
	NegativeX,
	NegativeY,
	NegativeZ
};

//////////////////////////////////////////////////////////////////////////
//axis aligned bounding box
struct UCORE_API alignas(16) AABB
{
	Vec3 mMin, mMax;
		
	AABB() {}
	AABB(InitZero) { mMin = mMax = Vec3(0.0f); }
	//useful for initializing the bound before a loop of calls to Enclose() which incrementally expands the bound to enclose the given objects
	AABB(InitDefault) : 
		mMin(+std::numeric_limits<float>::infinity()), 
		mMax(-std::numeric_limits<float>::infinity())
	{}

	explicit AABB(const Vec3& min, const Vec3& max) : mMin(min), mMax(max) {}

	static AABB MakeFromPoints(const void* points, unsigned numPoints, unsigned stride);
	
	static constexpr unsigned NumFaces() { return 6; }
	static constexpr unsigned NumVertices() { return 8; }
	static constexpr unsigned NumEdges() { return 12; }

	inline void SetNegativeInfinity()
	{
		mMin = +std::numeric_limits<float>::infinity();
		mMax = -std::numeric_limits<float>::infinity();
	}
	inline Vec3 GetMin() const { return mMin; }
	inline Vec3 GetMax() const { return mMax; }
	inline Vec3 GetCenter() const { return (mMin + mMax) * 0.5f; }
	inline Vec3 GetExtent() const { return (mMax - mMin) * 0.5f; }
	

// 	inline bool IsPointInside(const Vec3& point) const
// 	{
// 		return point > mMin && point < mMax;
// 	}

	//0 == box is currently intersecting the plane
	//1 == box is completely on positive side of plane
	//2 == box is completely on negative side of plane
	int Intersect(const Plane&) const;
	bool Intersects(const AABB& other) const;
	Vec3 GetCorner(bool positiveX, bool positiveY, bool positiveZ) const;

	LineSegment GetEdge(unsigned edgeIndex) const;
	
	Vec3 GetFaceNormal(unsigned axis, bool positive);
	Vec3 GetFaceCenter(unsigned axis, bool positive);
	Vec3 GetFacePoint(unsigned axis, bool positive, float u, float v) const;

	Vec3 GetFaceNormal(EBoxFace face) const;
	Vec3 GetFaceCenter(EBoxFace face) const;
	Vec3 GetFacePoint(EBoxFace face, float u, float v) const;
	
	bool Contains(const Vec3& point) const;
	bool Contains(const AABB& box) const;

	//transform a bounding box using a affine matrix, doesn't use the projection part of matrix
	static AABB TransformAffine(const AABB& box, const Matrix4& transform);

	void Translate(const Vec3& v)
	{
		mMin += v;
		mMax += v;
	}

	//expand to enclose the object
	void Enclose(const Vec3& position)
	{
		mMin = Min(mMin, position);
		mMax = Max(mMax, position);
	}
	void Enclose(const Vec3& vMin, const Vec3& vMax)
	{
		mMin = Min(mMin, vMin);
		mMax = Max(mMax, vMax);
	}
	void Enclose(const AABB& aabb)
	{
		Enclose(aabb.mMin, aabb.mMax);
	}
	void EncloseLineSegment(const Vec3& a, const Vec3& b)
	{
		Enclose(Min(a, b), Max(a, b));
	}
	void EncloseShpere(const Vec3& center, float radius)
	{
		Enclose(center - radius, center + radius);
	}
		
		
};
UCORE_API StringStreamOut& operator <<(StringStreamOut&, const AABB&);
	
inline AABB operator * (const AABB& a, float scale)
{
	return AABB(a.mMin * scale, a.mMax * scale);
}
//////////////////////////////////////////////////////////////////////////
struct UCORE_API BoundingBox
{
	Vec3 mMin;
	Vec3 mMax;
	bool mIsValid;

	BoundingBox() : mIsValid(false) {}
	BoundingBox(const Vec3& min, const Vec3& max) : mMin(min), mMax(max), mIsValid(true) {}

	inline Vec3 GetMin() const { return mMin; }
	inline Vec3 GetMax() const { return mMax; }
	inline Vec3 GetCenter() const { return (mMin + mMax) * 0.5f; }
	inline Vec3 GetExtent() const { return (mMax - mMin) * 0.5f; }
	inline bool IsValid() const { return mIsValid; }

	static BoundingBox MakeFromPoints(const void* points, unsigned numPoints, unsigned stride);
	static BoundingBox MakeFromPoints(const Vec3* points, unsigned numPoints);

	static constexpr unsigned NumFaces() { return 6; }
	static constexpr unsigned NumVertices() { return 8; }
	static constexpr unsigned NumEdges() { return 12; }

	void Enclose(const Vec3& point)
	{
		if (mIsValid)
		{
			mMin = Min(mMin, point);
			mMax = Max(mMax, point);
		}
		else
		{
			mMin = mMax = point;
			mIsValid = true;
		}
	}
	void Enclose(const Vec3& vMin, const Vec3& vMax)
	{
		if (mIsValid)
		{
			mMin = Min(mMin, vMin);
			mMax = Max(mMax, vMax);
		}
		else
		{
			mMin = vMin;
			mMax = vMax;
			mIsValid = true;
		}
	}
	void Enclose(const BoundingBox& box)
	{
		if (mIsValid && box.mIsValid)
		{
			mMin = Min(mMin, box.mMin);
			mMax = Max(mMax, box.mMax);
		}
		else if (box.mIsValid)
		{
			*this = box;
		}
	}
	void Enclose(const AABB& aabb)
	{
		Enclose(aabb.mMin, aabb.mMax);
	}
	void EncloseSphere(const Vec3& center, float radius)
	{
		Enclose(center - radius, center + radius);
	}

	Vec3 GetCorner(bool positiveX, bool positiveY, bool positiveZ) const
	{
		return ((const AABB*)this)->GetCorner(positiveX, positiveY, positiveZ);
	}
};

UCORE_API StringStreamOut& operator <<(StringStreamOut&, const BoundingBox&);
	
};