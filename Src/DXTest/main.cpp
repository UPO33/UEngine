#include "Base.h"
#include "Scene.h"

DX12Context*	gGFX = nullptr;
WindowWin32*	gMainWindow = nullptr;
TestScene*		gScene = nullptr;

// Main message handler for the sample.
LRESULT CALLBACK UWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WindowWin32* pSample = reinterpret_cast<WindowWin32*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (message)
	{
	case WM_CREATE:
	{
		// Save the DXSample* passed in to CreateWindow.
		LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
	}
	return 0;

	case WM_KEYDOWN:
		if (pSample)
		{
			//pSample->OnKeyDown(static_cast<UINT8>(wParam));
		}
		return 0;

	case WM_KEYUP:
		if (pSample)
		{
			//pSample->OnKeyUp(static_cast<UINT8>(wParam));
		}
		return 0;

	case WM_PAINT:
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	// Handle any messages the switch statement didn't.
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void URenderFrame(ID3D12GraphicsCommandList* cmdList)
{
	unsigned frameIndex = gGFX->mFrameIndex;

	float clearColor[] = { 0, RandFloat01(),0,0 };
	D3D12_CPU_DESCRIPTOR_HANDLE rtHandle = D3D12_CPU_DESCRIPTOR_HANDLE{ gMainWindow->mRTDescHeap->GetCPUDescriptorHandleForHeapStart().ptr + frameIndex * gGFX->mDescHandleIncSize[D3D12_DESCRIPTOR_HEAP_TYPE_RTV] };
	
	D3D12_VIEWPORT viewport = { 0,0, gMainWindow->mWidth, gMainWindow->mHeight, 0,1 };
	D3D12_RECT scissor = { 0,0, viewport.Width, viewport.Height };

	ID3D12Resource* curRTTexture = gMainWindow->mRTTextures[frameIndex];

	cmdList->RSSetViewports(1, &viewport);
	cmdList->RSSetScissorRects(1, &scissor);


	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(curRTTexture, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	
	cmdList->ClearRenderTargetView(rtHandle, clearColor, 0, nullptr);
	cmdList->OMSetRenderTargets(1, &rtHandle, false, nullptr);
	
	gScene->Render(cmdList);
	
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(curRTTexture, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	UCHECK_DXRET(cmdList->Close());
}
bool URender()
{
	ID3D12CommandAllocator* curCmdAllocator = gGFX->GetCurFrameResource().mCmdAllocator;
	ID3D12GraphicsCommandList* curCmdList = gGFX->GetCurFrameResource().mCmdList;

	curCmdAllocator->Reset();
	curCmdList->Reset(curCmdAllocator, nullptr);

	URenderFrame(curCmdList);

	ID3D12CommandList* cmds[] = { curCmdList };
	gGFX->mGFXQueue->ExecuteCommandLists(1, cmds);

 	(gMainWindow->mSwapChain->Present(1, 0));

	gGFX->WaitForGFXQueue();
	//Sleep(100);

	//gGFX->mFrameIndex = gMainWindow->mSwapChain->GetCurrentBackBufferIndex();
	gGFX->mFrameIndex = (gGFX->mFrameIndex + 1) % FRAME_COUNT;

	return true;
}

int main(int argc, char** argv) 
{
	gGFX = new DX12Context;
	gGFX->Init();
	gMainWindow = new WindowWin32(800, 600, L"wnd1");
	gMainWindow->CreateSwapChain();
	gScene = new TestScene;
	gScene->CreateResources();

	do 
	{
		MSG msg = {};
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			URender();
		}
		if (msg.message == WM_QUIT)
			break;
		
	} while (true);


	return 0;
}