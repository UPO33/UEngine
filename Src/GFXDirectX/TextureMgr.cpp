#include "TextureMgr.h"
#include "Context.h"
#include "DescriptorHeap.h"

#include <DirectXTK12/DDSTextureLoader.h>
#include <DirectXTK12/WICTextureLoader.h>


namespace UGFX
{
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
GFXTexture* TextureMgrDX12::LoadTextureDefault(TSPtr<SmartMemBlock> textureData, bool bSRGB)
{
	ID3D12Resource* textureDX = nullptr;
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	HRESULT res = DirectX::LoadDDSTextureFromMemory(mCtx->mDevice, (const uint8_t*)textureData->Memory(), textureData->Size(), &textureDX, subresources);
	if (SUCCEEDED(res))
	{
		CmdList* cmdList = mCtx->mQueueMgr->GetNewGraphicCmdList();

		ID3D12Resource* uploadHeap = nullptr;
		const UINT64 uploadBufferSize = GetRequiredIntermediateSize(textureDX, 0,static_cast<UINT>(subresources.size()));

		HRESULT hr = mCtx->mDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
			IID_PPV_ARGS(&uploadHeap));
		
		UCHECK_DXRET(hr);


		UpdateSubresources(cmdList->GetCommandList(), textureDX, uploadHeap, 0, 0, subresources.size(), subresources.data());

		cmdList->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(textureDX, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

		cmdList->Finish(true);


		

		TextureDX12* resultTexture = new TextureDX12(textureDX);

		//creating views
		{
			
			DescHandleCSU srvVisible = GetDescMgr()->AllocCSU(true);
			DescHandleCSU srvNonVisible = GetDescMgr()->AllocCSU(false);
			
			//handle can be a shader-visible or non-shader-visible
			mCtx->mDevice->CreateShaderResourceView(textureDX, nullptr, srvNonVisible.GetCpuHandle());
			mCtx->mDevice->CopyDescriptorsSimple(1, srvVisible, srvNonVisible, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			resultTexture->mSRV = srvNonVisible;
			resultTexture->mSRVShaderVisible = srvVisible;
		}

		return resultTexture;
	}
	else
	{
		ULOG_ERROR("failed to load texture");
	}
	return nullptr;
}




TextureDX12* TextureMgrDX12::GetChessTexture()
{
	// Note: ComPtr's are CPU objects but this resource needs to stay in scope until
	// the command list that references it has finished executing on the GPU.
	// We will flush the GPU at the end of this method to ensure the resource is not
	// prematurely destroyed.
	ID3D12Resource* textureUploadHeap;
	ID3D12Resource* texture = nullptr;

	const unsigned TextureWidth = 512;
	const unsigned TextureHeight = 512;
	const unsigned TexturePixelSize = 4;
	

	// Create the texture.
	{
		// Describe and create a Texture2D.
		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.MipLevels = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.Width = TextureWidth;
		textureDesc.Height = TextureHeight;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

		UCHECK_DXRET(mCtx->mDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&textureDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&texture)));

		const UINT64 uploadBufferSize = GetRequiredIntermediateSize(texture, 0, 1);

		// Create the GPU upload buffer.
		UCHECK_DXRET(mCtx->mDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&textureUploadHeap)));

		// Copy data to the intermediate upload heap and then schedule a copy 
		// from the upload heap to the Texture2D.
		std::vector<UINT8> texturePixels;
		for (unsigned i = 0; i < TextureWidth * TextureHeight * 5; i++)
			texturePixels.push_back(rand());

		D3D12_SUBRESOURCE_DATA textureData = {};
		textureData.pData = &texturePixels[0];
		textureData.RowPitch = TextureWidth * TexturePixelSize;
		textureData.SlicePitch = textureData.RowPitch * TextureHeight;

		CmdList* cmdList = mCtx->mQueueMgr->GetNewGraphicCmdList();

		
		UpdateSubresources(cmdList->GetCommandList(), texture, textureUploadHeap, 0, 0, 1, &textureData);
		cmdList->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(texture, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

		//// Describe and create a SRV for the texture.
		//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		//srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		//srvDesc.Format = textureDesc.Format;
		//srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		//srvDesc.Texture2D.MipLevels = 1;


		cmdList->Finish(true);
	}

	return new TextureDX12(texture);

	//mCtx->mDevice->CreateShaderResourceView(texture, &srvDesc, m_srvHeap->GetCPUDescriptorHandleForHeapStart());
}

TextureDX12* TextureMgrDX12::CreateWhiteTexture()
{
	ID3D12Resource* textureUploadHeap = nullptr;
	//ID3D12Resource* texture = nullptr;

	const unsigned TextureWidth = 8;
	const unsigned TextureHeight = 8;
	const unsigned TexturePixelSize = 4;


	// Create the texture.
	
		// Describe and create a Texture2D.
		D3D12_RESOURCE_DESC textureDesc = {};
		textureDesc.MipLevels = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.Width = TextureWidth;
		textureDesc.Height = TextureHeight;
		textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		textureDesc.DepthOrArraySize = 1;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		
		UCHECK_DXRET(mCtx->mDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&textureDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&textureUploadHeap)));

		//const UINT64 uploadBufferSize = GetRequiredIntermediateSize(texture, 0, 1);

		// Copy data to the intermediate upload heap and then schedule a copy 
		// from the upload heap to the Texture2D.
		std::vector<unsigned> texturePixels;
		for (unsigned i = 0; i < TextureWidth * TextureHeight; i++)
			texturePixels.push_back(0xFFFFffff);

		CmdList* cmdList = mCtx->mQueueMgr->GetNewGraphicCmdList();


		cmdList->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(textureUploadHeap,
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

		cmdList->Finish(true);
// 		void* mapped = nullptr;
// 		UCHECK_DXRET(textureUploadHeap->Map(0, nullptr, &mapped));
// 		
		return new TextureDX12(textureUploadHeap);

}

DescriptorMgrDX12* TextureMgrDX12::GetDescMgr() const
{
	return ((DescriptorMgrDX12*)mCtx->mDescriptorMgr);
}

TextureMgrDX12::TextureMgrDX12(GFXContextDX12* ctx) : mCtx(ctx)
{

}




};

