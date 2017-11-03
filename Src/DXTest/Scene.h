#pragma once
#include "Base.h"


#define USHADERBYTECODE(var) D3D12_SHADER_BYTECODE { var, sizeof(var) }

struct SimpleVertex
{
	float	mXY[2];
	float	mColorRGB[3];
};

class TestScene
{
public:
	ID3D12PipelineState*	mPSOScreenTri = nullptr;
	ID3D12RootSignature*	mEmptyRootSignatue = nullptr;
	ID3D12Resource*			mScreenTriVBPosition = nullptr;
	ID3D12Resource*			mScreenTriVBColor = nullptr;
	ID3D12Resource*			mScreenTriIB = nullptr;
	ID3D12Resource*			mSimpleTexture = nullptr;
	
	void CreateResources()
	{
		//emprt root signature
		{
			CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			ID3DBlob* signature = nullptr;
			ID3DBlob* error = nullptr;
			if (SUCCEEDED(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error)))
			{
				UCHECK_DXRET(gGFX->mDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&mEmptyRootSignatue)));
			}
			else
			{
				ULOG_FATAL((const char*)error->GetBufferPointer());
			}
		}

		//pso
		{
			D3D12_INPUT_ELEMENT_DESC inputElements[] = 
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
			};

			D3D12_GRAPHICS_PIPELINE_STATE_DESC screenTriPSODesc = {};
			//screenTriPSODesc.VS = USHADERBYTECODE(gShaderTestScreenTriangleVS);
			//screenTriPSODesc.PS = USHADERBYTECODE(gShaderTestScreenTrianglePS);

			screenTriPSODesc.DepthStencilState.DepthEnable = false;
			screenTriPSODesc.DepthStencilState.StencilEnable = false;
			screenTriPSODesc.NumRenderTargets = 1;
			screenTriPSODesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			screenTriPSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			screenTriPSODesc.pRootSignature = mEmptyRootSignatue;
			screenTriPSODesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			screenTriPSODesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
			screenTriPSODesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			screenTriPSODesc.SampleDesc.Count = 1;
			screenTriPSODesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
			screenTriPSODesc.SampleMask = UINT_MAX;
			screenTriPSODesc.InputLayout.NumElements = UARRAYLEN(inputElements);
			screenTriPSODesc.InputLayout.pInputElementDescs = inputElements;

			UCHECK_DXRET(gGFX->mDevice->CreateGraphicsPipelineState(&screenTriPSODesc, IID_PPV_ARGS(&mPSOScreenTri)));
		}
		//VB
		{
			float  simpleTriVertPositions[] = { -1, 1,     1,1,     -1,-1 };
			float  simpleTriVertColors[] = { 1,0,0,    0,1,0,   0,0,1 };


			{
				UCHECK_DXRET(gGFX->mDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE
					, &CD3DX12_RESOURCE_DESC::Buffer(sizeof(simpleTriVertPositions)), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mScreenTriVBPosition)));

				void* mappedValue = nullptr;

				UCHECK_DXRET(mScreenTriVBPosition->Map(0, nullptr, &mappedValue));
				MemCopy(mappedValue, simpleTriVertPositions, sizeof(simpleTriVertPositions));
				mScreenTriVBPosition->Unmap(0, nullptr);
			}

			{
				UCHECK_DXRET(gGFX->mDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE
					, &CD3DX12_RESOURCE_DESC::Buffer(sizeof(simpleTriVertColors)), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mScreenTriVBColor)));

				void* mappedValue = nullptr;
				UCHECK_DXRET(mScreenTriVBColor->Map(0, nullptr, &mappedValue));
				MemCopy(mappedValue, simpleTriVertColors, sizeof(simpleTriVertColors));
				mScreenTriVBColor->Unmap(0, nullptr);
			}
		}
		//IB
		{
			unsigned short indicies[] = { 0,1,2 };
			UCHECK_DXRET(gGFX->mDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, 
				&CD3DX12_RESOURCE_DESC::Buffer(sizeof(indicies)), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mScreenTriIB)));
			
			void* mappedValue = nullptr;
			UCHECK_DXRET(mScreenTriIB->Map(0, nullptr, &mappedValue));
			MemCopy(mappedValue, indicies, sizeof(indicies));
			mScreenTriIB->Unmap(0, nullptr);
		}
		//SimpleTexture
		{
			return;
			const unsigned TextureW = 128;
			const unsigned TextureH = 128;
			UCHECK_DXRET(gGFX->mDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, TextureW, TextureH), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mSimpleTexture)));

			void* mappedValue = nullptr;
			UCHECK_DXRET(mSimpleTexture->Map(0, nullptr, &mappedValue));
			for (size_t iX = 0; iX < TextureW; iX++)
			{
				for (size_t iY = 0; iY < TextureH; iY++)
				{
					((Color32*)mappedValue)[iY * TextureW + iX] = Color32(rand() % 255, rand() % 255, rand() % 255);
				}
			}
			mSimpleTexture->Unmap(0, nullptr);
		}
	}
	void Render(ID3D12GraphicsCommandList* cmdList)
	{
		cmdList->SetGraphicsRootSignature(mEmptyRootSignatue);
		cmdList->SetPipelineState(mPSOScreenTri);
		cmdList->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		D3D12_VERTEX_BUFFER_VIEW vertexBuffers[] =
		{
			D3D12_VERTEX_BUFFER_VIEW{ mScreenTriVBPosition->GetGPUVirtualAddress(), sizeof(float[2]) * 3, sizeof(float[2]) },
			D3D12_VERTEX_BUFFER_VIEW{ mScreenTriVBColor->GetGPUVirtualAddress(), sizeof(float[3]) * 3, sizeof(float[3]) }
		};
		D3D12_INDEX_BUFFER_VIEW ibv = { mScreenTriIB->GetGPUVirtualAddress(), sizeof(unsigned short) * 3, DXGI_FORMAT_R16_UINT };
		cmdList->IASetIndexBuffer(&ibv);
		cmdList->IASetVertexBuffers(0, UARRAYLEN(vertexBuffers), vertexBuffers);
		cmdList->DrawInstanced(3, 1, 0, 0);
	}
};

