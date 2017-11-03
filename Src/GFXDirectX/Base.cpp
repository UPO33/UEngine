#include "Base.h"

namespace UGFX
{

	D3D12_BLEND_DESC UToDX12(const GFXBlendDesc& input)
	{
		D3D12_BLEND_DESC ret;
		ret.AlphaToCoverageEnable = input.mAlphaToCoverageEnable;
		ret.IndependentBlendEnable = input.mIndependentBlendEnable;
		for (size_t i = 0; i < 8; i++)
		{
			ret.RenderTarget[i].BlendEnable = input.mRenderTarget[i].mBlendEnable;
			ret.RenderTarget[i].LogicOpEnable = input.mRenderTarget[i].mLogicOpEnable;
			ret.RenderTarget[i].SrcBlend = UToDX12(input.mRenderTarget[i].mSrcBlend);
			ret.RenderTarget[i].DestBlend = UToDX12(input.mRenderTarget[i].mDestBlend);
			ret.RenderTarget[i].BlendOp = UToDX12(input.mRenderTarget[i].mBlendOp);

			ret.RenderTarget[i].SrcBlendAlpha = UToDX12(input.mRenderTarget[i].mSrcBlendAlpha);
			ret.RenderTarget[i].DestBlendAlpha = UToDX12(input.mRenderTarget[i].mDestBlendAlpha);
			ret.RenderTarget[i].BlendOpAlpha = UToDX12(input.mRenderTarget[i].mBlendOpAlpha);

			ret.RenderTarget[i].RenderTargetWriteMask = UToDX12(input.mRenderTarget[i].mRenderTargetWriteMask);
		}
		return ret;
	}

	D3D12_RASTERIZER_DESC UToDX12(const GFXRasterizerDesc& input)
	{
		D3D12_RASTERIZER_DESC ret;
		ret.FillMode = input.mWireframe ? D3D12_FILL_MODE_WIREFRAME : D3D12_FILL_MODE_SOLID;
		ret.CullMode = UToDX12(input.mCullMode);
		ret.FrontCounterClockwise = input.mFrontCounterClockwise;
		ret.DepthBias = input.mDepthBias;
		ret.DepthBiasClamp = input.mDepthBiasClamp;
		ret.SlopeScaledDepthBias = input.mSlopeScaledDepthBias;
		ret.DepthClipEnable = input.mDepthClipEnable;
		ret.MultisampleEnable = input.mMultisampleEnable;
		ret.AntialiasedLineEnable = input.mAntialiasedLineEnable;
		ret.ForcedSampleCount = input.mForcedSampleCount;
		ret.ConservativeRaster = input.mConservativeRaster ? D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON : D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		return ret;
	}
	D3D12_DEPTH_STENCILOP_DESC UToDX12(const GFXDepthStencilOpDesc& input)
	{
		D3D12_DEPTH_STENCILOP_DESC ret;
		ret.StencilFailOp = UToDX12(input.mStencilFailOp);
		ret.StencilDepthFailOp = UToDX12(input.mStencilDepthFailOp);
		ret.StencilPassOp = UToDX12(input.mStencilPassOp);
		ret.StencilFunc = UToDX12(input.mStencilFunc);
		return ret;
	}
	D3D12_DEPTH_STENCIL_DESC UToDX12(const GFXDepthStencilDesc& input)
	{
		D3D12_DEPTH_STENCIL_DESC ret;
		ret.DepthEnable = input.mDepthEnable;
		ret.DepthWriteMask = input.mDepthWriteAll ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
		ret.DepthFunc = UToDX12(input.mDepthFunc);
		ret.StencilEnable = input.mStencilEnable;
		ret.StencilReadMask = input.mStencilReadMask;
		ret.StencilWriteMask = input.mStencilWriteMask;
		ret.FrontFace = UToDX12(input.mFrontFace);
		ret.BackFace = UToDX12(input.mBackFace);
		return ret;
	}



};