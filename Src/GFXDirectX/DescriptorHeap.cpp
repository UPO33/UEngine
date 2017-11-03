#include "DescriptorHeap.h"
#include "Context.h"

namespace UGFX
{

//////////////////////////////////////////////////////////////////////////
UGFXDIRECTX_API unsigned UGetDescHandleIncSize(D3D12_DESCRIPTOR_HEAP_TYPE heapType)
{
	return GetGFXContextDX12()->mDescHandleIncSize[heapType];
}

//////////////////////////////////////////////////////////////////////////
void DescHeapPool::Init(ID3D12Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE handleType, unsigned capcity, bool shaderVisible)
{
	mHeapDesc.NodeMask = 0;
	mHeapDesc.NumDescriptors = capcity;
	mHeapDesc.Type = handleType;
	mHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		
	UCHECK_DXRET(pDevice->CreateDescriptorHeap(&mHeapDesc, IID_PPV_ARGS(&mHeap)));
	mHandleIncSize = pDevice->GetDescriptorHandleIncrementSize(mHeapDesc.Type);
	mNumFreeDescriptors = mHeapDesc.NumDescriptors;
	mFirstHandle = DescHandleBase(mHeap->GetCPUDescriptorHandleForHeapStart(), mHeap->GetGPUDescriptorHandleForHeapStart());
	mNextFreeHandle = mFirstHandle;

}
//////////////////////////////////////////////////////////////////////////
DescHandleBase DescHeapPool::Alloc(uint32_t Count)
{
	UASSERT(HasAvailableSpace(Count), "Descriptor Heap out of space.  Increase heap size.");
	DescHandleBase ret = mNextFreeHandle;
	mNextFreeHandle.Offset(Count * mHandleIncSize);
	return ret;
}

DescHandleBase DescHeapPool::GetHandleAtOffset(uint32_t Offset) const
{
	return mFirstHandle.GetAt(Offset * mHandleIncSize);
}

//////////////////////////////////////////////////////////////////////////
bool DescHeapPool::ValidateHandle(const DescHandleBase& handle) const
{
	if (handle.GetCpuHandle().ptr < mFirstHandle.GetCpuHandle().ptr ||
		handle.GetCpuHandle().ptr >= mFirstHandle.GetCpuHandle().ptr + mHeapDesc.NumDescriptors * mHandleIncSize)
		return false;

	if (handle.GetGpuHandle().ptr - mFirstHandle.GetGpuHandle().ptr !=
		handle.GetCpuHandle().ptr - mFirstHandle.GetCpuHandle().ptr)
		return false;

	return true;
}
//////////////////////////////////////////////////////////////////////////
DescriptorMgrDX12::DescriptorMgrDX12(GFXContextDX12* ctx) : mCtx(ctx)
{
	mCSUHeap.Init(mCtx->mDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 8192, true);
	mSamplerHeap.Init(ctx->mDevice, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 1024, true);

	mNonVisibleCSUHeap.Init(mCtx->mDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 8192, false);
	mNonVisibleSmaplerHeap.Init(mCtx->mDevice, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 1024, false);

	mNonVisibleRTVHeap.Init(mCtx->mDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2048, false);
	mNonVisibleDSVHeap.Init(mCtx->mDevice, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 2048, false);
}
//////////////////////////////////////////////////////////////////////////
DescHandleBase DescriptorMgrDX12::AllocRTV(unsigned count /*= 1*/)
{
	return mNonVisibleRTVHeap.Alloc(count);
}

DescHandleBase DescriptorMgrDX12::AllocDSV(unsigned count /*= 1*/)
{
	return mNonVisibleDSVHeap.Alloc(count);
}

DescHandleBase DescriptorMgrDX12::AllocCSU(bool shaderVisible, unsigned count /*= 1*/)
{
	if (shaderVisible)
		return mCSUHeap.Alloc(count);

	return mNonVisibleCSUHeap.Alloc(count);
}

DescHandleBase DescriptorMgrDX12::AllocSampler(bool shaderVisible, unsigned count /*= 1*/)
{
	if (shaderVisible)
		return mSamplerHeap.Alloc(count);

	return mNonVisibleSmaplerHeap.Alloc(count);
}



}