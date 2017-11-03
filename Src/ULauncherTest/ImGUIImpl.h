#pragma once

#include "Base.h"

#include "../IMGUI/imgui.h"
#include "../IMGUI/imgui_internal.h"


struct ImGUIImplDX12
{
	static GFXRootSignature* _RootSignature;
	static GFXPiplineState* _PS;
	static DescHandleDSV _FontSRV;
	static ID3D12Resource* _FontTexture;
	static ID3D12Resource* _UploadBuffer;
	static CmdList* _CurCommandList;

	static void InitResources();
	static bool Init(void* _hwnd);
	static void RenderDrawLists(ImDrawData* _draw_data);

public:
	//must be called from the RenderThread,
	static void Render(CmdList*);
	static void NewFrame(HWND wnd);
	//set _CurCommandList before calling this
	static void EndFrame();

};