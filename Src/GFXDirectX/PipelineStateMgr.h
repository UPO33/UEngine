#pragma once

#include "Shaders.h"



namespace UGFX
{
	//////////////////////////////////////////////////////////////////////////
	class PipelineStateMgrDX12;
	class GFXContextDX12;

	//////////////////////////////////////////////////////////////////////////
	struct GraphicsPiplineStateDX12 : GFXGraphicPiplineState
	{
		GFXGraphicsPiplineStateDesc		mDesc;
		unsigned						mIndex;

		GraphicsPiplineStateDX12(ID3D12PipelineState* pso, const GFXGraphicsPiplineStateDesc& desc, unsigned index)
			: mDesc(desc), mIndex(index)
		{
			mNativeHandle = pso;
		}
		ID3D12PipelineState* GetHandle() const { return reinterpret_cast<ID3D12PipelineState*>(mNativeHandle); }
		
	};
	//////////////////////////////////////////////////////////////////////////
	struct ComputePipelineStateDX12 : GFXComputePiplineState
	{
		GFXComputePiplineStateDesc		mDesc;
		unsigned						mIndex;

		ComputePipelineStateDX12(ID3D12PipelineState* pso, const GFXComputePiplineStateDesc& desc, unsigned index)
			: mDesc(desc), mIndex(index)
		{
			mNativeHandle = pso;
		}

		ID3D12PipelineState* GetHandle() const { return reinterpret_cast<ID3D12PipelineState*>(mNativeHandle); }
	};

	void UToDX12(PipelineStateMgrDX12* psMgr, const GFXComputePiplineStateDesc& in, D3D12_COMPUTE_PIPELINE_STATE_DESC& out);

	//////////////////////////////////////////////////////////////////////////
	class PipelineStateMgrDX12 : public IPipelineStateMgr
	{
	public:

		GFXContextDX12*	mCtx = nullptr;

		TArray<GraphicsPiplineStateDX12*>			mGraphicStates;
		TArray<ComputePipelineStateDX12*>			mComputeStates;

		ID3D12RootSignature* mEmptyRootSignature = nullptr;

		//////////////////////////////////////////////////////////////////////////
		PipelineStateMgrDX12(GFXContextDX12* ctx);

		ID3D12Device* GetDXDevice() const;
		void FindPiplineStateUsingShader(GFXShader* tagetShader, TArray<GFXPiplineState*>& out)
		{
		}
		bool RecreatePipelineState(GFXPiplineState* ps);
		ID3D12PipelineState* CreateGraphicsPipleStateDX(const GFXGraphicsPiplineStateDesc& desc);

		//////////////////////////////////////////////////////////////////////////
		GraphicsPiplineStateDX12* GetGraphicsPS(const GFXGraphicsPiplineStateDesc& desc);
		ComputePipelineStateDX12* GetComputePS(const GFXComputePiplineStateDesc& desc)
		{
			throw std::runtime_error("");
		}
		virtual GFXGraphicPiplineState* GetGPS(const GFXGraphicsPiplineStateDesc& desc) override
		{
			return GetGraphicsPS(desc);
		}
		virtual GFXComputePiplineState* GetCPS(const GFXComputePiplineStateDesc& desc) override
		{
			return GetComputePS(desc);
		}

	};
};