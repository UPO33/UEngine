#pragma once

#include "EditorCommonBase.h"
#include "AssetViewer.h"

#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/qdockwidget.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qtabwidget.h>

namespace UEngine
{
	class Scene;
};

namespace UEditor
{
	class LogWidget;
	class LogDockWidget;
	class AssetBrowserDockWidget;
	class AssetBrowserWidget;
	class LevelEditorViewportWidget;
	class EntityBrowserDockWidget;
	class PropertyBrowserDock;

	//////////////////////////////////////////////////////////////////////////
	class UEDITORCOMMON_API MainEditorWindow : public QMainWindow
	{
	public:
		LogDockWidget*	mLog = nullptr;
		AssetBrowserDockWidget*	mAssetBrowser = nullptr;
		LevelEditorViewportWidget* mLevelEditor = nullptr;
		EntityBrowserDockWidget* mEntityBrowser = nullptr;
		PropertyBrowserDock* mPropertyBrowser = nullptr;
		Scene* mEditorScene = nullptr;

		MainEditorWindow(QWidget* parent = nullptr);

		void Tick();

		virtual QPaintEngine * paintEngine() const override;

	protected:
		virtual void paintEvent(QPaintEvent *event) override;

		void InitMenuBar();
		void InitToolBar();
		void CreateTestScene();
	};

	//////////////////////////////////////////////////////////////////////////
	class UEDITORCOMMON_API MainWidget : public QTabWidget
	{
	public:
		MainWidget(QWidget* parent = nullptr);
		
		using SuperT = QTabWidget;

		void Tick();

		MainEditorWindow * mMainTab;

		AssetBrowserWidget* GetAssetBrowser() const;

		void OpenAssetWindow(Asset*);
		//returns true if a window is open for the specified asset
		bool IsAssetWindowOpen(const Asset*);
		//returns true if the specified asset is open and active
		bool IsAssetWindowActive(const Asset*);

		//returns the index of this asset's window in tabbar if any, otherwise -1
		int GetAssetWindowIndex(const Asset*);

		void ShowAssetInAssetBrowser(const Asset*);

		MainEditorWindow* GetMainWindow() const { return mMainTab; }
		
		auto GetPropertyBrowserDock() const { return mMainTab->mPropertyBrowser; }
		auto GetAssetBrowserDock() const { return mMainTab->mAssetBrowser; }
		auto GetEntityBrowserDock() const { return mMainTab->mEntityBrowser; }

	protected:
		virtual void keyPressEvent(QKeyEvent *event) override;
		virtual void keyReleaseEvent(QKeyEvent *event) override;

	};

	extern UEDITORCOMMON_API MainWidget* gMainWidget;

};