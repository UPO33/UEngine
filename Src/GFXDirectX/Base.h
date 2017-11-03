#pragma once

#include "../GFXCore/Base.h"
#include "../Core/Name.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <d3d11.h>
#include <d3d10.h>
#include <d3d9.h>
#include <dxgi1_5.h>

#ifdef UGFXDIRECTX_SHARED
#ifdef UGFXDIRECTX_BUILD
#define UGFXDIRECTX_API UMODULE_EXPORT
#else
#define UGFXDIRECTX_API UMODULE_IMPORT
#endif
#else
#define UGFXDIRECTX_API
#endif

#define UHRFATAL(HR) UASSERT(SUCCEEDED(HR))
#define IIDHANDLE(type, handle) __uuidof(type), reinterpret_cast<void**>(&handle.mPtr)
#define UCHECK_DXRET(hr) UASSERT(SUCCEEDED(hr))

#define D3D12_GPU_VIRTUAL_ADDRESS_NULL      ((D3D12_GPU_VIRTUAL_ADDRESS)0)
#define D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN   ((D3D12_GPU_VIRTUAL_ADDRESS)-1)

namespace UGFX
{
	using namespace UCore;
	using namespace UGFX;

	//helper operators for CPU handle
	inline D3D12_CPU_DESCRIPTOR_HANDLE operator + (D3D12_CPU_DESCRIPTOR_HANDLE handle, size_t sizeInBytes)
	{
		return D3D12_CPU_DESCRIPTOR_HANDLE{ handle.ptr + sizeInBytes };
	}
	inline D3D12_CPU_DESCRIPTOR_HANDLE operator - (D3D12_CPU_DESCRIPTOR_HANDLE handle, size_t sizeInBytes)
	{
		return D3D12_CPU_DESCRIPTOR_HANDLE{ handle.ptr - sizeInBytes };
	}
	//helper operator for GPU handle
	inline D3D12_GPU_DESCRIPTOR_HANDLE operator + (D3D12_GPU_DESCRIPTOR_HANDLE handle, UINT64 sizeInBytes)
	{
		return D3D12_GPU_DESCRIPTOR_HANDLE{ handle.ptr + sizeInBytes };
	}
	inline D3D12_GPU_DESCRIPTOR_HANDLE operator - (D3D12_GPU_DESCRIPTOR_HANDLE handle, UINT64 sizeInBytes)
	{
		return D3D12_GPU_DESCRIPTOR_HANDLE{ handle.ptr + sizeInBytes };
	}


	static const unsigned FRAME_COUNT = 3;

#if 0
	//maybe GFXDevice->CreateSingleDescriptor(EGFXDescriptor::SRV)
	//there are 3 kinds of descriptor: Sampler, DSV, RTV, CBV_SRV_UAV
	void Dummy(ID3D12Device* pDevice)
	{
		ID3D12DescriptorHeap* descHeap = nullptr;
		D3D12_DESCRIPTOR_HEAP_DESC desc;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		desc.NodeMask = 0;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors = 8;

		pDevice->CreateDescriptorHeap(&desc, __uuidof(ID3D12DescriptorHeap), &descHeap);
	}
#endif

	//////////////////////////////////////////////////////////////////////////
#if 0

	class WindowWin32
	{
	public:
		HWND mWndHandle = nullptr;
		unsigned mWidth = 800;
		unsigned mHeight = 600;
		
		void CreateTheWindow()
		{
			auto hInstance = GetModuleHandle(nullptr);

			// Initialize the window class.
			WNDCLASSEX windowClass = { 0 };
			windowClass.cbSize = sizeof(WNDCLASSEX);
			windowClass.style = CS_HREDRAW | CS_VREDRAW;
			windowClass.lpfnWndProc = WindowProc;
			windowClass.hInstance = hInstance;
			windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
			windowClass.lpszClassName = TEXT("DXSampleClass");
			RegisterClassEx(&windowClass);

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
		}
		void DestroyTheWindwo()
		{

		}
	};

	class ViewportWindowDX12 : public ViewportBase
	{
	public:
		IDXGISwapChain4*	mSwapChain = nullptr;
		
	};
	class ViewportRenderTarget : public ViewportBase
	{
		ID3D12Resource*					mRTTextures[FRAME_COUNT];
		D3D12_CPU_DESCRIPTOR_HANDLE		mRTVs[FRAME_COUNT];
		D3D12_VIEWPORT					mViewportRect;
		
		void Test(ID3D12GraphicsCommandList* cmdList)
		{
			cmdList->RSSetViewports();
			cmdList->RSSetScissorRects();
			cmdList->OMSetRenderTargets();
			cmdList->ClearRenderTargetView();
			cmdList->ClearDepthStencilView();


		}
	};
#endif // 0


	//////////////////////////////////////////////////////////////////////////
	inline D3D12_QUERY_TYPE					UToDX12(EGFXQueryType in) { return (D3D12_QUERY_TYPE)in; }
	inline D3D12_PRIMITIVE_TOPOLOGY			UToDX12(EPrimitiveTopology in) { return (D3D12_PRIMITIVE_TOPOLOGY)in; }
	inline D3D12_COMMAND_LIST_TYPE			UToDX12(EGFXCommandListType in) { return (D3D12_COMMAND_LIST_TYPE)in; }
	inline D3D12_BLEND						UToDX12(EGFXBlend in) { return (D3D12_BLEND)in; }
	inline D3D12_BLEND_OP					UToDX12(EGFXBlendOp in) { return (D3D12_BLEND_OP)in; }
	inline UINT8							UToDX12(EGFXColorWriteEnable in) { return (UINT8)in; }
	inline DXGI_FORMAT						UToDX12(EGFXFormat in) { return (DXGI_FORMAT)in; }
	inline D3D12_PRIMITIVE_TOPOLOGY_TYPE	UToDX12(EPrimitiveTopologyType in) { return (D3D12_PRIMITIVE_TOPOLOGY_TYPE)in; }
	inline D3D12_CULL_MODE					UToDX12(EGFXCullMode in) { return (D3D12_CULL_MODE)in; }
	inline D3D12_COMPARISON_FUNC			UToDX12(EGFXComparison in) { return (D3D12_COMPARISON_FUNC)in; }
	inline D3D12_STENCIL_OP					UToDX12(EGFXStencilOp in) { return (D3D12_STENCIL_OP)in; }

	D3D12_BLEND_DESC				UToDX12(const GFXBlendDesc& input);
	D3D12_RASTERIZER_DESC			UToDX12(const GFXRasterizerDesc& input);
	D3D12_DEPTH_STENCILOP_DESC		UToDX12(const GFXDepthStencilOpDesc& input);
	D3D12_DEPTH_STENCIL_DESC		UToDX12(const GFXDepthStencilDesc& input);

};
