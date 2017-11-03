#include "ImGUIImpl.h"
#include "../Core/Queue.h"

const unsigned IMGUI_UPLOAD_BUFFER_SIZE = 1024 * 128 * sizeof(ImDrawVert);

UGFX::GFXRootSignature* ImGUIImplDX12::_RootSignature;

UGFX::GFXPiplineState* ImGUIImplDX12::_PS;

UGFX::DescHandleDSV ImGUIImplDX12::_FontSRV;

ID3D12Resource* ImGUIImplDX12::_FontTexture;

ID3D12Resource* ImGUIImplDX12::_UploadBuffer;



UGFX::CmdList* ImGUIImplDX12::_CurCommandList;

void ImGUIImplDX12::InitResources()
{
	auto pDevice = gGFX->mDevice;

	/////////////RootSig
	GFXRootSignatureDesc rsDesc;
	rsDesc << GFXRootParameter::CreateDescriptorTable(EDR_SRV, 8);
	rsDesc << GFXRootParameter::CreateCBV(0);
	

	_RootSignature = gGFX->CreateRootSignature(rsDesc);

	/////////////IL
	GFXInputLayout* il = new GFXInputLayout;
	il->mElements.AddDefault(3);

	il->mElements[0].mSemanticName = "POSITION";
	il->mElements[0].mFormat = EGFXFormat::R32G32_FLOAT;
	il->mElements[0].mAlignedByteOffset = offsetof(ImDrawVert, pos);


	il->mElements[1].mSemanticName = "UV";
	il->mElements[1].mFormat = EGFXFormat::R32G32_FLOAT;
	il->mElements[1].mAlignedByteOffset = offsetof(ImDrawVert, uv);


	il->mElements[2].mSemanticName = "COLOR";
	il->mElements[2].mFormat = EGFXFormat::R8G8B8A8_UNORM;
	il->mElements[2].mAlignedByteOffset = offsetof(ImDrawVert, col);

	/////////////PSO
	GFXGraphicsPiplineStateDesc psDesc;
	psDesc.mVS = GetGFXContext()->GetShaderMgr()->GetShader("Imgui.hlsl", "VSMain", EShader::Vertex);
	psDesc.mPS = GetGFXContext()->GetShaderMgr()->GetShader("Imgui.hlsl", "PSMain", EShader::Pixel);
	psDesc.mBlendState.mRenderTarget[0].InitAsAlphaBlend();
	psDesc.mNumRenderTargets = 1;
	psDesc.mRTVFormats[0] = EGFXFormat::R8G8B8A8_UNORM;
	psDesc.mPrimitiveTopologyType = EPrimitiveTopologyType::TRIANGLE;
	psDesc.mInputLayout = il;
	psDesc.mRasterizer.mCullMode = EGFXCullMode::None;
	psDesc.mRasterizer.mAntialiasedLineEnable = true;
	psDesc.mDepthStencil.mDepthEnable = false;
	psDesc.mRootSignature = _RootSignature;

	_PS = GetGFXContext()->GetPipelineStateMgr()->GetGPS(psDesc);

	////////
	_UploadBuffer = UCreateDefaultCommittedUploadBuffer(IMGUI_UPLOAD_BUFFER_SIZE);
	//////////////
	HRESULT hr;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.PlaneSlice = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.f;

	// Create fonts texture and SRV descriptor for it
	unsigned char* pixels = 0;
	int width, height;

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	// Create fonts texture
	D3D12_RESOURCE_DESC desc;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Alignment = 0;
	desc.Width = width;
	desc.Height = height;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;
	
	
	hr = pDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &desc
		, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&_FontTexture));

	UCHECK_DXRET(hr);

	//ID3D12Resource* fontResource = createCommittedResource(_device, HeapProperty::Default, &desc, 0);

	// Upload the fonts texture
	uint32_t subres = 0;
	uint32_t numRows;
	uint64_t rowPitch;
	uint64_t totalBytes;
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout;
	pDevice->GetCopyableFootprints(&desc
		, subres
		, 1
		, 0
		, &layout
		, &numRows
		, &rowPitch
		, &totalBytes
	);

	uint8_t* mappedBuffer;

	// Upload the font
	_UploadBuffer->Map(0, NULL, (void**)&mappedBuffer);
	memcpy(mappedBuffer, pixels, (size_t)totalBytes);
	_UploadBuffer->Unmap(0, NULL);

	D3D12_BOX box;
	box.left = 0;
	box.top = 0;
	box.right = (UINT)desc.Width;
	box.bottom = (UINT)desc.Height;
	box.front = 0;
	box.back = 1;

	auto pCmdList = gGFX->mQueueMgr->GetNewGraphicCmdList();
	auto _commandList = pCmdList->GetCommandList();

	D3D12_TEXTURE_COPY_LOCATION dst = { _FontTexture,   D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,{ subres } };
	D3D12_TEXTURE_COPY_LOCATION src = { _UploadBuffer, D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,  layout };
	_commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, &box);

	pCmdList->Finish(true);

	_FontSRV = gGFX->mDescriptorMgr->AllocCSU(true);
	pDevice->CreateShaderResourceView(_FontTexture, &srvDesc, _FontSRV);
}

bool ImGUIImplDX12::Init(void* _hwnd)
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeyMap[ImGuiKey_Tab] = VK_TAB;                              // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array that we will update during the application lifetime.
	io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
	io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
	io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
	io.KeyMap[ImGuiKey_Home] = VK_HOME;
	io.KeyMap[ImGuiKey_End] = VK_END;
	io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
	io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
	io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
	io.KeyMap[ImGuiKey_A] = 'A';
	io.KeyMap[ImGuiKey_C] = 'C';
	io.KeyMap[ImGuiKey_V] = 'V';
	io.KeyMap[ImGuiKey_X] = 'X';
	io.KeyMap[ImGuiKey_Y] = 'Y';
	io.KeyMap[ImGuiKey_Z] = 'Z';

	io.RenderDrawListsFn = RenderDrawLists;
	io.ImeWindowHandle = _hwnd;

	// 		if (!QueryPerformanceFrequency((LARGE_INTEGER *)&g_TicksPerSecond))
	// 			return false;
	// 		if (!QueryPerformanceCounter((LARGE_INTEGER *)&g_Time))
	// 			return false;

	return true;
}

struct SImDrawList
{
	// This is what you have to render
	ImVector<ImDrawCmd>     CmdBuffer;          // Commands. Typically 1 command = 1 GPU draw call.
	ImVector<ImDrawIdx>     IdxBuffer;          // Index buffer. Each command consume ImDrawCmd::ElemCount of those
	ImVector<ImDrawVert>    VtxBuffer;          // Vertex buffer.
	


	void Reset(ImDrawList* dl)
	{
		CmdBuffer.resize(dl->CmdBuffer.size());
		IdxBuffer.resize(dl->IdxBuffer.size());
		VtxBuffer.resize(dl->VtxBuffer.size());

		MemCopy(CmdBuffer.Data, dl->CmdBuffer.Data, sizeof(ImDrawCmd) * CmdBuffer.size());
		MemCopy(IdxBuffer.Data, dl->IdxBuffer.Data, sizeof(ImDrawIdx) * IdxBuffer.size());
		MemCopy(VtxBuffer.Data, dl->VtxBuffer.Data, sizeof(ImDrawVert) * VtxBuffer.size());

	}
};
struct SImDrawData
{
	TArray<SImDrawList> CmdLists;
	unsigned CmdListsCount; //use this not CmdLists.Length

	ImVec2 mDisplaySize;

	void Reset(ImDrawData* dd)
	{
		CmdListsCount = dd->CmdListsCount;

		if (CmdLists.Length() < dd->CmdListsCount)
			CmdLists.AddDefault(dd->CmdListsCount - CmdLists.Length());

		for (unsigned i = 0; i < dd->CmdListsCount; i++)
		{
			CmdLists[i].Reset(dd->CmdLists[i]);
		}

		mDisplaySize = ImGui::GetIO().DisplaySize;

	}
};

struct
{
	SImDrawData mData[3];	//double buffering
	unsigned mWriteIndex = 0;

	SImDrawData& GetFoWrite() { return mData[mWriteIndex % 3]; }
	SImDrawData& GetForRead() { return mData[(mWriteIndex + 1) % 3]; }
	
} gIMGuiDrawCtx;
//this function is called in game thread currently we use double buffering to store the data to draw them in render thread
void ImGUIImplDX12::RenderDrawLists(ImDrawData* _draw_data)
{
	SImDrawData& wCtx = gIMGuiDrawCtx.GetFoWrite();
	wCtx.Reset(_draw_data);

	gIMGuiDrawCtx.mWriteIndex++;
}

void ImGUIImplDX12::Render(CmdList* pCmdList)
{
	SImDrawData* _draw_data = &(gIMGuiDrawCtx.GetForRead());

	ID3D12Resource* pUploadBuffer = _UploadBuffer;

	// Range CPU will read from mapping the upload buffer
	// End < Begin specifies CPU will not read the mapped buffer
	D3D12_RANGE readRange;
	readRange.End = 0;
	readRange.Begin = 1;

	char* mappedBuffer = 0;
	HRESULT hr = pUploadBuffer->Map(0, &readRange, (void**)&mappedBuffer);
	UCHECK_DXRET(hr);


	char* writeCursor = mappedBuffer;

	// Copy the projection matrix at the beginning of the buffer
	{

		float translate = -0.5f * 2.f;
		const float L = 0.f;
		const float B = _draw_data->mDisplaySize.y;
		const float R = _draw_data->mDisplaySize.x;
		const float T = 0.f;

		//Matrix4 mvp = Matrix4::IDENTITY;
		//Matrix4 mvp = Matrix4Translation(-1, 1, 0) * Matrix4Scale(2, -2, 0) * Matrix4Scale(1 / R, 1 / B, 0);
		Matrix4 mvp = Matrix4Ortho(R, -B, 0, 100) * Matrix4Translation(-R/2, -B/ 2, 0);

		// 		const float mvp[4][4] =
		// 		{
		// 			{ 2.0f / (R - L),   0.0f,           0.0f,       0.0f },
		// 			{ 0.0f,         2.0f / (T - B),     0.0f,       0.0f, },
		// 			{ 0.0f,         0.0f,           0.5f,       0.0f },
		// 			{ (R + L) / (L - R),  (T + B) / (B - T),    0.5f,       1.0f },
		// 		};

		memcpy(writeCursor, &mvp, sizeof(mvp));
		writeCursor += sizeof(mvp);
	}

	// Copy the vertices and indices for each command list
	for (size_t n = 0; n < _draw_data->CmdListsCount; n++)
	{
		const SImDrawList* cmd_list = &(_draw_data->CmdLists[n]);
		size_t verticesCount = cmd_list->VtxBuffer.size();
		size_t indicesCount = cmd_list->IdxBuffer.size();
		size_t verticesSize = verticesCount * sizeof(ImDrawVert);
		size_t indicesSize = indicesCount * sizeof(ImDrawIdx);

		// Copy the vertex data
		memcpy(writeCursor, &cmd_list->VtxBuffer[0], verticesSize);
		writeCursor += verticesSize;

		// Copy the index data
		memcpy(writeCursor, &cmd_list->IdxBuffer[0], indicesSize);
		writeCursor += indicesSize;
	}

	ID3D12GraphicsCommandList* commandList = pCmdList->GetCommandList();

// 	D3D12_VIEWPORT viewport;
// 	viewport.Width = ImGui::GetIO().DisplaySize.x;
// 	viewport.Height = ImGui::GetIO().DisplaySize.y;
// 	viewport.MinDepth = 0.f;
// 	viewport.MaxDepth = 1.f;
// 	viewport.TopLeftX = 0.f;
// 	viewport.TopLeftY = 0.f;

	//commandList->RSSetViewports(1, &viewport);
	//commandList->OMSetRenderTargets(1, &g_renderTarget, FALSE, nullptr);
	pCmdList->SetGraphicsRootSignature(_RootSignature);
	pCmdList->SetPrimitiveTopology(EPrimitiveTopology::TriangleList);
	pCmdList->SetPipelineState(_PS);
	pCmdList->SetGraphicsRootDescriptorTable(0, _FontSRV);

	D3D12_GPU_VIRTUAL_ADDRESS bufferAddress = pUploadBuffer->GetGPUVirtualAddress();
	pCmdList->SetGraphicsRootCBV(1, bufferAddress);

	uint64_t readCursor = 64; // Our constant buffer takes 64 bytes - one mat4x4

	for (size_t n = 0; n < _draw_data->CmdListsCount; n++)
	{
		// Render command lists
		int vtx_offset = 0;
		int idx_offset = 0;

		const SImDrawList* cmd_list = &(_draw_data->CmdLists[n]);
		size_t verticesCount = cmd_list->VtxBuffer.size();
		size_t indicesCount = cmd_list->IdxBuffer.size();
		size_t verticesSize = verticesCount * sizeof(ImDrawVert);
		size_t indicesSize = indicesCount * sizeof(ImDrawIdx);

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
		vertexBufferView.BufferLocation = bufferAddress + readCursor;
		vertexBufferView.StrideInBytes = sizeof(ImDrawVert);
		vertexBufferView.SizeInBytes = verticesSize;
		readCursor += verticesSize;

		D3D12_INDEX_BUFFER_VIEW indexBufferView;
		indexBufferView.BufferLocation = bufferAddress + readCursor;
		indexBufferView.SizeInBytes = indicesSize;
		indexBufferView.Format = DXGI_FORMAT_R16_UINT;
		readCursor += indicesSize;

		commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
		commandList->IASetIndexBuffer(&indexBufferView);

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.size(); cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				//pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				const D3D12_RECT r = { (LONG)pcmd->ClipRect.x, (LONG)pcmd->ClipRect.y, (LONG)pcmd->ClipRect.z, (LONG)pcmd->ClipRect.w };
				commandList->RSSetScissorRects(1, &r);
				commandList->DrawIndexedInstanced(pcmd->ElemCount, 1, idx_offset, vtx_offset, 0);
			}
			idx_offset += pcmd->ElemCount;
		}
		vtx_offset += verticesCount;
	}
 
}

void ImGUIImplDX12::NewFrame(HWND wnd)
{
	ImGuiIO& io = ImGui::GetIO();

	// Setup display size (every frame to accommodate for window resizing)
	RECT rect;
	GetClientRect(wnd, &rect);
	io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));

	// Setup time step
	INT64 current_time;
	QueryPerformanceCounter((LARGE_INTEGER *)&current_time);
	io.DeltaTime = 0.01;// (float)(current_time - g_Time) / g_TicksPerSecond;
	//g_Time = current_time;

	// Read keyboard modifiers inputs
	io.KeyCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
	io.KeyShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
	io.KeyAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;
	// io.KeysDown : filled by WM_KEYDOWN/WM_KEYUP events
	// io.MousePos : filled by WM_MOUSEMOVE events
	// io.MouseDown : filled by WM_*BUTTON* events
	// io.MouseWheel : filled by WM_MOUSEWHEEL events

	// Hide OS mouse cursor if ImGui is drawing it
	SetCursor(io.MouseDrawCursor ? NULL : LoadCursor(NULL, IDC_ARROW));

	// Start the frame
	ImGui::NewFrame();
}

void ImGUIImplDX12::EndFrame()
{
	//our Render draw list is called after calling this function
	ImGui::Render();
}