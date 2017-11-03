#include "PipelineStateMgr.h"
#include "ShaderMgr.h"
#include "Context.h"

namespace UGFX
{ 
	//////////////////////////////////////////////////////////////////////////
	D3D12_INPUT_ELEMENT_DESC UToDX12(const GFXInputElementDesc& input)
	{
		D3D12_INPUT_ELEMENT_DESC ret;
		ret.SemanticName = input.mSemanticName;
		ret.SemanticIndex = input.mSemanticIndex;
		ret.Format = UToDX12(input.mFormat);
		ret.InputSlot = input.mInputSlot;
		ret.AlignedByteOffset = input.mAlignedByteOffset;
		ret.InstanceDataStepRate = input.mInstanceDataStepRate;
		ret.InputSlotClass = input.mPerVertex ? D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA : D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
		return ret;
	}
	
	//////////////////////////////////////////////////////////////////////////
	void UToDX12(PipelineStateMgrDX12* psMgr, const GFXComputePiplineStateDesc& in, D3D12_COMPUTE_PIPELINE_STATE_DESC& out)
	{
		out.CachedPSO.pCachedBlob = nullptr;
		out.CachedPSO.CachedBlobSizeInBytes = 0;
		out.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		out.NodeMask = 0;
		out.pRootSignature = nullptr;
		if (in.mCS)
			out.CS = ((ShaderDX12*)in.mCS)->GetDXByteCode();
		else
			out.CS = D3D12_SHADER_BYTECODE{ nullptr, 0 };
	}
	//////////////////////////////////////////////////////////////////////////
	PipelineStateMgrDX12::PipelineStateMgrDX12(GFXContextDX12* ctx) : mCtx(ctx)
	{
		UASSERT(ctx);

		//create empty signature
		{
			CD3DX12_ROOT_SIGNATURE_DESC desc;
			desc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			ID3DBlob* rsBytes = nullptr;
			ID3DBlob* error = nullptr;

			D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &rsBytes, &error);
			UCHECK_DXRET(mCtx->mDevice->CreateRootSignature(0, rsBytes->GetBufferPointer(), rsBytes->GetBufferSize(), IID_PPV_ARGS(&mEmptyRootSignature)));
		}
	}

	ID3D12Device* PipelineStateMgrDX12::GetDXDevice() const
	{
		return mCtx->mDevice;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PipelineStateMgrDX12::RecreatePipelineState(GFXPiplineState* ps)
	{
		if (GraphicsPiplineStateDX12* gpsDX = dynamic_cast<GraphicsPiplineStateDX12*>(ps))
		{
			if (gpsDX->GetHandle())
				gpsDX->GetHandle()->Release();
			
			gpsDX->mNativeHandle = nullptr;

			if (ID3D12PipelineState* newPSO = CreateGraphicsPipleStateDX(gpsDX->mDesc))
			{
				gpsDX->mNativeHandle = newPSO;
				return true;
			}
			else
			{
				//#TODO what should I do ? crash ? just setting null ? or setting a empty PSO?
				ULOG_ERROR("failed to recreate PipelineState");
				return false;
			}
		}
		else if (ComputePipelineStateDX12* cpsDX = dynamic_cast<ComputePipelineStateDX12*>(ps))
		{
			//#TODO
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	ID3D12PipelineState* PipelineStateMgrDX12::CreateGraphicsPipleStateDX(const GFXGraphicsPiplineStateDesc& desc)
	{
		D3D12_INPUT_ELEMENT_DESC inputElementsDX[GFXInputLayout::MAX_ELEMENT];
		D3D12_GRAPHICS_PIPELINE_STATE_DESC descDX = {};

		//assigning root signature
		{
			descDX.pRootSignature = nullptr;

			if (desc.mRootSignature == nullptr)
				descDX.pRootSignature = this->mEmptyRootSignature;
			else
				descDX.pRootSignature = (ID3D12RootSignature*)desc.mRootSignature->GetNativeHandle();
		}

		//assigning shaders
		if (desc.mVS) descDX.VS = ((ShaderDX12*)desc.mVS)->GetDXByteCode();
		if (desc.mPS) descDX.PS = ((ShaderDX12*)desc.mPS)->GetDXByteCode();
		if (desc.mDS) descDX.DS = ((ShaderDX12*)desc.mDS)->GetDXByteCode();
		if (desc.mHS) descDX.HS = ((ShaderDX12*)desc.mHS)->GetDXByteCode();
		if (desc.mGS) descDX.GS = ((ShaderDX12*)desc.mGS)->GetDXByteCode();

		//#TODO not implemented yet
		//out.StreamOutput

		descDX.BlendState = UToDX12(desc.mBlendState);
		descDX.SampleMask = desc.mSampleMask;
		descDX.RasterizerState = UToDX12(desc.mRasterizer);
		descDX.DepthStencilState = UToDX12(desc.mDepthStencil);

		//assigning input layout
		descDX.InputLayout.NumElements = 0;
		if (desc.mInputLayout)
		{
			descDX.InputLayout.NumElements = desc.mInputLayout->mElements.Length();
			UASSERT(desc.mInputLayout->mElements.Length() <= GFXInputLayout::MAX_ELEMENT);

			descDX.InputLayout.pInputElementDescs = inputElementsDX;

			for (unsigned iInputElement = 0; iInputElement < desc.mInputLayout->mElements.Length(); iInputElement++)
			{
				inputElementsDX[iInputElement] = UToDX12(desc.mInputLayout->Get(iInputElement));
			}
		}


		//#TODO 
		//out.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		
		descDX.PrimitiveTopologyType = UToDX12(desc.mPrimitiveTopologyType);
		descDX.NumRenderTargets = desc.mNumRenderTargets;

		for (UINT i = 0; i < descDX.NumRenderTargets; i++)
			descDX.RTVFormats[i] = UToDX12(desc.mRTVFormats[i]);

		descDX.DSVFormat = UToDX12(desc.mDSVFormat);

		descDX.SampleDesc.Count = desc.mSampleDesc.mCount;
		descDX.SampleDesc.Quality = desc.mSampleDesc.Quality;

		descDX.NodeMask = 0;
		descDX.CachedPSO.CachedBlobSizeInBytes = 0;
		descDX.CachedPSO.pCachedBlob = nullptr;

		descDX.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		ID3D12PipelineState* pso = nullptr;
		GetDXDevice()->CreateGraphicsPipelineState(&descDX, IID_PPV_ARGS(&pso));
		return pso;
	}
	//////////////////////////////////////////////////////////////////////////
	UGFX::GraphicsPiplineStateDX12* PipelineStateMgrDX12::GetGraphicsPS(const GFXGraphicsPiplineStateDesc& desc)
	{
		if (ID3D12PipelineState* pso = CreateGraphicsPipleStateDX(desc))
		{
			auto newPSO = new GraphicsPiplineStateDX12(pso, desc, mGraphicStates.Length());
			mGraphicStates.Add(newPSO);
			return newPSO;
		}
		else
		{
			ULOG_ERROR("failed to create GraphicPipelineState, see the debugger output");
		}
		
		return nullptr;
	}

};
