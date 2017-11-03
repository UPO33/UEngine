#pragma once

#include "Base.h"
#include "DescriptorHeap.h"

namespace UGFX
{

	//////////////////////////////////////////////////////////////////////////
	class GFXContextDX12;

	//////////////////////////////////////////////////////////////////////////
	struct UGFXDIRECTX_API TextureDX12 : public GFXTexture
	{
		DescHandleCSU mSRVShaderVisible; // shader visible SRV of texture if any
		DescHandleCSU mSRV;	//non visible shader resource view of texture if any, RTV is created to this one first
		
		//depth and render target view are non-shader visible at all
		DescHandleRTV mRTV;
		DescHandleDSV mDSV;

		TextureDX12(ID3D12Resource* pTexture)
		{
			mNativeHandle = pTexture;
		}

		ID3D12Resource* GetHandle() const { return reinterpret_cast<ID3D12Resource*>(mNativeHandle); }

	};

	//////////////////////////////////////////////////////////////////////////
	class UGFXDIRECTX_API TextureMgrDX12 : public ITextureMgr
	{
	public:
		TextureMgrDX12(GFXContextDX12* ctx);

		GFXContextDX12* mCtx;

		virtual GFXTexture* LoadTextureDefault(TSPtr<SmartMemBlock> textureData, bool bSRGB) override;

		TextureDX12* GetChessTexture();
		TextureDX12* CreateWhiteTexture();
		DescriptorMgrDX12* GetDescMgr() const;
	};



};
