#pragma once
#include "Base.h"

namespace UGFX
{

// Maximum 64 DWORDS divied up amongst all root parameters.
// Root constants = 1 DWORD * NumConstants
// Root descriptor (CBV, SRV, or UAV) = 2 DWORDs each
// Descriptor table pointer = 1 DWORD
// Static samplers = 0 DWORDS (compiled into shader)
struct RootSignatureDX12 : GFXRootSignature
{
	GFXRootSignatureDesc	mDesc;

	RootSignatureDX12(ID3D12RootSignature* rs, const GFXRootSignatureDesc& desc) :
		mDesc(desc)
	{
		mNativeHandle = rs;
	}

	ID3D12RootSignature* GetHandle() const { return reinterpret_cast<ID3D12RootSignature*>(mNativeHandle); }
};

//////////////////////////////////////////////////////////////////////////
#if 0
struct RootSignatureCommon 
{
	RootSignatureCommon(ID3D12Device* pDevice, unsigned maxSRV, unsigned maxUAV, unsigned maxCBV, unsigned maxSampler)
	{
		CD3DX12_ROOT_PARAMETER dxParams[4];
		dxParams[0].InitAsDescriptorTable(1, &CD3DX12_DESCRIPTOR_RANGE{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, maxSRV, 0, 0, 0 });
		dxParams[1].InitAsDescriptorTable(1, &CD3DX12_DESCRIPTOR_RANGE{ D3D12_DESCRIPTOR_RANGE_TYPE_UAV, maxUAV, 0, 0, 0 });
		dxParams[2].InitAsDescriptorTable(1, &CD3DX12_DESCRIPTOR_RANGE{ D3D12_DESCRIPTOR_RANGE_TYPE_CBV, maxCBV, 0, 0, 0 });
		dxParams[3].InitAsDescriptorTable(1, &CD3DX12_DESCRIPTOR_RANGE{ D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, maxSampler, 0, 0, 0 });

		CD3DX12_ROOT_SIGNATURE_DESC dxDesc;
		dxDesc.Init(UARRAYLEN(dxParams), dxParams, 0, nullptr);

		ID3DBlob* rsBlob = nullptr;
		ID3DBlob* errorBlob = nullptr;

		if (SUCCEEDED(D3D12SerializeRootSignature(&dxDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rsBlob, &errorBlob)))
		{
			pDevice->CreateRootSignature(0, rsBlob->GetBufferPointer(), rsBlob->GetBufferSize(), IID_PPV_ARGS(&mRS));
		}
		else
		{
			ULOG_FATAL("%", (const char*)errorBlob->GetBufferPointer());
		}
	}

	ID3D12RootSignature* mRS = nullptr;
};
#endif

};