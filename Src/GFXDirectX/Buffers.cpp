#include "Buffers.h"
#include "Context.h"

namespace UGFX
{

//////////////////////////////////////////////////////////////////////////
#if 0
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
#endif

GFXBuffer* GFXContextDX12::CreateStaticVB(size_t size, const void* initialData, const char* debugStr /* = nullptr */)
{
	ID3D12Resource* uploadBuffer = nullptr;
	HRESULT hr = mDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(size), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploadBuffer));
	
	if (SUCCEEDED(hr))
	{
		ID3D12Resource* defaultBuffer = nullptr;
		hr = mDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(size), D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&defaultBuffer));
		
		if (SUCCEEDED(hr))
		{
			D3D12_SUBRESOURCE_DATA srd;
			srd.pData = initialData;
			srd.RowPitch = size;
			srd.SlicePitch = size;

			CmdList* cmdList = this->mQueueMgr->GetNewGraphicCmdList();

			UpdateSubresources<1>(cmdList->GetCommandList(), defaultBuffer, uploadBuffer, 0, 0, 1, &srd);

			cmdList->Finish(true);
			
			return new BufferDX12(defaultBuffer);
		}
	}

}

GFXBuffer* GFXContextDX12::CreateStaticIB(size_t size, const void* initialData, const char* debugStr /* = nullptr */)
{
	return GFXContextDX12::CreateStaticVB(size, initialData, debugStr);
}

};