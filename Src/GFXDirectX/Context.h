#pragma once

#include "Base.h"
#include "DescriptorHeap.h"
#include "CommandList.h"

namespace UGFX
{
	class TextureMgrDX12;
	class DescriptorMgrDX12;
;

	//////////////////////////////////////////////////////////////////////////
	class UGFXDIRECTX_API GFXContextDX11 : public GFXContextBase
	{
		UCLASS(GFXContextDX11, GFXContextBase)

		ID3D11Device*			mDevice;
		ID3D11DeviceContext*	mImmediateContext;
		D3D_FEATURE_LEVEL		mFeatureLevel;

		virtual void Init()
		{
			UINT flags = 0;
			HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, nullptr, 0, D3D11_SDK_VERSION, &mDevice, &mFeatureLevel, &mImmediateContext);
			if (FAILED(result))
			{
				ULOG_FATAL("D3D11 Device Creation Failed");
			}
		}
		virtual void Release()
		{
			SafeRelease(mImmediateContext);
			SafeRelease(mDevice);
		}
	};


	//////////////////////////////////////////////////////////////////////////
	class UGFXDIRECTX_API GFXContextDX12 : public GFXContextBase
	{
		UCLASS(GFXContextDX12, GFXContextBase)


		ID3D12Device*			mDevice;
		IDXGIFactory4*			mGIFactory;
		ID3D12Fence*			mFance;
		unsigned				mDescHandleIncSize[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
		unsigned				mFrameIndex = 0;
		uint64					mFenceValue[FRAME_COUNT] = {};
		HANDLE					mEvent;
		IShaderMgr*				mShaderMgr = nullptr;
		IPipelineStateMgr*		mPipelineStateMgr = nullptr;
		TextureMgrDX12*			mTextureMgr = nullptr;
		DescriptorMgrDX12*		mDescriptorMgr = nullptr;

		DescHeapPool		mDescHeapAllocator[D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

		QueueMgr* mQueueMgr = nullptr;

		//////////////////////////////////////////////////////////////////////////
		void CreateFrameResources();
		//////////////////////////////////////////////////////////////////////////
		virtual void Init();
		virtual IShaderMgr* GetShaderMgr() override;
		virtual IPipelineStateMgr* GetPipelineStateMgr() override;
		virtual ITextureMgr* GetTextureMgr() override;
		virtual IDescriptorMgr* GetDescriptorMgr() override;

		//IMPL in RootSignature.cpp
		virtual GFXRootSignature* CreateRootSignature(const GFXRootSignatureDesc&) override;
		virtual GFXBuffer* CreateStaticVB(size_t size, const void* initialData, const char* debugStr /* = nullptr */) override;
		virtual GFXBuffer* CreateStaticIB(size_t size, const void* initialData, const char* debugStr /* = nullptr */) override;


		//////////////////////////////////////////////////////////////////////////
		virtual void Release();

		virtual const char* GetAPIName() const override
		{
			return "DX12";
		}
		// Helper function for acquiring the first available hardware adapter that supports Direct3D 12
		IDXGIAdapter3* GetHardwareAdapter(IDXGIFactory4* pFactory, D3D_FEATURE_LEVEL featureLevel) const;

	};

	inline GFXContextDX12* GetGFXContextDX12()
	{
		return ((GFXContextDX12*)GetGFXContext());
	}
};