#pragma once

#include "EditorCommonBase.h"
#include "../Engine/Asset.h"

#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/qmenubar.h>
#include <QtWidgets/qtoolbar.h>

namespace UEditor
{
	//////////////////////////////////////////////////////////////////////////
	class PropertyBrowserDock;
	
	//base class for asset viewers
	class UEDITORCOMMON_API AssetWindowBase : public QMainWindow
	{
	public:
		AssetWindowBase(QWidget* parent);

		virtual void Tick() {}

		Asset* GetAsset() const { return mAsset.Get(); }
		virtual void AttachAsset(Asset* pAsset) { mAsset = pAsset; }

		//the attached asset to this window
		TObjectPtr<Asset> mAsset;
		QToolBar* mMainToolBar = nullptr;

		virtual void OnSave();
	};

	//default asset viewer which just shows the properties of the asset
	class UEDITORCOMMON_API AssetWindowDefault : public AssetWindowBase
	{
	public:
		using SuperT = AssetWindowBase;

		AssetWindowDefault(QWidget* parent = nullptr);

		PropertyBrowserDock* mPropertyBrowser = nullptr;

		virtual void AttachAsset(Asset* pAsset) override;
	};
	
	class MainWidget;

	struct UEDITORCOMMON_API AssetViewer
	{
		AssetViewer(MainWidget* mainWidget);

		void OpenWindow(Asset*);
		//returns true if a window is open for the specified asset
		bool IsAssetOpen(const Asset*);
		//returns true if the specified asset is open and active
		bool IsAssetActive(const Asset*);

		//returns the index of this asset's window in tabbar if any, otherwise -1
		int GetAssetWindowIndex(const Asset*);

		MainWidget* mMainWidget = nullptr;
		TArray<AssetWindowBase*> mAssetWindows;
	};

	extern UEDITORCOMMON_API AssetViewer* gAssetViewer;
	inline AssetViewer* GAssetViewer() { return gAssetViewer; }
};