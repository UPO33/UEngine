#pragma once

#include "Base.h"

#if 0
inline ID3D12Resource* UCreateRenderTarget2D(DXGI_FORMAT format, unsigned w, unsigned h)
{
	gGFX->mDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(format, w, h));
};

struct DescHandle
{

};
struct TestRenderer
{
	ID3D12Resource* mDepthBuffer = nullptr;
	ID3D12Resource* mSceneColor = nullptr;

	TestRenderer(unsigned w, unsigned h)
	{
		mDepthBuffer = UCreateRenderTarget2D(DXGI_FORMAT_D32_FLOAT, w, h);
		gGFX->mDevice->CreateDepthStencilView(mDepthBuffer, nullptr, dsvHandle);

		//alpha is unused
		mSceneColor = UCreateRenderTarget2D(DXGI_FORMAT_R16G16B16A16_FLOAT, w, h);
		gGFX->mDevice->CreateRenderTargetView(mSceneColor, nullptr, rtvHandle);
		gGFX->mDevice->CreateShaderResourceView(mSceneColor, nullptr, srvHandle);
		
		
	}
};
#endif // _DEBUG