#include "Base.h"
#include "MainWindow.h"
#include "TestDraw.h"
#include "TestScene.h"

#include "../Core/Timer.h"
#include "../Engine/TaskMgr.h"
#include "../Engine/Scene.h"
#include "../Engine/Entity.h"

#include "ShaderFileWatcher.h"
#include "TestEntities.h"

#include "ImGUIImpl.h"
#include "../EditorBaseIM/EditorBaseIM.h"
#include "WorkSpace.h"
#include "WorkSpaceLevelEditor.h"

WindowWin32*	gMainWindow = nullptr;
GFXContextDX12*  gGFX = nullptr;
TestScreenQuadGen* gTestQuadGen = nullptr;
TestScene* gTestScene = nullptr;

HWND UGetMainHWND()
{
	return gMainWindow->mWndHandle;
}

LRESULT ImGuiWndProcHandler(HWND, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ImGuiIO& io = ImGui::GetIO();
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		io.MouseDown[0] = true;
		return true;
	case WM_LBUTTONUP:
		io.MouseDown[0] = false;
		return true;
	case WM_RBUTTONDOWN:
		io.MouseDown[1] = true;
		return true;
	case WM_RBUTTONUP:
		io.MouseDown[1] = false;
		return true;
	case WM_MBUTTONDOWN:
		io.MouseDown[2] = true;
		return true;
	case WM_MBUTTONUP:
		io.MouseDown[2] = false;
		return true;

	case WM_MOUSEWHEEL:
		io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
		return true;
	case WM_MOUSEMOVE:
		io.MousePos.x = (signed short)(lParam);
		io.MousePos.y = (signed short)(lParam >> 16);
		return true;
	case WM_KEYDOWN:
		if (wParam < 256)
			io.KeysDown[wParam] = 1;
		return true;
	case WM_KEYUP:
		if (wParam < 256)
			io.KeysDown[wParam] = 0;
		return true;
	case WM_CHAR:
		// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
		if (wParam > 0 && wParam < 0x10000)
			io.AddInputCharacter((unsigned short)wParam);
		return true;
	}
	return 0;
}

// Main message handler for the sample.
LRESULT CALLBACK UWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	WindowWin32* pSample = reinterpret_cast<WindowWin32*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	if (ImGuiWndProcHandler(hWnd, message, wParam, lParam))
	{
		 return true;
	}

	switch (message)
	{
	case WM_CREATE:
	{
		// Save the DXSample* passed in to CreateWindow.
		LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
		return 0;
	}

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

// 	case WM_PAINT:
// 		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	// Handle any messages the switch statement didn't.
	return DefWindowProc(hWnd, message, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////
void URenderFrame(CmdList* pCmdList)
{
	ID3D12GraphicsCommandList*  cmdList = pCmdList->GetCommandList();

	unsigned frameIndex = gGFX->mFrameIndex;

	float clearColor[] = { 0, RandFloat01(),0,0 };

	DescHandleRTV rtHandle = gMainWindow->mRTVHead + frameIndex;
	
	D3D12_VIEWPORT viewport = { 0,0, gMainWindow->mWidth, gMainWindow->mHeight, 0,1 };
	D3D12_RECT scissor = { 0,0, viewport.Width, viewport.Height };

	ID3D12Resource* curRTTexture = gMainWindow->mRTTextures[frameIndex];

	pCmdList->SetViewport(0, 0, gMainWindow->mWidth, gMainWindow->mHeight);
	pCmdList->SetScissor(0, 0, gMainWindow->mWidth, gMainWindow->mHeight);


	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(curRTTexture, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	cmdList->ClearRenderTargetView(rtHandle.GetCpuHandle(), clearColor, 0, nullptr);
	cmdList->OMSetRenderTargets(1, &rtHandle.GetCpuHandle(), false, nullptr);

	//render target is set, start rendering
	if(gTestQuadGen) gTestQuadGen->Render(pCmdList);
	if (gTestScene)
		gTestScene->Render(pCmdList);


	
	ImGUIImplDX12::Render(pCmdList);

	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(curRTTexture, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	

// 	UCHECK_DXRET(cmdList->Close());
}

uint64 gFrameFances[FRAME_COUNT] = {};
inline uint64 GetLastFrameFenceValue(unsigned curFrameIndex)
{
	return gFrameFances[(curFrameIndex + 1) % FRAME_COUNT];
}


bool URender()
{
	unsigned frameIndex = gGFX->mFrameIndex;


	//gGFX->mQueueMgr->WaitForIdle();
	CmdList* pCMDList = gGFX->mQueueMgr->GetNewGraphicCmdList();

	//pCMDList->SetSetDescriptorHeaps(gGFX->mDescriptorMgr->mCSUHeap.GetHeapPointer(), gGFX->mDescriptorMgr->mSamplerHeap.GetHeapPointer());
	URenderFrame(pCMDList);

	
	gFrameFances[frameIndex] = pCMDList->Finish(true); //currenly we wait for new frame

	(gMainWindow->mSwapChain->Present(1, 0));

	//Sleep(100);
	//the frame index that must be finished till now
	unsigned frmFinishIndex = (frameIndex + 1) % FRAME_COUNT;
	gGFX->mQueueMgr->GetGraphicQueue().WaitForFence(gFrameFances[frmFinishIndex]);



	//gGFX->mFrameIndex = gMainWindow->mSwapChain->GetCurrentBackBufferIndex();
	gGFX->mFrameIndex = (gGFX->mFrameIndex + 1) % FRAME_COUNT;



	return true;
}
void UTick(float delta)
{
	//Update scenes
	for (Scene* pScene : Scene::ScenesArray)
	{
		gScene = pScene;
		pScene->Tick(delta);
	}

	ImGUIImplDX12::NewFrame(gMainWindow->mWndHandle);

	
	gWorkSpace->Update();

	if (ImGui::IsMouseClicked(0))
	{
		if(!ImGui::IsAnyWindowHovered())
			ULOG_MESSAGE("Main Wnd Click");
	}

	ImGUIImplDX12::EndFrame();
}
//////////////////////////////////////////////////////////////////////////

ShaderFilesWatcher* gShadeFileWatcher;

//////////////////////////////////////////////////////////////////////////
bool UPeekMessages()
{
	MSG msg = {};
	// Check to see if any messages are waiting in the queue
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		// Translate the message and dispatch it to WindowProc()
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT)
		return false;

	return true;
}
//////////////////////////////////////////////////////////////////////////
void UEngine_ShutDown()
{
	SafeDelete(gWorkSpace);

	GameInput::Shutdown();

	gGFX->Release();
	gGFX = nullptr;
	SetGFXContext(nullptr);
}

ChronometerHR gTimer;
unsigned gFrameCounter = 0;

//////////////////////////////////////////////////////////////////////////
void UEngine_Step()
{
	static double LastFrameTime = gTimer.ElapsedSeconds();
	double timePoint = gTimer.ElapsedSeconds();
	double delta = timePoint - LastFrameTime;
	LastFrameTime = timePoint;


	bool bContinue = true;
	
	bContinue = UPeekMessages();



	UTick(delta);


	{

		
		//GameInput::Update(0.1);

		gShadeFileWatcher->Update();
		gShadeFileWatcher->RecompileModifides();

// 		if (GameInput::IsPressed(GameInput::kKey_escape))
// 			bContinue = false;
	}

	

	UEnqueue(EET_Render, [] {
		
		// Run game code here
		URender();

		UEnqueue(EET_Game, []() {
			UEngine_Step();
		});
	});
	
	Thread::Sleep(4);

	if (!bContinue)
	{
		UFlushTasks();
		UEngine_ShutDown();
		gTaskMgr->ReqestExit();
	}

	gFrameCounter++;	
}
//////////////////////////////////////////////////////////////////////////
void UCreateDebugScenes()
{
	Scene* pScene1 = new Scene(SceneCreationParam());
	gScene = pScene1;

	{
		//-Root
		//----Child0
		//-----------child00
		//----Child1
		//-----------child10


		Entity* pRoot = NewObject<Entity>();
		pRoot->SetName("Root");
		Entity* pNode0 = NewObject<Entity>(EntityTest0::GetClassStatic(), pRoot);
		pNode0->SetName("Child0");
		Entity* pNode1 = NewObject<Entity>(EntityTest0::GetClassStatic(), pRoot);
		pNode1->SetName("Chlid1");
		NewObject<EntityPrimitive>(EntityPrimitive::GetClassStatic(), pRoot);
		NewObject<EntityPrimitive>(EntityPrimitive::GetClassStatic(), pRoot);
		pRoot->RegisterEntityTo(pScene1->GetRootEntity());
		
		pScene1->GetRootEntity()->ForEachChild(true, true, [](Object* obj){
			Entity* pEnt = UCastSure<Entity>(obj);
			
		});
	}
	
}
void UInit_WorSpaces()
{
	gWorkSpace = new WorkSpaceContext;
	gWorkSpace->AddWorkSpace(new WorkSpaceDebug(), true);
	gWorkSpace->AddWorkSpace(new WorkSpaceLevelEditor(), true);
}
//////////////////////////////////////////////////////////////////////////
void UEngine_Init()
{
	gTimer.Reset();

	gGFX = new GFXContextDX12;
	SetGFXContext(gGFX);
	gGFX->Init();
	//auto cmdListInit = gGFX->GetCurFrameResource().mCmdList;

	CmdList* cmdList = gGFX->mQueueMgr->GetNewGraphicCmdList();
	

	UCreateDebugScenes();

	gTestQuadGen = new TestScreenQuadGen();
	gTestQuadGen->CreateResources();

	gTestScene = new TestScene();
	
	gMainWindow = new WindowWin32(1200, 800, L"wnd1");
	gMainWindow->CreateSwapChain();


	//GameInput::Initialize();

	gShadeFileWatcher = new ShaderFilesWatcher;

	ImGUIImplDX12::InitResources();
	ImGUIImplDX12::Init(gMainWindow->mWndHandle);

	UInit_WorSpaces();
}
//////////////////////////////////////////////////////////////////////////
//the first task that is executed on the main thread this function must not be a loop
void UEngineProc()
{
	UASSERT(UIsGameThread());

	UEngine_Init();
	UEngine_Step();
}

//////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
	gTaskMgr = new TaskMgr;
	gTaskMgr->Run(&UEngineProc);
	delete gTaskMgr;
	return 0;
}