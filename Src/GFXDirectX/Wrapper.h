#pragma once

#include "Base.h"
#include "../Core/Threading.h"

#define IIDHANDLE(type, handle) __uuidof(type), reinterpret_cast<void**>(&handle.mPtr)
#define UCHECK_DXRET(hr) UASSERT(SUCCEEDED(hr))

#define UWRAPPER_HRCHECK(HR) UASSERT(SUCCEEDED(HR))

namespace UGFX
{
	typedef D3D12_RECT Rect;

	
	
	//////////////////////////////////////////////////////////////////////////
	struct HBaseDX12
	{
		void* mPtr = nullptr;

		ID3D12Object* Get() const { return reinterpret_cast<ID3D12Object*>(mPtr); }
		void SetName(const wchar_t* in)
		{
			UCHECK_DXRET(Get()->SetName(in));
		}
	};

	//////////////////////////////////////////////////////////////////////////
	struct HCmdAllocatorDX12 : public HBaseDX12
	{
		ID3D12CommandAllocator* Get() const { return reinterpret_cast<ID3D12CommandAllocator*>(mPtr); }

		/*
		Indicates to re-use the memory that is associated with the command allocator.
		Apps call Reset to reuse the memory that is associated with a command allocator.From this call to Reset, the runtime and driver determine
		  that the GPU is no longer executing any command lists that have recorded commands with the command allocator.
		Unlike ID3D12GraphicsCommandList::Reset, it is not recommended that you call Reset on the command allocator while a command list is still being executed.
		*/
		void Reset() 
		{
			UCHECK_DXRET(Get()->Reset());
		}
	};
	//////////////////////////////////////////////////////////////////////////
	struct HResourceDX12 : public HBaseDX12
	{
		ID3D12Resource* Get() const { return reinterpret_cast<ID3D12Resource*>(mPtr); }
	};
	//////////////////////////////////////////////////////////////////////////
	//Represents the state of all currently set shaders as well as certain fixed function state objects.
	struct HPipelineStateDX12 : public HBaseDX12
	{
		ID3D12PipelineState* Get() const { return reinterpret_cast<ID3D12PipelineState*>(mPtr); }
	};

	//////////////////////////////////////////////////////////////////////////
	struct HQueryHeapDX12
	{
		ID3D12QueryHeap*	mPtr;

		ID3D12QueryHeap* Get() const { return mPtr; }
	};
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	struct HDescriptorHeapDX12 : HBaseDX12
	{
		ID3D12DescriptorHeap* Get() { return reinterpret_cast<ID3D12DescriptorHeap*>(mPtr); }
	};


	//////////////////////////////////////////////////////////////////////////
	struct HCmdListDX12 : public HBaseDX12
	{
		ID3D12CommandList* Get() const { return reinterpret_cast<ID3D12CommandList*>(mPtr); }
	};
	
	/*
	Encapsulates a list of graphics commands for rendering. Includes APIs for instrumenting the command list execution,
	and for setting and clearing the pipeline state.
	Bundles cannot be used with compute or copy command lists or queues.

	Copy and compute command lists can use the following methods:
		Close
		CopyBufferRegion
		CopyResource
		CopyTextureRegion
		CopyTiles
		Reset
		ResourceBarrier

	Compute command lists can also use the following methods:
		ClearState
		ClearUnorderedAccessViewFloat
		ClearUnorderedAccessViewUint
		DiscardResource
		Dispatch
		ExecuteIndirect
		SetComputeRoot32BitConstant
		SetComputeRoot32BitConstants
		SetComputeRootConstantBufferView
		SetComputeRootDescriptorTable
		SetComputeRootShaderResourceView
		SetComputeRootSignature
		SetComputeRootUnorderedAccessView
		SetDescriptorHeaps
		SetPipelineState
		SetPredication

	*/
	struct HGFXCmdListDX12 : HCmdListDX12
	{
		ID3D12GraphicsCommandList* Get() { return reinterpret_cast<ID3D12GraphicsCommandList*>(mPtr); }

		//
		void CopyTextureRegion(HResourceDX12 dst, unsigned dstX, unsigned dstY, unsigned dstZ, HResourceDX12 src,
			unsigned regionX, unsigned regionW, unsigned regionY, unsigned regionH, unsigned regionZ = 0, unsigned regionD = 1)
		{
			D3D12_BOX box =
			{
				regionX, regionY, regionZ,
				regionX + regionW,
				regionY + regionH,
				regionZ + regionD
			};
			Get()->CopyTextureRegion(&CD3DX12_TEXTURE_COPY_LOCATION(dst.Get()), dstX, dstY, dstZ,
				&CD3DX12_TEXTURE_COPY_LOCATION(src.Get()), &box);
		}
		//Copies the entire contents of the source resource to the destination resource. 
		void CopyResource(HResourceDX12 dst /*must be copy dst*/, HResourceDX12 src /*must be copy src*/)
		{
			Get()->CopyResource(dst.Get(), src.Get());
		}
		//Copies a region of a buffer from one resource to another.
		//Consider using the CopyResource method when copying an entire resource, and use this method for 
		void CopyBufferRegion(HResourceDX12 dst, uint64 dstOffset, HResourceDX12 src, uint64 srcOffset, uint64 numBytes)
		{
			Get()->CopyBufferRegion(dst.Get(), dstOffset, src.Get(), srcOffset, numBytes);
		}


		//Resets a command list back to its initial state as if a new command list was just created. 
		//By using Reset, you can re-use command list tracking structures without any allocations. 
		//Unlike HCmdAllocatorDX12::Reset, you can call Reset while the command list is still being executed. 
		//A typical pattern is to submit a command list and then immediately reset it to reuse the allocated memory for another command list. 
		//You can use Reset for both direct command lists and bundles.
		//The command allocator that Reset takes as input can be associated with no more than one recording command list at a time.
		//The allocator type, direct command list or bundle, must match the type of command list that is being created.
		//If a bundle doesn't specify a resource heap, it can't make changes to which descriptor tables are bound.Either way, bundles can't change 
		//	the resource heap within the bundle. If a heap is specified for a bundle, the heap must match the calling 'parent' command list’s heap. 
		//Before an app calls Reset, the command list must be in the "closed" state.
		//After Reset succeeds, the command list is left in the "recording" state. Reset will fail if it would cause the maximum concurrently 
		//	recording command list limit which is specified at device creation, to be exceeded. 
		void Reset(HCmdAllocatorDX12 allocator, HPipelineStateDX12 optInitialState)
		{
			Get()->Reset(allocator.Get(), optInitialState.Get());
		}
		//Indicates that recording to the command list has finished. 
		void Close()
		{
			Get()->Close();
		}

		//@param VertexCountPerInstance		Number of vertices to draw. 
		//@param InstanceCount				Number of instances to draw. 
		//@param StartVertexLocation		Index of the first vertex.
		//@param StartInstanceLocation		A value added to each index before reading per-instance data from a vertex buffer. 
		void DrawInstanced(UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation,UINT StartInstanceLocation)
		{
			Get()->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
		}
		//@param IndexCountPerInstance		Number of indices read from the index buffer for each instance.
		//@param InstanceCount				Number of instances to draw.
		//@param StartIndexLocation			The location of the first index read by the GPU from the index buffer.
		//@param BaseVertexLocation			A value added to each index before reading a vertex from the vertex buffer.
		//@param StartInstanceLocation		A value added to each index before reading per-instance data from a vertex buffer.
		void DrawIndexedInstanced(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation)
		{
			Get()->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
		}
		//execute commands in a compute shader. A compute shader can be run on many threads in parallel, within a thread group. 
		//Index a particular thread, within a thread group using a 3D vector given by (x,y,z). 
		void Dispatch(unsigned ThreadGroupCountX, unsigned ThreadGroupCountY, unsigned ThreadGroupCountZ)
		{
			Get()->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
		}
		//Resets the state of a direct command list back to the state it was in when the command list was created.
		//It is invalid to call ClearState on a bundle.
		//all currently bound resources are unbound. The primitive topology is set to undefined. Viewports, scissor rectangles, stencil reference value
		//and the blend factor are set to empty values (all zeros). Predication is disabled.
		//The provided pipeline state object becomes bound as the currently set pipeline state object.
		void ClearState(HPipelineStateDX12 optInitialState)
		{
			Get()->ClearState(optInitialState.Get());
		}

		//Bundles inherit all state from the parent command list on which ExecuteBundle is called, except the pipeline state object and primitive topology.
		//All of the state that is set in a bundle will affect the state of the parent command list. Note that ExecuteBundle is not a predicated operation. 
		void ExecuteBundle(HGFXCmdListDX12 bundle)
		{
			Get()->ExecuteBundle(bundle.Get());
		}
		//Starts a query running.
		//there are 3 states that a query can be in 
		//	-Inactive(this is the initial state of all queries)
		//	-Querying
		//	-Predicating
		//BeginQuery transitions a query from the inactive state to the querying state. EndQuery transitions a query from the querying 
		//	state to the inactive state. 
		//@param index		Specifies the index of the query within the query heap. 
		void BeginQuery(HQueryHeapDX12 queryHeap, EGFXQueryType queryType, unsigned index)
		{
			Get()->BeginQuery(queryHeap.Get(), UToDX12(queryType), index);
		}
		//@param index		Specifies the index of the query within the query heap. 
		void EndQuery(HQueryHeapDX12 queryHeap, EGFXQueryType queryType, unsigned index)
		{
			Get()->EndQuery(queryHeap.Get(), UToDX12(queryType), index);
		}
		//Sets a rendering predicate
		//denote that subsequent rendering and resource manipulation commands are not actually performed if the resulting predicate data of the predicate 
		//	is equal to the operation specified. However, some predicates are only hints, so they may not actually prevent operations from being performed. 
		//@param equalZero	true means if resulting predicate is zero don't perform, false means if resulting predicate is not zero don't perform
		//Unlike Direct3D 11,  predication state is not inherited by direct command lists. All direct command lists begin with predication disabled.
		//Bundles do inherit predication state. It is legal for the same predicate to be bound multiple times. 
		void SetPridection(HResourceDX12 optResource, uint64 alignedBufferOffset, bool equalZero)
		{
			Get()->SetPredication(optResource.Get(), alignedBufferOffset, equalZero ? D3D12_PREDICATION_OP_EQUAL_ZERO : D3D12_PREDICATION_OP_NOT_EQUAL_ZERO);
		}
		//clear a render target view
		void ClearRenderTargetView(size_t renderTargetViewCPUHandle, float* colorRGBA, unsigned numRects = 0, const Rect* rects = nullptr)
		{
			static_assert(sizeof(Rect) == sizeof(D3D12_RECT), "");
			Get()->ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE { renderTargetViewCPUHandle }, colorRGBA, numRects, (const D3D12_RECT*)rects);
		}
		//clear a depth stencil view
		void ClearDepthStencilView(size_t depthStencilViewCPUHandle, bool clearDepth, bool clearStencil, float depth, uint8 stencil, unsigned numRects = 0, const Rect* rects = nullptr)
		{
			static_assert(sizeof(Rect) == sizeof(D3D12_RECT), "");
			D3D12_CLEAR_FLAGS clearFlag = (D3D12_CLEAR_FLAGS)0;
			if (clearDepth) clearFlag |= D3D12_CLEAR_FLAG_DEPTH;
			if (clearStencil) clearFlag |= D3D12_CLEAR_FLAG_STENCIL;
			Get()->ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE{ depthStencilViewCPUHandle }, clearFlag, depth, stencil, numRects, (const D3D12_RECT*)rects);
		}
		
		void SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* pView)
		{
			Get()->IASetIndexBuffer(pView);
		}
		void SetVertexBuffers(unsigned startSlot, unsigned numView, const D3D12_VERTEX_BUFFER_VIEW* pViews)
		{
			Get()->IASetVertexBuffers(startSlot, numView, pViews);
		}
		void SetPrimitiveTopology(EPrimitiveTopology topology)
		{
			Get()->IASetPrimitiveTopology(UToDX12(topology));
		}
		//the types of descriptor heap that can be set on a command list are 
		//D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV and D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER
		// the heaps being set on the command list must have been created as shader visible
		void SetDescriptorHeaps(HDescriptorHeapDX12 heapCBV_SRV_UAV, HDescriptorHeapDX12 heapSampler)
		{
			ID3D12DescriptorHeap* heaps[] = { heapCBV_SRV_UAV.Get(), heapSampler.Get() };
			Get()->SetDescriptorHeaps(2, heaps);
		}
	};

	//Represents a fence, an object used for synchronization of the CPU and one or more GPUs.
	struct HFenceDX12 : public HBaseDX12
	{
		ID3D12Fence* Get() { return reinterpret_cast<ID3D12Fence*>(mPtr); }

		//Sets the fence to the specified value.
		//Use this method to set a fence value from the CPU side. Use ID3D12CommandQueue::Signal to set a fence from the GPU side.
		void Signal(uint64 value)
		{
			Get()->Signal(value);
		}
		//Gets the current value of the fence.
		uint64 GetCompletedValue()
		{
			 return Get()->GetCompletedValue();
		}
		//Specifies an event that should be fired when the fence reaches a certain value.
		//@param value		The fence value when the event is to be signaled.
		//@param event		A handle to the event object.
		/*
		example:
			// Wait for the command list to execute; we are reusing the same command
			// list in our main loop but for now, we just want to wait for setup to
			// complete before continuing.

			// Signal and increment the fence value.
			const UINT64 fenceToWaitFor = m_fenceValue;
			ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), fenceToWaitFor));
			m_fenceValue++;

			// Wait until the fence is completed.
			ThrowIfFailed(m_fence->SetEventOnCompletion(fenceToWaitFor, m_fenceEvent));
			WaitForSingleObject(m_fenceEvent, INFINITE);
		*/
		void SetEventOnCompletion(uint64 value, HANDLE event)
		{
			Get()->SetEventOnCompletion(value, event);
		}
	};

	//////////////////////////////////////////////////////////////////////////
	//Provides methods for submitting command lists, synchronizing command list execution, instrumenting the command queue, and updating resource tile mappings.
	//Queues of all types (3D, compute and copy) share the same interface and are all command-list based.
	//Resource mapping operations remain on the queue interface, but are only allowed on 3D and compute queues (not copy).
	struct HCmdQueueDX12 : public HBaseDX12
	{
		ID3D12CommandQueue* Get() const { return reinterpret_cast<ID3D12CommandQueue*>(mPtr); }

		//Submits an array of command lists for execution.
		//
		//Bundles can't be submitted to a command queue directly. If a bundle is passed to this method, the runtime will drop the call.
		//The runtime will also drop the call if the Close has not been called on any of the command lists.
		//The runtime will detect if the command allocators associated with the command lists have been reset after Close was called.
		//	The runtime will drop the call and remove the device in this situation.
		//The runtime will drop the call and remove the device if the command queue fence indicates that a previous execution 
		//	of any of the command lists has not yet completed.
		//The runtime will validate the "before" and "after" states of resource transition barriers inside of ExecuteCommandLists.
		//If the 'before' state of a transition does not match up with the 'after' state of a previous transition,
		//	then the runtime will drop the call and remove the device.
		//The runtime will validate the 'before' and 'after' states of queries used by the command lists.
		//	If an error is detected, then the runtime will drop the call and remove the device.
		//
		//The driver is free to patch the submitted command lists. It is the calling application's responsibility to ensure 
		//	that the GPU is not currently reading the any of the submitted command lists from a previous execution.
		//Applications are encouraged to batch together command list executions to reduce fixed costs associated with submitted commands to the GPU.
		void ExecuteCommandLists(unsigned numCommandList, const HCmdListDX12* commandLists)
		{
			static_assert(sizeof(HCmdListDX12) == sizeof(void*), "");
			Get()->ExecuteCommandLists(numCommandList, (ID3D12CommandList**)commandLists);
		}

		//Updates a fence to a specified value.
		//@param value	The value to set the fence to.
		void Signal(HFenceDX12 fance, uint64 value)
		{
			Get()->Signal(fance.Get(), value);
		}
		//Waits until the specified fence reaches or exceeds the specified value.
		//@param value		The value that the command queue is waiting for the fence to reach or exceed. 
		//					So when ID3D12Fence::GetCompletedValue is greater than or equal to Value, the wait is terminated.
		void Wait(HFenceDX12 fence, uint64 value)
		{
			Get()->Wait(fence.Get(), value);
		}
	};

	struct HRootSignatureDX12 : public HBaseDX12
	{
		ID3D12RootSignature* Get() const { return reinterpret_cast<ID3D12RootSignature*>(mPtr); }
	};



	//////////////////////////////////////////////////////////////////////////
	struct HGFXDeviceDX12 : public HBaseDX12
	{
		ID3D12Device* Get() const { return reinterpret_cast<ID3D12Device*>(mPtr); }


		HCmdAllocatorDX12 CreateCommandAllocator(EGFXCommandListType type)
		{
			HCmdAllocatorDX12 ret;
			UCHECK_DXRET(Get()->CreateCommandAllocator(UToDX12(type), IIDHANDLE(ID3D12CommandAllocator, ret)));
			return ret;
		}
		HCmdQueueDX12 CreateCommandQueue(EGFXCommandListType type, unsigned nodeMask = 0, unsigned priority = 0)
		{
			D3D12_COMMAND_QUEUE_DESC desc;
			desc.NodeMask = nodeMask;
			desc.Priority = priority > 0 ? D3D12_COMMAND_QUEUE_PRIORITY_HIGH : D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
			desc.Type = UToDX12(type);
			desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			HCmdQueueDX12 ret;
			UCHECK_DXRET(Get()->CreateCommandQueue(&desc, IIDHANDLE(ID3D12CommandQueue, ret)));
			return ret;
		}
		//@commandAllocator the allocator object that the device creates command lists from
		HCmdListDX12 CreateCommandList(EGFXCommandListType type, HCmdAllocatorDX12 commandAllocator, HPipelineStateDX12 optInitialState = HPipelineStateDX12(), unsigned nodeMask = 0)
		{
			HCmdListDX12 ret;
			UCHECK_DXRET(Get()->CreateCommandList(nodeMask, UToDX12(type), commandAllocator.Get(), optInitialState.Get(), IIDHANDLE(ID3D12CommandList, ret)));
			return ret;
		}
		
		HPipelineStateDX12 CreateGraphicsPipelineState()
		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc;
			HPipelineStateDX12 ret;
			UCHECK_DXRET(Get()->CreateGraphicsPipelineState(&desc, IIDHANDLE(ID3D12PipelineState, ret)));
			return ret;
		}
		HPipelineStateDX12 CreateComputePiplineState(HRootSignatureDX12 rootSignature, GFXShader* cs, unsigned nodeMask = 0)
		{
			//#TODO Shader byte code
			D3D12_COMPUTE_PIPELINE_STATE_DESC desc;
			desc.NodeMask = nodeMask;
			desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
			desc.CS.BytecodeLength = 0;
			desc.CS.pShaderBytecode = 0;
			desc.CachedPSO.CachedBlobSizeInBytes = 0;
			desc.CachedPSO.pCachedBlob = nullptr;
			desc.pRootSignature = rootSignature.Get();
			HPipelineStateDX12 ret;
			UCHECK_DXRET(Get()->CreateComputePipelineState(&desc, IIDHANDLE(ID3D12PipelineState, ret)));
			return ret;
		}

	};

		
	struct RootParameterDX
	{
		CD3DX12_ROOT_PARAMETER mParam;
		
		void InitAsConstants(unsigned num32BitValues, unsigned shaderRegister)
		{
			mParam.InitAsConstants(num32BitValues, shaderRegister);
		}
		void InitAsCBV(unsigned shaderRegister)
		{
			mParam.InitAsConstantBufferView(shaderRegister);
		}
		void InitAsSRV(unsigned shaderRegister)
		{
			mParam.InitAsShaderResourceView(shaderRegister);
		}
		void InitAsUAV(unsigned shaderRegister)
		{
			mParam.InitAsUnorderedAccessView(shaderRegister);
		}
	};


	

};