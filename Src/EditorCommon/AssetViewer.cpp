#include "AssetViewer.h"
#include "MainWindow.h"
#include "AssetInterface.h"
#include "AssetMgrEd.h"
#include "PropertyBrowser.h"

namespace UEditor
{

	AssetViewer::AssetViewer(MainWidget* mainWidget)
	{
		mMainWidget = mainWidget;
		
	}

	void AssetViewer::OpenWindow(Asset* pAsset)
	{
		int tabIndex = GetAssetWindowIndex(pAsset);
		if (tabIndex == -1)
		{
			AssetWindowBase* assetWnd = new AssetWindowBase(nullptr);
			assetWnd->AttachAsset(pAsset);

			mMainWidget->addTab(assetWnd, UGetClassIcon(pAsset->GetClass()), QString(pAsset->GetName()));
			
		}

		mMainWidget->setCurrentIndex(tabIndex);
	}

	bool AssetViewer::IsAssetOpen(const Asset* pAsset)
	{
		return GetAssetWindowIndex(pAsset) != -1;
	}

	bool AssetViewer::IsAssetActive(const Asset* pAsset)
	{
		return GetAssetWindowIndex(pAsset) == mMainWidget->currentIndex();
	}

	int AssetViewer::GetAssetWindowIndex(const Asset* pAsset)
	{
		//#note the tab at zero index is level editor and always open
		for (int iTab = 1; iTab < mMainWidget->tabBar()->count(); iTab++)
		{
			auto assetWindow = dynamic_cast<AssetWindowBase*>(mMainWidget->widget(iTab));
			UASSERT(assetWindow);
			if (assetWindow->GetAsset() == pAsset)
				return iTab;
		}
		return -1;
	}

	//////////////////////////////////////////////////////////////////////////
	AssetWindowBase::AssetWindowBase(QWidget* parent) : QMainWindow(parent)
	{
// 		QMenu* m = this->menuBar()->addMenu("Asset");
// 		m->addAction(QIcon(), "Save", []() {
// 
// 		}, QKeySequence(QKeySequence::Save));
// 
// 		m->addAction(QIcon(), "Close", []() {
// 
// 		},QKeySequence(QKeySequence::Close));
		
		//main toolbar
		{
			mMainToolBar = this->addToolBar("MainToolBar");

			mMainToolBar->addAction("Save", this, [this]() {
				this->OnSave();
			});
			mMainToolBar->addAction("Show In AssetBrowser", this, [this]() {
				gMainWidget->ShowAssetInAssetBrowser(this->GetAsset());
			});
		}
	}

	void AssetWindowBase::OnSave()
	{
		GAssetMgrEd()->SaveAsset(GetAsset());
	}

	//////////////////////////////////////////////////////////////////////////
	AssetWindowDefault::AssetWindowDefault(QWidget* parent /*= nullptr*/) : AssetWindowBase(parent)
	{
		mPropertyBrowser = new PropertyBrowserDock(this);
		this->addDockWidget(Qt::LeftDockWidgetArea, mPropertyBrowser);
	}

	void AssetWindowDefault::AttachAsset(Asset* pAsset)
	{
		SuperT::AttachAsset(pAsset);
		mPropertyBrowser->AttachObject(pAsset);
	}

};