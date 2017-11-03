#include "RootSignature.h"
#include "Context.h"

#include <DirectXTK12/CommonStates.h>

namespace UGFX
{



//////////////////////////////////////////////////////////////////////////
inline D3D12_ROOT_SIGNATURE_FLAGS UToDX12(ERootSignatureFlags in)
{
	return (D3D12_ROOT_SIGNATURE_FLAGS)in;
}
//////////////////////////////////////////////////////////////////////////
inline D3D12_ROOT_PARAMETER_TYPE UToDX12(ERootParameterType in)
{
	return (D3D12_ROOT_PARAMETER_TYPE)in;
}
//////////////////////////////////////////////////////////////////////////
inline D3D12_DESCRIPTOR_RANGE_TYPE UToDX12(EDescriptorRange in)
{
	return (D3D12_DESCRIPTOR_RANGE_TYPE)in;
}
//////////////////////////////////////////////////////////////////////////
GFXRootSignature* GFXContextDX12::CreateRootSignature(const GFXRootSignatureDesc& desc)
{
	CD3DX12_ROOT_PARAMETER paramsDX[MAX_ROOT_PARAMETER];
	CD3DX12_ROOT_SIGNATURE_DESC descDX;
	
	static const unsigned MAX_RANGES = 64;
	CD3DX12_DESCRIPTOR_RANGE rangesDX[MAX_RANGES];
	unsigned rangeIter = 0;

	unsigned numParam = desc.mParams.Length();

	//extracting parameters
	for (unsigned iParam = 0; iParam < numParam; iParam++)
	{
		const GFXRootParameter& paramDesc = desc.mParams[iParam];

		paramsDX[iParam].ParameterType = UToDX12(paramDesc.mType);
		paramsDX[iParam].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;


		switch (paramDesc.mType)
		{
		case ERootParameterType::ERPT_32BIT_CONSTANTS:
		{
			paramsDX[iParam].Constants.Num32BitValues = paramDesc.mConstants.mNum32BitValues;
			paramsDX[iParam].Constants.ShaderRegister = paramDesc.mConstants.mShaderRegister;
			paramsDX[iParam].Constants.RegisterSpace = 0;
			break;
		};

		case ERootParameterType::ERPT_CBV:
		case ERootParameterType::ERPT_SRV:
		case ERootParameterType::ERPT_UAV:
		{
			paramsDX[iParam].Descriptor.ShaderRegister = paramDesc.mDescriptor.mShaderRegister;
			paramsDX[iParam].Descriptor.RegisterSpace = 0;

			break;
		};
		case ERootParameterType::ERPT_DESCRIPTOR_TABLE:
		{
			UASSERT(rangeIter < MAX_RANGES);

			paramsDX[iParam].DescriptorTable.NumDescriptorRanges = paramDesc.mDescriptorTable.mNumRange;
			paramsDX[iParam].DescriptorTable.pDescriptorRanges = &rangesDX[rangeIter];

			for(unsigned iRange = 0; iRange < paramDesc.mDescriptorTable.mNumRange; iRange++)
			{
				rangesDX[rangeIter].NumDescriptors = paramDesc.mDescriptorTable.mRanges[iRange].mCount;
				rangesDX[rangeIter].BaseShaderRegister = paramDesc.mDescriptorTable.mRanges[iRange].mBaseShaderRegister;
				rangesDX[rangeIter].RangeType = UToDX12(paramDesc.mDescriptorTable.mRanges[iRange].mType);
				rangesDX[rangeIter].RegisterSpace = 0;
				rangesDX[rangeIter].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


				rangeIter++;
			}

			break;
		}
		}
	}

	D3D12_STATIC_SAMPLER_DESC defaultSamplers[] = 
	{
		DirectX::CommonStates::StaticPointWrap(0),
		DirectX::CommonStates::StaticPointClamp(1),
		DirectX::CommonStates::StaticLinearWrap(2),
		DirectX::CommonStates::StaticLinearClamp(3),
		DirectX::CommonStates::StaticAnisotropicWrap(4),
		DirectX::CommonStates::StaticAnisotropicClamp(5),
	};
	

	//#TODO static sampler not implemented yet
	descDX.Init(numParam, paramsDX, UARRAYLEN(defaultSamplers), defaultSamplers, UToDX12(desc.mFlags));

	ID3DBlob* rsBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	ID3D12RootSignature* rs = nullptr;

	if (SUCCEEDED(D3D12SerializeRootSignature(&descDX, D3D_ROOT_SIGNATURE_VERSION_1, &rsBlob, &errorBlob)))
	{
		if (SUCCEEDED(mDevice->CreateRootSignature(0, rsBlob->GetBufferPointer(), rsBlob->GetBufferSize(), IID_PPV_ARGS(&rs))))
		{
		}
		else
		{
			ULOG_ERROR("failed to create RootSignature");
			return nullptr;
		}
	}
	else
	{
		ULOG_ERROR("failed to serialize RootSignature\n %", (const char*)errorBlob->GetBufferPointer());
		return nullptr;
	}
	
	if (rs)
	{
		RootSignatureDX12* newRS = new RootSignatureDX12(rs, desc);
		return newRS;
	}

	return nullptr;
}

//Common RS
//RootParam0   32 SRV	6 UAV	 

};