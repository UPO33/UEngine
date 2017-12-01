#pragma once

#include "Base.h"
#include "DescriptorHeap.h"
#include "../Core/Threading.h"

#include <vector>
#include <queue>

namespace UGFX
{

//////////////////////////////////////////////////////////////////////////
struct UGFXDIRECTX_API CommandAllocatorPool
{
	CommandAllocatorPool(D3D12_COMMAND_LIST_TYPE type) : mType(type)
	{}
	~CommandAllocatorPool() 
	{
		Shutdown();
	}

	void Initilize(ID3D12Device* pDevice);
	void Shutdown();

	ID3D12CommandAllocator* RequestAllocator(uint64_t CompletedFenceValue);
	void DiscardAllocator(uint64_t FenceValue, ID3D12CommandAllocator* Allocator);

	inline size_t Size() { return m_AllocatorPool.Length(); }

	const D3D12_COMMAND_LIST_TYPE mType;
	ID3D12Device* m_Device = nullptr;
	TArray<ID3D12CommandAllocator*> m_AllocatorPool;
	std::queue<std::pair<uint64, ID3D12CommandAllocator*>> m_ReadyAllocators;
	StdLock m_AllocatorMutex;
};

//////////////////////////////////////////////////////////////////////////
class GFXContextDX12;
struct CmdQueue;
class QueueMgr;

//////////////////////////////////////////////////////////////////////////
class UGFXDIRECTX_API CmdList
{
	friend CmdQueue;
	friend QueueMgr;

	CmdList(QueueMgr* qMgr, D3D12_COMMAND_LIST_TYPE Type, ID3D12GraphicsCommandList* cmd, ID3D12CommandAllocator* allocator) :
		mQueueMgr(qMgr), m_Type(Type), m_CommandList(cmd), m_CurrentAllocator(allocator)
	{}
	void Reset(void);

public:

	// Flush existing commands and release the current context
	uint64_t Finish(bool WaitForCompletion = false);

	ID3D12GraphicsCommandList* GetCommandList() {
		return m_CommandList;
	}

	void SetGraphicsRootSignature(GFXRootSignature* rs)
	{
		ID3D12RootSignature* dxRS = rs ? ((ID3D12RootSignature*)rs->mNativeHandle) : nullptr;
		m_CommandList->SetGraphicsRootSignature(dxRS);
		
	}
	void SetComputeRootSignature(GFXRootSignature* rs)
	{
		ID3D12RootSignature* dxRS = rs ? ((ID3D12RootSignature*)rs->mNativeHandle) : nullptr;
		m_CommandList->SetComputeRootSignature(dxRS);
	}
	void SetSetDescriptorHeaps(ID3D12DescriptorHeap* pCSUHeap, ID3D12DescriptorHeap* pSamplerHeap)
	{
		ID3D12DescriptorHeap* dxHeaps[2] = { pCSUHeap, pSamplerHeap };
		m_CommandList->SetDescriptorHeaps(2, dxHeaps);
	}
	void SetPipelineState(GFXPiplineState* ps)
	{
		ID3D12PipelineState* dxPS = ps ? (ID3D12PipelineState*)ps->mNativeHandle : nullptr;
		m_CommandList->SetPipelineState(dxPS);
	}
	void SetPrimitiveTopology(EPrimitiveTopology pt)
	{
		m_CommandList->IASetPrimitiveTopology(UToDX12(pt));
	}
	//All viewports must be set atomically as one operation.Any viewports not defined by the call are disabled. 
	//Which viewport to use is determined by the SV_ViewportArrayIndex semantic output by a geometry shader.
	//If a geometry shader does not make use of the SV_ViewportArrayIndex semantic then Direct3D will use the first viewport in the array.
	void SetViewports(unsigned numViewports, const D3D12_VIEWPORT* pViewports)
	{
		m_CommandList->RSSetViewports(numViewports, pViewports);
	}
	void SetScissors(unsigned numRects, const D3D12_RECT* pRects)
	{
		m_CommandList->RSSetScissorRects(numRects, pRects);
	}
	void SetViewport(float x, float y, float w, float h, float minDepth = 0, float maxDepth = 1)
	{
		D3D12_VIEWPORT viewport = { x, y, w, h, minDepth, maxDepth };
		m_CommandList->RSSetViewports(1, &viewport);
	}
	void SetScissor(unsigned x, unsigned y, unsigned w, unsigned h)
	{
		D3D12_RECT rect = { x, y, x + w, y + h };
		m_CommandList->RSSetScissorRects(1, &rect);
	}
	void DrawInstanced(unsigned VertexCountPerInstance, unsigned InstanceCount, unsigned StartVertexLocation, unsigned StartInstanceLocation)
	{
		m_CommandList->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
	}

	void DrawIndexedInstanced(unsigned IndexCountPerInstance, unsigned InstanceCount, unsigned StartIndexLocation, int BaseVertexLocation, unsigned StartInstanceLocation)
	{
		m_CommandList->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
	}

	void Dispatch(unsigned ThreadGroupCountX, unsigned ThreadGroupCountY, unsigned ThreadGroupCountZ)
	{
		m_CommandList->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
	}
	void SetIndexBuffer(D3D12_GPU_VIRTUAL_ADDRESS bufferLocation, unsigned sizeInBytes, DXGI_FORMAT format)
	{
		D3D12_INDEX_BUFFER_VIEW ibv = { bufferLocation, sizeInBytes, format};
		m_CommandList->IASetIndexBuffer(&ibv);
	}
	void SetVertexBuffers(unsigned startSlot, unsigned numViews, const D3D12_VERTEX_BUFFER_VIEW* pViews)
	{
		m_CommandList->IASetVertexBuffers(startSlot, numViews, pViews);
	}
	void SetVertexBuffer(unsigned slot, D3D12_VERTEX_BUFFER_VIEW view)
	{
		m_CommandList->IASetVertexBuffers(slot, 1, &view);
	}
	//////////////////////////////////////////////////////////////////////////
	void SetGraphicsRoot32BitConstants(unsigned RootParameterIndex, unsigned Num32BitValuesToSet, const void* pSrcData, unsigned DestOffsetIn32BitValues = 0)
	{
		m_CommandList->SetGraphicsRoot32BitConstants(RootParameterIndex, Num32BitValuesToSet, pSrcData, DestOffsetIn32BitValues);
	}
	void SetGraphicsRoot32BitConstant(unsigned RootParameterIndex, unsigned SrcData, unsigned DestOffsetIn32BitValues)
	{
		m_CommandList->SetGraphicsRoot32BitConstant(RootParameterIndex, SrcData, DestOffsetIn32BitValues);
	}
	//////////////////////////////////////////////////////////////////////////
	void SetGraphicsRootDescriptorTable(unsigned RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor)
	{
		m_CommandList->SetGraphicsRootDescriptorTable(RootParameterIndex, baseDescriptor);
	}
	void SetGraphicsRootDescriptorTable(unsigned RootParameterIndex, DescHandleBase baseDescriptor)
	{
		UASSERT(baseDescriptor.IsShaderVisible());
		m_CommandList->SetGraphicsRootDescriptorTable(RootParameterIndex, baseDescriptor.GetGpuHandle());
	}
	//////////////////////////////////////////////////////////////////////////
	void SetGraphicsRootCBV(unsigned rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation)
	{
		m_CommandList->SetGraphicsRootConstantBufferView(rootParameterIndex, bufferLocation);
	}
	void SetGraphicsRootCBV(unsigned rootParameterIndex, const GFXResource* buffer)
	{
		auto bl = ((ID3D12Resource*)buffer->mNativeHandle)->GetGPUVirtualAddress();
		m_CommandList->SetGraphicsRootConstantBufferView(rootParameterIndex, bl);
	}
	//////////////////////////////////////////////////////////////////////////
	void SetGraphicsRootSRV(unsigned rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation)
	{
		m_CommandList->SetGraphicsRootShaderResourceView(rootParameterIndex, bufferLocation);
	}
	void SetGraphicsRootSRV(unsigned rootParameterIndex, const GFXResource* buffer)
	{
		auto bl = ((ID3D12Resource*)buffer->mNativeHandle)->GetGPUVirtualAddress();
		m_CommandList->SetGraphicsRootShaderResourceView(rootParameterIndex, bl);
	}
	//////////////////////////////////////////////////////////////////////////
	void SetGraphicsRootUAV(unsigned rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS bufferLocation)
	{
		m_CommandList->SetGraphicsRootUnorderedAccessView(rootParameterIndex, bufferLocation);
	}
	void SetGraphicsRootUAV(unsigned rootParameterIndex, const GFXResource* buffer)
	{
		auto bl = ((ID3D12Resource*)buffer->mNativeHandle)->GetGPUVirtualAddress();
		m_CommandList->SetGraphicsRootUnorderedAccessView(rootParameterIndex, bl);
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceBarrier(unsigned numBarriers, D3D12_RESOURCE_BARRIER* barriers)
	{
		m_CommandList->ResourceBarrier(numBarriers, barriers);
	}
	template<unsigned numBarriers> void ResourceBarrier(D3D12_RESOURCE_BARRIER(&barriers)[numBarriers])
	{
		this->ResourceBarrier(numBarriers, barriers);
	}
	//////////////////////////////////////////////////////////////////////////
	void SetRenderTargets(unsigned numRenderTraget, DescHandleRTV renderTargetBase, DescHandleDSV depthStencil)
	{
		m_CommandList->OMSetRenderTargets(numRenderTraget, numRenderTraget ? &(renderTargetBase.GetCpuHandle()) : nullptr, true
			, depthStencil.IsNull() ? nullptr : &(depthStencil.GetCpuHandle()));
	}
	void SetRenderTargets(unsigned numRenderTraget, const DescHandleRTV* renderTargets, DescHandleDSV depthStencil)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handles[8];
		for (unsigned i = 0; i < numRenderTraget; i++)
			handles[i] = renderTargets[i].GetCpuHandle();

		m_CommandList->OMSetRenderTargets(numRenderTraget, numRenderTraget ? handles : nullptr, false
			, depthStencil.IsNull() ? nullptr : &(depthStencil.GetCpuHandle()));

	}
	
	//////////////////////////////////////////////////////////////////////////
	void ClearRTV(DescHandleRTV rtv, const Color& clearColor)
	{
		m_CommandList->ClearRenderTargetView(rtv, (float*)&clearColor, 0, nullptr);
	}
	
	//////////////////////////////////////////////////////////////////////////
protected:

	void BindDescriptorHeaps(void);

	ID3D12GraphicsCommandList* m_CommandList;
	ID3D12CommandAllocator* m_CurrentAllocator;

// 	ID3D12RootSignature* m_CurGraphicsRootSignature;
// 	ID3D12PipelineState* m_CurGraphicsPipelineState;
// 	ID3D12RootSignature* m_CurComputeRootSignature;
// 	ID3D12PipelineState* m_CurComputePipelineState;

	D3D12_RESOURCE_BARRIER m_ResourceBarrierBuffer[16];
	UINT m_NumBarriersToFlush;

	D3D12_COMMAND_LIST_TYPE m_Type;

	QueueMgr* mQueueMgr = nullptr;
};


//////////////////////////////////////////////////////////////////////////
struct UGFXDIRECTX_API CmdQueue
{
	CmdQueue(D3D12_COMMAND_LIST_TYPE type);
	~CmdQueue() { Shutdown(); }

	void Initilize(ID3D12Device* pDevice);
	void Shutdown();


	uint64 IncrementFence();
	bool IsFenceComplete(uint64 fenceValue);
	void StallForFence(uint64 fenceValue);
	void StallForProducer(CmdQueue& producer);
	void WaitForFence(uint64 fenceValue);
	void WaitForIdle(void) { WaitForFence(IncrementFence()); }

	ID3D12CommandQueue* GetCommandQueue() { return m_CommandQueue; }
	uint64 GetNextFenceValue() { return m_NextFenceValue; }


	uint64 ExecuteCommandList(ID3D12CommandList* List);
	ID3D12CommandAllocator* RequestAllocator(void);
	void DiscardAllocator(uint64 FenceValueForReset, ID3D12CommandAllocator* Allocator);

	ID3D12CommandQueue* m_CommandQueue;
	CommandAllocatorPool m_AllocatorPool;
	StdLock m_FenceMutex;
	StdLock m_EventMutex;
	const D3D12_COMMAND_LIST_TYPE m_Type;
	// Lifetime of these objects is managed by the descriptor cache
	ID3D12Fence* m_pFence;
	uint64_t m_NextFenceValue;
	uint64_t m_LastCompletedFenceValue;
	HANDLE m_FenceEventHandle;
};

//////////////////////////////////////////////////////////////////////////
class UGFXDIRECTX_API QueueMgr
{
	CmdQueue mCopyQueue;
	CmdQueue mGraphicQueue;
	CmdQueue mComputeQueue;
	GFXContextDX12* mCtx;

public:
	QueueMgr();
	~QueueMgr() { Shutdown(); }

	void Initilize(GFXContextDX12* ctx);
	void Shutdown();

	CmdQueue& GetCopyQueue() { return mCopyQueue; }
	CmdQueue& GetGraphicQueue() { return mGraphicQueue; }
	CmdQueue& GetComputeQueue() { return mComputeQueue; }

	//the CPU will wait for all the queues to be finished
	void WaitForIdle();

	CmdList* AllocCmdList(D3D12_COMMAND_LIST_TYPE Type);
	
	//call finish to end the list
	CmdList* GetNewGraphicCmdList() { return AllocCmdList(D3D12_COMMAND_LIST_TYPE_DIRECT); }
	CmdList* GetNewComputeCmdList() { return AllocCmdList(D3D12_COMMAND_LIST_TYPE_COMPUTE); }
	
	//return queue by type
	CmdQueue& GetQueue(D3D12_COMMAND_LIST_TYPE);

	void FreeCmdList(CmdList* usedList);

	std::vector<CmdList*> sm_ContextPool[4];
	std::queue<CmdList*> sm_AvailableContexts[4];
	std::mutex sm_ContextAllocationMutex;
};


};
