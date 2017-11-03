#include "Base.h"

void WindowWin32::CreateSwapChain()
{
	D3D12_DESCRIPTOR_HEAP_DESC dhDesc = {};
	dhDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dhDesc.NodeMask = 0;
	dhDesc.NumDescriptors = FRAME_COUNT;
	dhDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	UCHECK_DXRET(gGFX->mDevice->CreateDescriptorHeap(&dhDesc, IID_PPV_ARGS(&mRTDescHeap)));
	
	
	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Width = mWidth;
	scDesc.Height = mHeight;
	scDesc.BufferCount = FRAME_COUNT;
	scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	scDesc.Scaling = DXGI_SCALING_STRETCH;
	scDesc.Stereo = false;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	scDesc.Flags = 0;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;

	UASSERT(gGFX->mGFXQueue);
	UASSERT(mWndHandle);
	IDXGISwapChain1* swapChain = nullptr;
	UCHECK_DXRET(gGFX->mGIFactory->CreateSwapChainForHwnd(gGFX->mGFXQueue, mWndHandle, &scDesc, nullptr, nullptr, &swapChain));
	UCHECK_DXRET(swapChain->QueryInterface(IID_PPV_ARGS(&mSwapChain)));
	ULOG_SUCCESS("swap chain created");

	UCHECK_DXRET(gGFX->mGIFactory->MakeWindowAssociation(mWndHandle, DXGI_MWA_NO_ALT_ENTER));

	for (UINT iBuffer = 0; iBuffer < FRAME_COUNT; iBuffer++)
	{
		ID3D12Resource* renderTarget = nullptr;
		UCHECK_DXRET(mSwapChain->GetBuffer(iBuffer, IID_PPV_ARGS(&renderTarget)));

		mRTTextures[iBuffer] = renderTarget;
		
		size_t rtHandle = mRTDescHeap->GetCPUDescriptorHandleForHeapStart().ptr + iBuffer * gGFX->mDescHandleIncSize[D3D12_DESCRIPTOR_HEAP_TYPE_RTV];
		gGFX->mDevice->CreateRenderTargetView(renderTarget, nullptr, D3D12_CPU_DESCRIPTOR_HANDLE { rtHandle });
		renderTarget->Release();
	}
}
