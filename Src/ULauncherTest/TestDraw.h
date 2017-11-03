#pragma once

#include "Base.h"
#include "GameInput.h"

static constexpr bool gWireFrame = false;

struct SimpleVertex
{
	float	mXY[2];
	float	mColorRGB[3];
};


struct PerDrawDescHandle
{
	DescHandleBase mCSU;
	DescHandleBase mSampler;
};

//////////////////////////////////////////////////////////////////////////
struct TestScreenQuadGen
{
	struct PointData
	{
		Vec2 mPosition;
		Vec2 mSize;
	};
	Name					mFileName = "ScreenQuadGen.hlsl";
	GFXRootSignature*		mRS = nullptr;
	ID3D12Resource*			mVB = nullptr;	
	GFXInputLayout*			mInputLayout = nullptr;
	GFXPiplineState*		mPSO = nullptr;
	TArray<PointData>		mPoints;
	DescHandleCSU			mDescHandleBase;

	Matrix4 mMatrix;
	Vec3 mOffset = Vec3(0.0f);

	static const unsigned NumItem = 8192;

	struct SBufferColor
	{
		static const unsigned NUM_COLOR = 1024;
		Color	mColors[NUM_COLOR];

		SBufferColor()
		{
			for (Color& c : mColors)
			{
				c = Color(RandFloat01(), RandFloat01(), RandFloat01(), RandFloat01());
			}
		}
	};

	DescHandleCSU mBaseCBufferHandle;
	ID3D12Resource* mColorBuffer0 = nullptr;
	ID3D12Resource* mColorBuffer1 = nullptr;
	ID3D12DescriptorHeap* mDescHeapSRV = nullptr;
	
	GFXTexture* mTextureFire = nullptr;
	GFXTexture* mTextureSpark = nullptr;
	GFXTexture* mTextureSmoke = nullptr;

	void GenRndPoints();

	void CreateResources();

	void Render(CmdList* cmdList);
};


