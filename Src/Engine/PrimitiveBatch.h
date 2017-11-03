#pragma once

#include "Base.h"
#include "../Core/Vector.h"
#include "../Core/Array.h"
#include "../Core/Queue.h"
#include "../Core/Bound.h"
#include "../Core/Matrix.h"

namespace UEngine
{
//////////////////////////////////////////////////////////////////////////
class Scene;

//////////////////////////////////////////////////////////////////////////
class HitetionBase;

struct PBLineItem
{
	float		mPositionA[3];
	float		mPositionB[3];
	Color32		mColor;

	PBLineItem(const Vec3& a, const Vec3& b, Color32 color)
	{
		a.StoreTo(mPositionA);
		b.StoreTo(mPositionB);
		mColor = color;
	}
};
struct PBBillboardItem
{
	Vec3		mCenterPosition;
	float		mWidth;
	float		mHeight;
	Color32		mColor;
};

//////////////////////////////////////////////////////////////////////////
class PrimitiveBatch
{
		
public:
	virtual void SetHit(HitetionBase*){}
	virtual void DrawPoint(const Vec3& position, float size = 1){}
	virtual void DrawBillboard(const Vec3& center, float width, float height, Color32 color){}
	virtual void WireDisc(const Vec3& center, const Vec3& right, const Vec3& forward, Color32 color){}
	
	using LineContainerT = TArray<PBLineItem>;

	TRWBuffer<LineContainerT> mLines;

	void Swap()
	{
		mLines.Swap();
	}
	LineContainerT& GetLinesContainerForWriting() 
	{
		return mLines.GetWrite();
	}
	//////////////////////////////////////////////////////////////////////////
	void DrawLine(const Vec3& a, const Vec3& b, const Color32 color)
	{
		auto lines = GetLinesContainerForWriting();
		lines.Add(a, b, color);
	}

	//////////////////////////////////////////////////////////////////////////
	void DrawWireBox(const AABB& box, Color32 color)
	{
		auto& lines = GetLinesContainerForWriting();
		auto start = lines.AddUnInit(box.NumEdges());
		for (int iEdge = 0; iEdge < box.NumEdges(); iEdge++)
		{
			auto edge = box.GetEdge(iEdge);
			edge.mPointA.StoreTo(lines[start + iEdge].mPositionA);
			edge.mPointB.StoreTo(lines[start + iEdge].mPositionB);
			lines[start + iEdge].mColor = color;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void WireDisc(const Vec3& center, const Vec3& xAxis, const Vec3& yAxis, Color32 color, unsigned numVertex = 32)
	{
		numVertex = Max(numVertex, 4U);
		float radStep = 360 * DEG2RAD / numVertex;
		
		float fs, fc;
		SinCos(0, fs, fc);
		Vec3 prePos = center + xAxis * fs + yAxis * fc;
		
		//draw lines
		for (unsigned iLine = 0; iLine < numVertex; iLine++)
		{
			SinCos(radStep * ((iLine + 1) % numVertex), fs, fc);
			Vec3 point = center + xAxis * fs + yAxis * fc;
			DrawLine(prePos, point, color);
			prePos = point;
		}
	}
};

//primitive batch special for Scene
class PrimitiveBatchScene : public PrimitiveBatch
{
public:
	Scene* mScene;

	PrimitiveBatchScene(Scene* pScene) : mScene(pScene) {}
};
	
};