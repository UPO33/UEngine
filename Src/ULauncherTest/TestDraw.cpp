#include "TestDraw.h"
//#include "../GFXDirectX/RootSignature.h"
//#include "../GFXDirectX/Wrapper.h"
#include "../GFXDirectX/TextureMgr.h"

#define MYDESC

//////////////////////////////////////////////////////////////////////////
inline void UCopyTextureHandleTo(GFXTexture* pTexture, DescHandleBase dst)
{
	if (TextureDX12* dxt = (TextureDX12*)pTexture)
	{
		ID3D12Device* pDevice = nullptr;
		dxt->GetHandle()->GetDevice(IID_PPV_ARGS(&pDevice));
		UASSERT(pDevice);
		pDevice->CopyDescriptorsSimple(1, dst.GetCpuHandle(), dxt->mSRV.GetCpuHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
}
//////////////////////////////////////////////////////////////////////////
inline void UCreateSRV(GFXTexture* pTexture, DescHandleBase dst)
{
	TextureDX12* dxt = (TextureDX12*)pTexture;
	UASSERT(dxt);

	{
		ID3D12Device* pDevice = nullptr;
		dxt->GetHandle()->GetDevice(IID_PPV_ARGS(&pDevice));
		UASSERT(pDevice);
		pDevice->CreateShaderResourceView(dxt->GetHandle(), nullptr, dst.GetCpuHandle());
	}
}
//////////////////////////////////////////////////////////////////////////
inline void UCopyTextureSRV(ID3D12Device* pDevice, GFXTexture* pTexture, DescHandleCSU dst)
{
	pDevice->CopyDescriptorsSimple(1, dst, ((TextureDX12*)pTexture)->mSRV, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

//////////////////////////////////////////////////////////////////////////
inline D3D12_SHADER_BYTECODE UGetShaderByteCode(Name filename, Name entryPoint, EShader type, const ShaderMacros& macros = ShaderMacros())
{
	if (GFXShader* pShader = gGFX->GetShaderMgr()->GetShader(filename, entryPoint, type, macros))
	{
		return ((ShaderDX12*)pShader)->GetDXByteCode();
	}
	return D3D12_SHADER_BYTECODE{ nullptr, 0 };
}
inline void* UMapResource(ID3D12Resource* resource, unsigned subResource = 0, const D3D12_RANGE* range = nullptr)
{
	void* mapped = nullptr;
	UCHECK_DXRET(resource->Map(subResource, range, &mapped));
	return mapped;
}
inline void UUnmapResource(ID3D12Resource* res, unsigned subRes = 0, const D3D12_RANGE* range = nullptr)
{
	res->Unmap(subRes, range);
}

struct alignas(256) CBTest
{
	Vec4 mColor = Vec4(0.0f);
	float mOffset;
};

ID3D12RootSignature* USerAndCreateRoorSignature(D3D12_ROOT_SIGNATURE_DESC* rDesc)
{
	ID3DBlob* rsBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	ID3D12RootSignature* rs = nullptr;
	if (SUCCEEDED(D3D12SerializeRootSignature(rDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rsBlob, &errorBlob)))
	{
		gGFX->mDevice->CreateRootSignature(0, rsBlob->GetBufferPointer(), rsBlob->GetBufferSize(), IID_PPV_ARGS(&rs));
		return rs;
	}
	else
	{
		ULOG_ERROR("Errors:\n %", (const char*)errorBlob->GetBufferPointer());
		return nullptr;
	}
}
ID3D12RootSignature* USerAndCreateRoorSignature(D3D12_ROOT_PARAMETER* params, size_t numParam)
{
	CD3DX12_ROOT_SIGNATURE_DESC rDesc;
	rDesc.Init(numParam, params, 0, nullptr);
	ID3DBlob* rsBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	ID3D12RootSignature* rs = nullptr;
	if (SUCCEEDED(D3D12SerializeRootSignature(&rDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rsBlob, &errorBlob)))
	{
		gGFX->mDevice->CreateRootSignature(0, rsBlob->GetBufferPointer(), rsBlob->GetBufferSize(), IID_PPV_ARGS(&rs));
		return rs;
	}
	else
	{
		ULOG_ERROR("Errors:\n %", (const char*)errorBlob->GetBufferPointer());
		return nullptr;
	}
}

void TestScreenQuadGen::GenRndPoints()
{
	const size_t numPoint = 4096;
	mPoints.AddUnInit(numPoint);

	for (size_t i = 0; i < numPoint; i++)
	{
		mPoints[i].mPosition = Vec2(RandFloat01() * 2 - 1, RandFloat01() * 2 - 1);
		//mPoints[i].mPosition = Vec2(RandFloat01() * 8 - 4, RandFloat01() * 8 - 4);
		mPoints[i].mSize = Vec2(RandFloat01() * 0.1 + 0.01);
	}
}

void TestScreenQuadGen::CreateResources()
{
	{
		mTextureFire = gGFX->GetTextureMgr()->LoadTextureDefault(UFileOpenReadFull("../../Assets/Fire.dds"), true);
		mTextureSmoke = gGFX->GetTextureMgr()->LoadTextureDefault(UFileOpenReadFull("../../Assets/Smoke.dds"), true);
		mTextureSpark = gGFX->GetTextureMgr()->LoadTextureDefault(UFileOpenReadFull("../../Assets/Spark.dds"), true);
		
	}
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		desc.NodeMask = 0;
		desc.NumDescriptors = 512;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

		UCHECK_DXRET(gGFX->mDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mDescHeapSRV)));

		SBufferColor colors0;
		mColorBuffer0 = UCreateDefaultCommittedUploadBuffer(sizeof(SBufferColor), &colors0);
		SBufferColor colors1;
		mColorBuffer1 = UCreateDefaultCommittedUploadBuffer(sizeof(SBufferColor), &colors1);

		D3D12_SHADER_RESOURCE_VIEW_DESC srv[2];
		srv[0].Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srv[0].Format = DXGI_FORMAT_UNKNOWN;
		srv[0].ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srv[0].Buffer.FirstElement = 0;
		srv[0].Buffer.StructureByteStride = sizeof(float[4]);
		srv[0].Buffer.NumElements = SBufferColor::NUM_COLOR;
		srv[0].Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		srv[1] = srv[0];

		//2 SBuffer + 3 Texture
		mDescHandleBase = gGFX->mDescriptorMgr->AllocCSU(true, 8);

#ifdef MYDESC
		gGFX->mDevice->CreateShaderResourceView(mColorBuffer0, srv + 0, (mDescHandleBase + 0).GetCpuHandle());
		gGFX->mDevice->CreateShaderResourceView(mColorBuffer1, srv + 1, (mDescHandleBase + 1).GetCpuHandle());
		UCopyTextureSRV(gGFX->mDevice, mTextureFire, (mDescHandleBase + 2));
		UCopyTextureSRV(gGFX->mDevice, mTextureSmoke, (mDescHandleBase + 3));
		UCopyTextureSRV(gGFX->mDevice, mTextureSpark, (mDescHandleBase + 4));

#else
		
#endif
	}
	//RoorSig
	{
		const unsigned Width = 512;
		const unsigned Height = 512;

// 		CD3DX12_ROOT_PARAMETER params[1];
// 		CD3DX12_DESCRIPTOR_RANGE ranges[1];
// 		ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
// 		params[0].InitAsDescriptorTable(1, ranges, D3D12_SHADER_VISIBILITY_ALL);
// 		CD3DX12_ROOT_SIGNATURE_DESC desc;
// 
// 		desc.Init(1, params, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
// 		mRS = USerAndCreateRoorSignature(&desc);

		GFXRootSignatureDesc rsd;
		rsd << GFXRootParameter::CreateConstans(16, 0);
		rsd << GFXRootParameter::CreateDescriptorTableRanges(EDR_SRV, 8, 0, EDR_UAV, 4, 0);

		mRS = gGFX->CreateRootSignature(rsd);
	}
	//create buffer and fill it
	{
		GenRndPoints();
		mVB = UCreateDefaultCommittedUploadBuffer(mPoints.Length() * sizeof(PointData), mPoints.Elements());
	}

	//PSO
	{
		mInputLayout = new GFXInputLayout;
		mInputLayout->mElements.AddDefault(2);

		mInputLayout->mElements[0].mSemanticName = "POSITION";
		mInputLayout->mElements[0].mFormat = EGFXFormat::R32G32_FLOAT;
		mInputLayout->mElements[0].mAlignedByteOffset = 0;

		mInputLayout->mElements[1].mSemanticName = "SIZE";
		mInputLayout->mElements[1].mFormat = EGFXFormat::R32G32_FLOAT;
		mInputLayout->mElements[1].mAlignedByteOffset = sizeof(float[2]);

		GFXGraphicsPiplineStateDesc psDesc;

		psDesc.mVS = gGFX->GetShaderMgr()->GetShader(mFileName, "VS", EShader::Vertex);
		psDesc.mGS = gGFX->GetShaderMgr()->GetShader(mFileName, "GS", EShader::Geometry);
		psDesc.mPS = gGFX->GetShaderMgr()->GetShader(mFileName, "PS", EShader::Pixel);

		psDesc.mDepthStencil.mDepthEnable = false;
		psDesc.mPrimitiveTopologyType = EPrimitiveTopologyType::POINT;
		psDesc.mRasterizer.mCullMode = EGFXCullMode::None;
		psDesc.mNumRenderTargets = 1;
		psDesc.mRTVFormats[0] = EGFXFormat::R8G8B8A8_UNORM;
		psDesc.mBlendState.mRenderTarget[0].InitAsAlphaBlend();
		
		psDesc.mRootSignature = mRS;
		psDesc.mInputLayout = mInputLayout;

		mPSO = gGFX->GetPipelineStateMgr()->GetGPS(psDesc);
	}
}

void TestScreenQuadGen::Render(CmdList* cmdList)
{
	{
		this->mOffset.x += GameInput::GetAnalogInput(GameInput::kAnalogMouseX);
		this->mOffset.y += GameInput::GetAnalogInput(GameInput::kAnalogMouseY);

		mMatrix = Matrix4Translation(mOffset);
	}


	cmdList->SetGraphicsRootSignature(mRS);

	//auto descMgr = (DescriptorMgrDX12*)gGFX->GetDescriptorMgr();
#ifdef MYDESC
	cmdList->SetSetDescriptorHeaps(gGFX->mDescriptorMgr->mCSUHeap.GetHeapPointer(), gGFX->mDescriptorMgr->mSamplerHeap.GetHeapPointer());

#else

	ID3D12DescriptorHeap* hesps[] = { this->mDescHeapSRV };
	cmdList->GetCommandList()->SetDescriptorHeaps(1, hesps);

#endif

	cmdList->SetPipelineState(mPSO);
	cmdList->SetPrimitiveTopology(EPrimitiveTopology::PointList);
	cmdList->SetGraphicsRoot32BitConstants(0, 16, &mMatrix);

#ifdef MYDESC
	cmdList->SetGraphicsRootDescriptorTable(1, mDescHandleBase);	//param1 is srv table
#else
	cmdList->SetGraphicsRootDescriptorTable(1, mDescHeapSRV->GetGPUDescriptorHandleForHeapStart());	//param1 is srv table
#endif

	cmdList->SetVertexBuffer(0, D3D12_VERTEX_BUFFER_VIEW{ mVB->GetGPUVirtualAddress(), (UINT)(sizeof(PointData) * mPoints.Length()), sizeof(PointData) });
	cmdList->DrawInstanced(mPoints.Length(), 1, 0, 0);
}
