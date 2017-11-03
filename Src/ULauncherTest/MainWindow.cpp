#include "MainWindow.h"

WindowWin32::WindowWin32(unsigned w, unsigned h, const wchar_t* wndClass) : mWidth(w), mHeight(h)
{
	auto hInstance = GetModuleHandle(nullptr);

	// Initialize the window class.
	WNDCLASSEX windowClass = { 0 };
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = UWindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = wndClass;
	auto result = RegisterClassEx(&windowClass);
	UASSERT(result);

	RECT windowRect = { 0, 0, static_cast<LONG>(mWidth), static_cast<LONG>(mHeight) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	// Create the window and store a handle to it.
	mWndHandle = CreateWindow(
		windowClass.lpszClassName,
		TEXT("Tittle"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,		// We have no parent window.
		nullptr,		// We aren't using menus.
		hInstance,
		this);

	ShowWindow(mWndHandle, SW_NORMAL);
	UpdateWindow(mWndHandle);
}

void WindowWin32::CreateSwapChain()
{
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

	UASSERT(mWndHandle);
	IDXGISwapChain1* swapChain = nullptr;
	UCHECK_DXRET(gGFX->mGIFactory->CreateSwapChainForHwnd(gGFX->mQueueMgr->GetGraphicQueue().GetCommandQueue(), mWndHandle, &scDesc, nullptr, nullptr, &swapChain));
	UCHECK_DXRET(swapChain->QueryInterface(IID_PPV_ARGS(&mSwapChain)));

	ULOG_SUCCESS("");

	UCHECK_DXRET(gGFX->mGIFactory->MakeWindowAssociation(mWndHandle, DXGI_MWA_NO_ALT_ENTER));

	mRTVHead = gGFX->mDescriptorMgr->AllocRTV(FRAME_COUNT);

	for (UINT iBuffer = 0; iBuffer < FRAME_COUNT; iBuffer++)
	{
		ID3D12Resource* renderTarget = nullptr;
		UCHECK_DXRET(mSwapChain->GetBuffer(iBuffer, IID_PPV_ARGS(&renderTarget)));

		mRTTextures[iBuffer] = renderTarget;

		
		//size_t rtHandle = mRTDescHeap->GetCPUDescriptorHandleForHeapStart().ptr + iBuffer * gGFX->mDescHandleSize[D3D12_DESCRIPTOR_HEAP_TYPE_RTV];
		gGFX->mDevice->CreateRenderTargetView(renderTarget, nullptr, (mRTVHead + iBuffer).GetCpuHandle() );
		renderTarget->Release();
	}
#if 0	

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
	
	ULOG_SUCCESS("");

	UCHECK_DXRET(gGFX->mGIFactory->MakeWindowAssociation(mWndHandle, DXGI_MWA_NO_ALT_ENTER));

	for (UINT iBuffer = 0; iBuffer < FRAME_COUNT; iBuffer++)
	{
		ID3D12Resource* renderTarget = nullptr;
		UCHECK_DXRET(mSwapChain->GetBuffer(iBuffer, IID_PPV_ARGS(&renderTarget)));

		mRTTextures[iBuffer] = renderTarget;

		size_t rtHandle = mRTDescHeap->GetCPUDescriptorHandleForHeapStart().ptr + iBuffer * gGFX->mDescHandleSize[D3D12_DESCRIPTOR_HEAP_TYPE_RTV];
		gGFX->mDevice->CreateRenderTargetView(renderTarget, nullptr, D3D12_CPU_DESCRIPTOR_HANDLE{ rtHandle });
		renderTarget->Release();
	}
#endif // 0
}
