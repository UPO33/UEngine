#pragma once

#include "Asset.h"

namespace UEngine
{
	//////////////////////////////////////////////////////////////////////////
	class GFXTexture2D;
	class GFXSamplerState;

	class ATexture2DRS
	{
		GFXTexture2D*		mTexture = nullptr;
		GFXSamplerState*	mSampler = nullptr;
	};

	class UENGINE_API ATextureBase : public Asset
	{

	};

	class UENGINE_API ATexture2D : public ATextureBase
	{
		
	};

	class UENGINE_API ARenderTarget2D : public ATextureBase
	{

	};
};