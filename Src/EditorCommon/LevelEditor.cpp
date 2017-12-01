#include "LevelEditor.h"

namespace UEditor
{

	void LevelEditorViewportWidget::Tick()
	{
		UEnqueue(EET_Render, [this]() {
			this->Render();
		});
	}

	void LevelEditorViewportWidget::Render()
	{
#if 1
		CmdList* pCmdList = GetGFXContextDX12()->mQueueMgr->GetNewGraphicCmdList();

		Color clearColor = Color(RandFloat01(), RandFloat01(), RandFloat01(), 1);

		this->mRenderWidget->mFrameIndex = this->mRenderWidget->mSwapChain->GetCurrentBackBufferIndex();

		auto curRTV = this->mRenderWidget->mRTVHead + this->mRenderWidget->mFrameIndex;
		auto curRTT = this->mRenderWidget->mRTTextures[this->mRenderWidget->mFrameIndex];

		{
			D3D12_RESOURCE_BARRIER barriers[] =
			{
				CD3DX12_RESOURCE_BARRIER::Transition(curRTT, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET)
			};
			pCmdList->ResourceBarrier(barriers);
		}

		pCmdList->SetRenderTargets(1, curRTV, DescHandleDSV());
		pCmdList->SetViewport(0, 0, width(), height());
		pCmdList->SetScissor(0, 0, width(), height());
		pCmdList->ClearRTV(curRTV, clearColor);




		

		{
			D3D12_RESOURCE_BARRIER barriers[] =
			{
				CD3DX12_RESOURCE_BARRIER::Transition(curRTT, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT)
			};
			pCmdList->ResourceBarrier(barriers);
		}

		pCmdList->Finish(true);

		this->mRenderWidget->mSwapChain->Present(1, 0);
#endif
	}

};