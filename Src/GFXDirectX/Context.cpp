#include "Context.h"
#include "../Core/Meta.h"

#include "ShaderMgr.h"
#include "PipelineStateMgr.h"
#include "TextureMgr.h"

namespace UGFX
{
	UCLASS_BEGIN_IMPL(GFXContextDX12)
		UCLASS_END_IMPL(GFXContextDX12)
};


namespace UGFX
{

void GFXContextDX12::CreateFrameResources()
{

}


void GFXContextDX12::Init()
{
	UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
	//Enable the debug layer (requires the Graphics Tools "optional feature").
	//#NOTE: Enabling the debug layer after device creation will invalidate the active device.
	{
		ID3D12Debug* debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			// Enable additional debug layers.
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	UHRFATAL(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&mGIFactory)));
	ULOG_SUCCESS("DXGIFactory4 created");

	UHRFATAL(D3D12CreateDevice(nullptr, featureLevel, IID_PPV_ARGS(&mDevice)));
	ULOG_SUCCESS("DX12 device created");

	//set size of descriptor handles
	for (unsigned i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; i++)
		mDescHandleIncSize[i] = mDevice->GetDescriptorHandleIncrementSize((D3D12_DESCRIPTOR_HEAP_TYPE)i);

	mQueueMgr = new QueueMgr();
	mQueueMgr->Initilize(this);

	mShaderMgr = new ShaderMgrDX(this);
	mPipelineStateMgr = new PipelineStateMgrDX12(this);
	mTextureMgr = new TextureMgrDX12(this);
	mDescriptorMgr = new DescriptorMgrDX12(this);
}

IShaderMgr* GFXContextDX12::GetShaderMgr()
{
	return mShaderMgr;
}

IPipelineStateMgr* GFXContextDX12::GetPipelineStateMgr()
{
	return mPipelineStateMgr;
}




ITextureMgr* GFXContextDX12::GetTextureMgr()
{
	return mTextureMgr;
}

IDescriptorMgr* GFXContextDX12::GetDescriptorMgr()
{
	return mDescriptorMgr;
}

void GFXContextDX12::Release()
{
	SafeDelete(mPipelineStateMgr);
	SafeDelete(mShaderMgr);
	SafeDelete(mQueueMgr);
	
	mDevice->Release();
	mGIFactory->Release();
}

IDXGIAdapter3* GFXContextDX12::GetHardwareAdapter(IDXGIFactory4* pFactory, D3D_FEATURE_LEVEL featureLevel) const
{
	IDXGIAdapter3* pAdapter = nullptr;

	for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, (IDXGIAdapter1**)&pAdapter); ++adapterIndex)
	{
		DXGI_ADAPTER_DESC2 desc;
		pAdapter->GetDesc2(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			// Don't select the Basic Render Driver adapter.
			continue;
		}

		// Check to see if the adapter supports Direct3D 12, but don't create the
		// actual device yet.
		if (SUCCEEDED(D3D12CreateDevice(pAdapter, featureLevel, __uuidof(ID3D12Device), nullptr)))
		{
			return pAdapter;
		}
	}
}
}