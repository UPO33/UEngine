#pragma once

#include "Asset.h"
#include "../GFXCore/Base.h"

namespace UEngine
{
	//////////////////////////////////////////////////////////////////////////
	class ATexture2D;

	struct AttrMaterialTexture : AttrBase
	{
		unsigned mIndex;
	};
	struct AttMaterialConstant : AttrBase
	{
		const char* mConstantName;
		AttMaterialConstant(const char* constantName) : mConstantName(constantName) {}
	};

	enum class EMaterialDomain
	{
		Debug,
		Opaque,	//opaque meshes, fill all the pixels
		Cutout, //opaque meshes that use hlsl clip() for masking 
		Translucent,
	};

	class AMaterial : public Asset
	{
		UCLASS(AMaterial, Asset)

		EMaterialDomain mDomain;
		bool mWirefame = false;
		bool mTwoFalse = true;

		void ReCompileShaders() {}

		virtual GFXPiplineState* GetDepthOnlyPipline() = 0;
		virtual GFXPiplineState* GetMainPassPipline() = 0;
		virtual GFXPiplineState* GetMainPassWireframePipeline() = 0;
	};


#if 0
	struct MaterialShaders
	{
		GFXShader*	mDepthOnlyVS;

		GFXShader*	mGBufferPassVS;
		GFXShader*	mGBufferPassHS;
		GFXShader*	mGBufferPassDS;
		GFXShader*	mGBufferPassGS;
		GFXShader*	mGBufferPassPS;
	};

	class AMaterialRS
	{
	public:
		
	};

	struct EntSM
	{
		GFXPiplineState* mPSODepthOnly[MAXLOD];
		GFXPiplineState* mPSOMainPass[LOD];

		void SetMateril(AMaterial* material)
		{
			mataerial->GetPSODepthOnly(mesh);
		};
	};
#endif // 
};