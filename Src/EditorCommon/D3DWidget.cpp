#include "D3DWidget.h"
#include "../Engine/Engine.h"

#include <QtGui/qevent.h>
#include <QtWidgets/qtoolbar.h>
#include <QtWidgets/qspinbox.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qcombobox.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/QtWidgets>

namespace UEditor
{
	//////////////////////////////////////////////////////////////////////////
	D3DRenderWidget::D3DRenderWidget(QWidget* parent /*= nullptr*/)
		: QWidget(parent)
	{
		setAttribute(Qt::WA_PaintOnScreen);
		setAttribute(Qt::WA_NativeWindow);

		void* windowHandle = (void*)(winId());

		UEnqueueWait(EET_Render, [this]() {
			this->CreateSwapChain();
		});
	}
	//////////////////////////////////////////////////////////////////////////
	D3DRenderWidget::~D3DRenderWidget()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void D3DRenderWidget::CreateSwapChain()
	{
#if 1
		UASSERT(UIsRenderThread());

		unsigned w = this->width();
		unsigned h = this->height();
		HWND wndHandle = (HWND)(this->winId());
		UASSERT(wndHandle);

		DXGI_SWAP_CHAIN_DESC1 scDesc = {};
		scDesc.Width = w;
		scDesc.Height = h;
		scDesc.BufferCount = FRAME_COUNT;
		scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		scDesc.Scaling = DXGI_SCALING_STRETCH;
		scDesc.Stereo = false;
		scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		scDesc.Flags = 0;
		scDesc.SampleDesc.Count = 1;
		scDesc.SampleDesc.Quality = 0;


		IDXGISwapChain1* swapChain = nullptr;
		UCHECK_DXRET(GetGFXContextDX12()->mGIFactory->CreateSwapChainForHwnd(GetGFXContextDX12()->mQueueMgr->GetGraphicQueue().GetCommandQueue(), wndHandle, &scDesc, nullptr, nullptr, &swapChain));
		UCHECK_DXRET(swapChain->QueryInterface(IID_PPV_ARGS(&mSwapChain)));

		ULOG_SUCCESS("SwapChain Created");

		UCHECK_DXRET(GetGFXContextDX12()->mGIFactory->MakeWindowAssociation(wndHandle, DXGI_MWA_NO_ALT_ENTER));

		//creating RTVs ...
		mRTVHead = GetGFXContextDX12()->mDescriptorMgr->AllocRTV(FRAME_COUNT);

		for (UINT iBuffer = 0; iBuffer < FRAME_COUNT; iBuffer++)
		{
			ID3D12Resource* renderTarget = nullptr;
			UCHECK_DXRET(mSwapChain->GetBuffer(iBuffer, IID_PPV_ARGS(&renderTarget)));

			mRTTextures[iBuffer] = renderTarget;

			GetGFXContextDX12()->mDevice->CreateRenderTargetView(renderTarget, nullptr, (mRTVHead + iBuffer).GetCpuHandle());
		}
#endif
	}

	void D3DRenderWidget::ResizeSwapChain()
	{
#if 1
		UASSERT(UIsRenderThread());

		ULOG_MESSAGE("Resizing swap chain to %x%", mBufferW, mBufferH);

		GetGFXContextDX12()->mQueueMgr->WaitForIdle();

		for (unsigned iFrame = 0; iFrame < FRAME_COUNT; iFrame++)
		{
			mRTTextures[iFrame]->Release();
		}

		DXGI_SWAP_CHAIN_DESC1 scDesc;
		mSwapChain->GetDesc1(&scDesc);

		HRESULT hr = mSwapChain->ResizeBuffers(0, mBufferW, mBufferH, DXGI_FORMAT_UNKNOWN, 0);
		UCHECK_DXRET(hr);

		for (UINT iBuffer = 0; iBuffer < FRAME_COUNT; iBuffer++)
		{
			ID3D12Resource* renderTarget = nullptr;
			UCHECK_DXRET(mSwapChain->GetBuffer(iBuffer, IID_PPV_ARGS(&renderTarget)));

			mRTTextures[iBuffer] = renderTarget;

			GetGFXContextDX12()->mDevice->CreateRenderTargetView(renderTarget, nullptr, (mRTVHead + iBuffer).GetCpuHandle());
		}
#endif

	}
	//////////////////////////////////////////////////////////////////////////
	void D3DRenderWidget::resizeEvent(QResizeEvent* evt)
	{
		mBufferW = evt->size().width();
		mBufferH = evt->size().height();
		
		UFlushTasks();
		UEnqueueWait(EET_Render, [this]() {
			this->ResizeSwapChain();
		});
	}

	void D3DRenderWidget::paintEvent(QPaintEvent* evt)
	{

	}

	ViewportWidget::ViewportWidget(QWidget* parent /*= nullptr*/) : QWidget(parent)
	{
		this->setLayout(new QVBoxLayout);
		mToolBar = new QToolBar;

		mToolBar->addAction("test", this, [this]() {

			QMenu* projection = new QMenu;
			QActionGroup* group = new QActionGroup(projection);
			QAction* ac0 = group->addAction("Perspective");
			QAction* ac1 = group->addAction("Top");
			QAction* ac2 = group->addAction("Left");
			ac0->setCheckable(true);
			ac1->setCheckable(true);
			ac2->setCheckable(true);
			projection->addAction(ac0);
			projection->addAction(ac1);
			projection->addAction(ac2);
			projection->popup(QPoint(66,6));

		});
		
		

		mToolBar->addSeparator();
		mToolBar->addWidget(new QSpinBox);

		QComboBox* cmbBox = new QComboBox;
		for (unsigned i = 0; i < 16; i++)
			cmbBox->addItem("item");
		


		mToolBar->addWidget(cmbBox);
		auto actionPopup = mToolBar->addAction("Popup");
		QMenu* m;
		
		{
			QWidget* w = new QWidget(nullptr, Qt::WindowType::Popup);
			w->setLayout(new QVBoxLayout);
			w->layout()->addWidget(new QSpinBox);
			w->layout()->addWidget(new QSpinBox);
			w->layout()->addWidget(new QLineEdit);

			connect(actionPopup, &QAction::triggered, this, [=](bool) {
				QRect r = mToolBar->actionGeometry(actionPopup);
				w->setGeometry(r.left(), r.bottom(), -1, -1);
				w->show();
			});
		}

		mRenderWidget = new D3DRenderWidget;
		this->layout()->addWidget(mToolBar);
		this->layout()->addWidget(mRenderWidget);
	}

};