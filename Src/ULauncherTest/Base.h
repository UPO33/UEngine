#pragma once

#include "../GFXCore/Base.h"
#include "../GFXDirectX/Context.h"
#include "../GFXCore/GlobalShader.h"
#include "../GFXDirectX/Shaders.h"
#include "../GFXDirectX/DescriptorHeap.h"
#include "../Engine/Scene.h"

using namespace UGFX;
using namespace UEngine;

extern GFXContextDX12* gGFX;


inline ID3D12Resource* UCreateDefaultCommittedUploadBuffer(size_t bufferSize, const void* pInitialData = nullptr)
{
	ID3D12Resource* ret = nullptr;
	UCHECK_DXRET(gGFX->mDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD)
		, D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(bufferSize), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&ret)));
	if (pInitialData)
	{
		void* mapped = nullptr;
		UCHECK_DXRET(ret->Map(0, nullptr, &mapped));
		MemCopy(mapped, pInitialData, bufferSize);
		ret->Unmap(0, nullptr);
	}
	return ret;
}