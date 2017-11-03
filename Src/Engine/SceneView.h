#pragma once

#include "Base.h"
#include "../Core/Matrix.h"
#include "../Core/Frustum.h"
#include "../Core/Bound.h"

namespace UEngine
{
//////////////////////////////////////////////////////////////////////////
class EntityCamera;

//////////////////////////////////////////////////////////////////////////
enum class EViewRenderMode
{
	Test,
	Wireframe,
	Unlit,
};

//////////////////////////////////////////////////////////////////////////
struct ViewOptions
{
	EViewRenderMode mRenderMode;
	//whether draw bounding box of items or not
	size_t		mDrawBounds : 1;
	size_t		mDrawCanvas : 1;
	//whether draw FPS at to left of viewport
	size_t		mDrawFPS : 1;
	//draw all the elements at this LOD, -1 to disable it
	int mForceLODIndex = -1;
};

//////////////////////////////////////////////////////////////////////////
struct UENGINE_API alignas(64) SceneView
{
	Matrix4					mMatrixProjection;
	Matrix4					mMatrixInvProjection;
	Matrix4					mMatrixView;
	Matrix4					mMatrixInvView;
	Matrix4					mMatrixWorlToClip;	//Projection * View
	Matrix4					mMatrixClipToWorld;
	Frustum					mFrustumWS;
	AABB					mBoundWS;
	ViewOptions				mViewOptions;
	bool					mIsPerspective;
	float					mFiledOfView;
	float					mOrthoSize;
	float					mNearPlane;
	float					mFarPlane;
	unsigned				mRenderLayer;
	EntityCamera*			mCameraEntity;	//the camera this data was generated from, if any
	float					mLODCoefficient;
	
	//returns the world space position of the view
	inline Vec3 GetPosition() const { return mMatrixInvView.GetTranslation(); }
};

//////////////////////////////////////////////////////////////////////////
struct SceneViewGroup
{
	static const unsigned MAX_VIEW = 32;
	
	SceneView		mViews[MAX_VIEW];
	Scene*			mScene = nullptr;
	unsigned		mNumView = 0;
};

//base class for scene renderer
class SceneRendererBase
{
	virtual void Render(SceneViewGroup* views) {}
};


};