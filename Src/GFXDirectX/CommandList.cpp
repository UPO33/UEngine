#include "CommandList.h"
#include "Context.h"

namespace UGFX
{






//////////////////////////////////////////////////////////////////////////
void CommandAllocatorPool::Initilize(ID3D12Device * pDevice)
{
	m_Device = pDevice;
}
//////////////////////////////////////////////////////////////////////////
void CommandAllocatorPool::Shutdown()
{
	for (size_t i = 0; i < m_AllocatorPool.Length(); i++)
		m_AllocatorPool[i]->Release();

	m_AllocatorPool.Empty();
}
//////////////////////////////////////////////////////////////////////////
ID3D12CommandAllocator * CommandAllocatorPool::RequestAllocator(uint64_t CompletedFenceValue)
{
	USCOPE_LOCK(m_AllocatorMutex);

	ID3D12CommandAllocator* pAllocator = nullptr;

	if (!m_ReadyAllocators.empty())
	{
		std::pair<uint64_t, ID3D12CommandAllocator*>& AllocatorPair = m_ReadyAllocators.front();

		if (AllocatorPair.first <= CompletedFenceValue)
		{
			pAllocator = AllocatorPair.second;
			UCHECK_DXRET(pAllocator->Reset());
			m_ReadyAllocators.pop();
		}
	}

	// If no allocator's were ready to be reused, create a new one
	if (pAllocator == nullptr)
	{
		UCHECK_DXRET(m_Device->CreateCommandAllocator(mType, IID_PPV_ARGS(&pAllocator)));
		wchar_t AllocatorName[32];
		swprintf(AllocatorName, 32, L"CommandAllocator %zu", m_AllocatorPool.Length());
		pAllocator->SetName(AllocatorName);
		m_AllocatorPool.Add(pAllocator);
	}

	return pAllocator;
}
//////////////////////////////////////////////////////////////////////////
void CommandAllocatorPool::DiscardAllocator(uint64_t FenceValue, ID3D12CommandAllocator * Allocator)
{
	USCOPE_LOCK(m_AllocatorMutex);

	// That fence value indicates we are free to reset the allocator
	m_ReadyAllocators.push(std::make_pair(FenceValue, Allocator));
}

//////////////////////////////////////////////////////////////////////////
QueueMgr::QueueMgr() :
	mGraphicQueue(D3D12_COMMAND_LIST_TYPE_DIRECT),
	mComputeQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE),
	mCopyQueue(D3D12_COMMAND_LIST_TYPE_COPY)
{

}

//////////////////////////////////////////////////////////////////////////
void QueueMgr::Initilize(GFXContextDX12* ctx)
{
	mCtx = ctx;
	mCopyQueue.Initilize(ctx->mDevice);
	mComputeQueue.Initilize(ctx->mDevice);
	mGraphicQueue.Initilize(ctx->mDevice);
}
//////////////////////////////////////////////////////////////////////////
void QueueMgr::Shutdown()
{
	WaitForIdle();
	mGraphicQueue.Shutdown();
	mComputeQueue.Shutdown();
	mCopyQueue.Shutdown();
}
//////////////////////////////////////////////////////////////////////////
void QueueMgr::WaitForIdle()
{
	mCopyQueue.WaitForIdle();
	mComputeQueue.WaitForIdle();
	mGraphicQueue.WaitForIdle();
}

UGFX::CmdList* QueueMgr::AllocCmdList(D3D12_COMMAND_LIST_TYPE Type)
{
	std::lock_guard<std::mutex> LockGuard(sm_ContextAllocationMutex);

	auto& AvailableContexts = sm_AvailableContexts[Type];

	UASSERT(Type != D3D12_COMMAND_LIST_TYPE_BUNDLE, "Bundles are not supported");

	CmdList* ret = nullptr;
	if (AvailableContexts.empty())
	{
		ID3D12CommandAllocator* allocator = nullptr;
		ID3D12GraphicsCommandList* newCmdList = nullptr;

		switch (Type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT: allocator = mGraphicQueue.RequestAllocator(); break;
		case D3D12_COMMAND_LIST_TYPE_BUNDLE: break;
		case D3D12_COMMAND_LIST_TYPE_COMPUTE: allocator = mComputeQueue.RequestAllocator(); break;
		case D3D12_COMMAND_LIST_TYPE_COPY: allocator = mCopyQueue.RequestAllocator(); break;
		}

		UCHECK_DXRET(this->mCtx->mDevice->CreateCommandList(0, Type, allocator, nullptr, IID_PPV_ARGS(&newCmdList)));
		newCmdList->SetName(L"CommandList");

		ret = new CmdList(this, Type, newCmdList, allocator);
		sm_ContextPool[Type].emplace_back(ret);

	}
	else
	{
		ret = AvailableContexts.front();
		AvailableContexts.pop();
		ret->Reset();
	}

	UASSERT(ret != nullptr);
	UASSERT(ret->m_Type == Type);

	return ret;
}

CmdQueue& QueueMgr::GetQueue(D3D12_COMMAND_LIST_TYPE type)
{
	switch (type)
	{
	case D3D12_COMMAND_LIST_TYPE_DIRECT: return mGraphicQueue;
		break;
	case D3D12_COMMAND_LIST_TYPE_COMPUTE: return mComputeQueue;
		break;
	case D3D12_COMMAND_LIST_TYPE_COPY: return mCopyQueue;
		break;
	default:
		break;
	}
	UASSERT(false);
	return mGraphicQueue;
}

//////////////////////////////////////////////////////////////////////////
void QueueMgr::FreeCmdList(CmdList* usedList)
{
	UASSERT(usedList != nullptr);
	std::lock_guard<std::mutex> LockGuard(sm_ContextAllocationMutex);
	sm_AvailableContexts[usedList->m_Type].push(usedList);
}

//////////////////////////////////////////////////////////////////////////
CmdQueue::CmdQueue(D3D12_COMMAND_LIST_TYPE Type) :
	m_Type(Type),
	m_CommandQueue(nullptr),
	m_pFence(nullptr),
	m_NextFenceValue((uint64_t)Type << 56 | 1),
	m_LastCompletedFenceValue((uint64_t)Type << 56),
	m_AllocatorPool(Type)
{

}
//////////////////////////////////////////////////////////////////////////
void CmdQueue::Initilize(ID3D12Device* pDevice)
{
	UASSERT(pDevice != nullptr);
	UASSERT(m_AllocatorPool.Size() == 0);

	D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
	QueueDesc.Type = m_Type;
	QueueDesc.NodeMask = 1;
	UCHECK_DXRET(pDevice->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&m_CommandQueue)));

	UCHECK_DXRET(pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence)));
	m_pFence->SetName(L"CmdQueue::mFence");
	m_pFence->Signal((uint64_t)m_Type << 56);

	m_FenceEventHandle = CreateEventA(nullptr, false, false, "CmdQueue::mFenceEventHandle");
	UASSERT(m_FenceEventHandle != INVALID_HANDLE_VALUE);

	m_AllocatorPool.Initilize(pDevice);
}
//////////////////////////////////////////////////////////////////////////
void CmdQueue::Shutdown()
{
	if (m_CommandQueue == nullptr)
		return;

	m_AllocatorPool.Shutdown();

	CloseHandle(m_FenceEventHandle);

	m_pFence->Release();
	m_pFence = nullptr;

	m_CommandQueue->Release();
	m_CommandQueue = nullptr;
}

uint64 CmdQueue::IncrementFence()
{
	USCOPE_LOCK(m_FenceMutex);

	m_CommandQueue->Signal(m_pFence, m_NextFenceValue);
	return m_NextFenceValue++;
}

bool CmdQueue::IsFenceComplete(uint64 FenceValue)
{
	// Avoid querying the fence value by testing against the last one seen.
	// The max() is to protect against an unlikely race condition that could cause the last
	// completed fence value to regress.
	if (FenceValue > m_LastCompletedFenceValue)
		m_LastCompletedFenceValue = Max(m_LastCompletedFenceValue, m_pFence->GetCompletedValue());

	return FenceValue <= m_LastCompletedFenceValue;
}

void CmdQueue::StallForProducer(CmdQueue& Producer)
{
	UASSERT(Producer.m_NextFenceValue > 0);
	m_CommandQueue->Wait(Producer.m_pFence, Producer.m_NextFenceValue - 1);

}

void CmdQueue::WaitForFence(uint64 FenceValue)
{
	if (IsFenceComplete(FenceValue))
		return;

	// TODO:  Think about how this might affect a multi-threaded situation.  Suppose thread A
	// wants to wait for fence 100, then thread B comes along and wants to wait for 99.  If
	// the fence can only have one event set on completion, then thread B has to wait for 
	// 100 before it knows 99 is ready.  Maybe insert sequential events?
	{
		USCOPE_LOCK(m_EventMutex);

		m_pFence->SetEventOnCompletion(FenceValue, m_FenceEventHandle);
		WaitForSingleObject(m_FenceEventHandle, INFINITE);
		m_LastCompletedFenceValue = FenceValue;
	}
}


uint64 CmdQueue::ExecuteCommandList(ID3D12CommandList* List)
{
	USCOPE_LOCK(m_FenceMutex);

	UCHECK_DXRET(((ID3D12GraphicsCommandList*)List)->Close());

	// Kickoff the command list
	m_CommandQueue->ExecuteCommandLists(1, &List);

	// Signal the next fence value (with the GPU)
	m_CommandQueue->Signal(m_pFence, m_NextFenceValue);

	// And increment the fence value.  
	return m_NextFenceValue++;
}

ID3D12CommandAllocator* CmdQueue::RequestAllocator(void)
{
	uint64_t CompletedFence = m_pFence->GetCompletedValue();

	return m_AllocatorPool.RequestAllocator(CompletedFence);
}

void CmdQueue::DiscardAllocator(uint64 FenceValueForReset, ID3D12CommandAllocator* Allocator)
{
	m_AllocatorPool.DiscardAllocator(FenceValueForReset, Allocator);

}

void CmdList::Reset(void)
{
	// We only call Reset() on previously freed contexts.  The command list persists, but we must
	// request a new allocator.
	UASSERT(m_CommandList != nullptr && m_CurrentAllocator == nullptr);
	m_CurrentAllocator = mQueueMgr->GetQueue(m_Type).RequestAllocator();
	m_CommandList->Reset(m_CurrentAllocator, nullptr);
}



uint64_t CmdList::Finish(bool WaitForCompletion /*= false*/)
{
// 	ASSERT(m_Type == D3D12_COMMAND_LIST_TYPE_DIRECT || m_Type == D3D12_COMMAND_LIST_TYPE_COMPUTE);
// 
// 	FlushResourceBarriers();
// 
// 	if (m_ID.length() > 0)
// 		EngineProfiling::EndBlock(this);

	UASSERT(m_CurrentAllocator != nullptr);

	uint64_t FenceValue = mQueueMgr->GetQueue(m_Type).ExecuteCommandList(m_CommandList);
	mQueueMgr->GetQueue(m_Type).DiscardAllocator(FenceValue, m_CurrentAllocator);
	m_CurrentAllocator = nullptr;

	if (WaitForCompletion)
		mQueueMgr->GetQueue(m_Type).WaitForFence(FenceValue);

	mQueueMgr->FreeCmdList(this);

	
	

// 	uint64_t FenceValue = Queue.ExecuteCommandList(m_CommandList);
// 	Queue.DiscardAllocator(FenceValue, m_CurrentAllocator);
// 	m_CurrentAllocator = nullptr;
// 
// 	m_CpuLinearAllocator.CleanupUsedPages(FenceValue);
// 	m_GpuLinearAllocator.CleanupUsedPages(FenceValue);
// 	m_DynamicViewDescriptorHeap.CleanupUsedHeaps(FenceValue);
// 	m_DynamicSamplerDescriptorHeap.CleanupUsedHeaps(FenceValue);
// 
// 	if (WaitForCompletion)
// 		g_CommandManager.WaitForFence(FenceValue);
// 
// 	g_ContextManager.FreeContext(this);

	return FenceValue;
}

};
