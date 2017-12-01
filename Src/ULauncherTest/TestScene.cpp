#include "TestScene.h"
#include  <DirectXTK12/DDSTextureLoader.h>
#include <DirectXTK12/CommonStates.h>
#include "TestDraw.h"

TestScene::TestScene()
{
	{
		//mMonkeyMesh = AStaticMesh::ImportFromAssetFile(UFileOpenReadFull("../../Assets/Monkey.fbx"));
		//ULOG_SUCCESS("Mesh loaded");
	}
	//test screen quad to render target
	{
		auto tsq = new TestScreenQuadGen;
		tsq->CreateResources();
		
		HRESULT hr;

		unsigned Width = 64;
		unsigned Height = 64;

		CmdList* pCmdList = gGFX->mQueueMgr->GetNewGraphicCmdList();
		
		hr = gGFX->mDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, 128, 128, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET), D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&mRenderTargetTexture));
		
		UCHECK_DXRET(hr);

		mRTV = gGFX->mDescriptorMgr->AllocRTV();
		mSRV = gGFX->mDescriptorMgr->AllocCSU(true);

		gGFX->mDevice->CreateRenderTargetView(mRenderTargetTexture, nullptr, mRTV);
		gGFX->mDevice->CreateShaderResourceView(mRenderTargetTexture, nullptr, mSRV);

		pCmdList->SetViewport(0, 0, Width, Height);
		pCmdList->SetScissor(0, 0, Width, Height);
		
		{
			D3D12_RESOURCE_BARRIER barriers[] =
			{
				CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargetTexture, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET)
			};
			pCmdList->ResourceBarrier(barriers);
		}

		pCmdList->SetRenderTargets(1, mRTV, DescHandleDSV());
		
		tsq->Render(pCmdList);


		{
			D3D12_RESOURCE_BARRIER barriers[] =
			{
				CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargetTexture, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
			};
			pCmdList->ResourceBarrier(barriers);
		}

		pCmdList->Finish(true);
		
	}
}

void TestScene::Update(float delta)
{
	mCamera.Update(delta);
}

void TestScene::Render(CmdList* cmdList)
{
	mCamera.Update(1);

// 	Matrix4 local2Clip = mCamera.mMatrixWorldToClip *  Matrix4Translation(Vec3(0, 0, 0));
// 
// 	if (mEntityTestMesh)
// 	{
// 		auto mesh = mEntityTestMesh->mMesh;
// 
// 		mesh->BindBuffers(cmdList);
// 
// 		cmdList->SetPipelineState((ID3D12PipelineState*)mPSO->GetNativeHandle());
// 		cmdList->SetGraphicsRootSignature((ID3D12RootSignature*)mRS->GetNativeHandle());
// 		cmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
// 		cmdList->SetGraphicsRoot32BitConstants(0, 16, &local2Clip, 0);
// 		cmdList->DrawIndexedInstanced(mesh->mNumIndex, 1, 0, 0, 0);
// 	}
}



void TestCamera::Update(float delta)
{
	mCameraRotationSpeed = 100;

	mRotationEuler.y += GameInput::GetAnalogInput(GameInput::kAnalogMouseX) * mCameraRotationSpeed;
	//#note moving mouse up gives +Y
	mRotationEuler.x += GameInput::GetAnalogInput(GameInput::kAnalogMouseY) * -mCameraRotationSpeed;

	float fowradInputValue = 0;
	float righInputValue = 0;
	float upInputValue = 0;

	if (GameInput::IsPressed(GameInput::kKey_w))
		fowradInputValue = 1;
	if (GameInput::IsPressed(GameInput::kKey_s))
		fowradInputValue = -1;

	if (GameInput::IsPressed(GameInput::kKey_e))
		upInputValue = 1;
	if (GameInput::IsPressed(GameInput::kKey_q))
		upInputValue = -1;

	if (GameInput::IsPressed(GameInput::kKey_d))
		righInputValue = 1;
	if (GameInput::IsPressed(GameInput::kKey_a))
		righInputValue = -1;

	float moveSpeed = 10;

	Matrix4 cameraRotation = Matrix4RotationXYZ(mRotationEuler);
	mPosition += cameraRotation.TransformNormal(Vec3(righInputValue, 0, fowradInputValue)) * moveSpeed;
	mPosition.y += upInputValue * moveSpeed;

	mMatrixProjection = Matrix4PerspectiveFOV(mFOVDegree, 1, mZNear, mZFar);
	mMatrixView = Matrix4Translation(mPosition) * cameraRotation;
	mMatrixView.InvertAffine();

	mMatrixWorldToClip = mMatrixProjection * mMatrixView;
}

