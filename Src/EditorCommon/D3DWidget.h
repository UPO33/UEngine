#pragma once

#include "EditorCommonBase.h"

#include <QtWidgets/QWidget>

#include "../GFXDirectX/Context.h"

//////////////////////////////////////////////////////////////////////////
class QToolBar;

namespace UEditor
{
	//////////////////////////////////////////////////////////////////////////
	class UEDITORCOMMON_API D3DRenderWidget : public QWidget
	{
	public:
		D3DRenderWidget(QWidget* parent = nullptr);
		virtual ~D3DRenderWidget();

		virtual QPaintEngine* paintEngine() const override { return nullptr; }

		void CreateSwapChain();
		void ResizeSwapChain();

		IDXGISwapChain3*		mSwapChain = nullptr;
		ID3D12Resource*			mRTTextures[FRAME_COUNT] = {};
		DescHandleRTV			mRTVHead;
		unsigned				mFrameIndex = 0;
		unsigned				mBufferW = 0;
		unsigned				mBufferH = 0;

	protected:
		virtual void resizeEvent(QResizeEvent* evt) override;
		virtual void paintEvent(QPaintEvent* evt) override;
	};

	//////////////////////////////////////////////////////////////////////////
	class ViewportWidget : public QWidget
	{
	public:
		QToolBar* mToolBar;
		D3DRenderWidget* mRenderWidget;
		
		ViewportWidget(QWidget* parent = nullptr);
	};
	
};
