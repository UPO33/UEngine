#pragma once
#include "../Core/Base.h"
#include "../GFXDirectX/Base.h"

using namespace UCore;
using namespace UGFX;

LRESULT CALLBACK UWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//////////////////////////////////////////////////////////////////////////
class WindowWin32
{
public:
	HWND mWndHandle = nullptr;
	unsigned mWidth = 800;
	unsigned mHeight = 600;
	IDXGISwapChain3*		mSwapChain = nullptr;
	ID3D12Resource*			mRTTextures[FRAME_COUNT] = {};
	ID3D12DescriptorHeap*	mRTDescHeap = nullptr;
	
	WindowWin32(unsigned w, unsigned h, const wchar_t* wndClass) : mWidth(w), mHeight(h)
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

	void CreateSwapChain();


};

//////////////////////////////////////////////////////////////////////////
class DX12Context : public GFXContextBase
{
public:
	ID3D12Device*					mDevice;
	ID3D12CommandQueue*				mGFXQueue;
	ID3D12CommandQueue*				mComputeQueue;
	IDXGIFactory4*					mGIFactory;
	ID3D12Fence*					mFance;
	unsigned						mDescHandleSize[8];
	unsigned						mFrameIndex = 0;
	uint64							mFenceValue[FRAME_COUNT] = {};
	HANDLE							mEvent;
	

	struct FrameResource
	{
		ID3D12GraphicsCommandList*		mCmdList = nullptr;
		ID3D12CommandAllocator*			mCmdAllocator = nullptr;
	};
	
	FrameResource	mFrameResources[FRAME_COUNT];
	
	FrameResource& GetCurFrameResource() { return mFrameResources[mFrameIndex]; }

	//////////////////////////////////////////////////////////////////////////
	virtual void Init()
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

		mDescHandleSize[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV] = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		mDescHandleSize[D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER] = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		mDescHandleSize[D3D12_DESCRIPTOR_HEAP_TYPE_RTV] = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		mDescHandleSize[D3D12_DESCRIPTOR_HEAP_TYPE_DSV] = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);



		// Describe and create the command queue.
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		UHRFATAL(mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mGFXQueue)));

		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		UHRFATAL(mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mComputeQueue)));

		{
			for (size_t iFrame = 0; iFrame < FRAME_COUNT; iFrame++)
			{
				ID3D12CommandAllocator* ca = nullptr;
				ID3D12GraphicsCommandList* cl = nullptr;

				UCHECK_DXRET(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&ca)));
				UCHECK_DXRET(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, ca, nullptr, IID_PPV_ARGS(&cl)));

				cl->Close();
				
				mFrameResources[iFrame].mCmdAllocator = ca;
				mFrameResources[iFrame].mCmdList = cl;

			}
		}
		
		mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFance));

		mEvent = ::CreateEvent(nullptr, false, false, nullptr);

	}
	//wait for graphic queue to be finished
	void WaitForGFXQueue()
	{
		uint64 fv = mFenceValue[mFrameIndex];
		mFance->SetEventOnCompletion(fv, mEvent);
		mGFXQueue->Signal(mFance, fv);
		mFenceValue[mFrameIndex]++;
		WaitForSingleObject(mEvent, INFINITE);
	}
	void WaitForComputeQueue()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	virtual void Release()
	{
		mComputeQueue->Release();
		mGFXQueue->Release();
		mDevice->Release();
		mGIFactory->Release();
	}

	virtual const char* GetAPIName() const override
	{
		return "DX12";
	}
	// Helper function for acquiring the first available hardware adapter that supports Direct3D 12
	IDXGIAdapter3* GetHardwareAdapter(IDXGIFactory4* pFactory, D3D_FEATURE_LEVEL featureLevel) const
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

// 	D3D12_SHADER_BYTECODE UGetShader(const wchar_t* name, const char* entrypoint, EShader shader);
};

extern DX12Context* gGFX;