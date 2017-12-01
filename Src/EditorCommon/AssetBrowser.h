#pragma once

#include "EditorCommonBase.h"
#include "../Engine/Asset.h"

#include <QtWidgets/qdockwidget.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qtreewidget.h>

class QTreeWidget;

namespace UEngine
{
	class Entity;
};

namespace UEditor
{
	class FilterWidget;
	class AssetFileInfo;


	//////////////////////////////////////////////////////////////////
	class AssetBrowserTreeItem : public QTreeWidgetItem
	{
	public:
		using SelfT = AssetBrowserTreeItem;

		AssetBrowserTreeItem(AssetFileInfo* af = nullptr, AssetBrowserTreeItem* p = nullptr);

		bool IsFolder() const;

		AssetBrowserTreeItem* GetParent() const { return (AssetBrowserTreeItem*)this->parent(); }
		AssetFileInfo* mInfo = nullptr;
		
		template<typename TLambda> void ForEachChild(bool bDescendants, TLambda proc)
		{
			for (int iChild = 0; iChild < this->childCount(); iChild++)
			{
				SelfT* pChild = (SelfT*)this->child(iChild);
				proc(pChild);
				if (bDescendants)
					pChild->ForEachChild(true, proc);

			}
		}
		//search this one and its descendants and return the tree item that has the specified asset
		AssetBrowserTreeItem* FindByAsset(const Asset* pAsset) const;
	};

	//////////////////////////////////////////////////////////////////
	class AssetBrowserTree : public QTreeWidget
	{
	public:

		AssetBrowserTree(QWidget* parent = nullptr);

		QModelIndex IndexFromItem(QTreeWidgetItem* item, int column = 0) const
		{
			return this->indexFromItem(item, column);
		}
	};

	//////////////////////////////////////////////////////////////////
	class UEDITORCOMMON_API AssetBrowserWidget : public QWidget
	{
	public:
		AssetBrowserWidget(QWidget* parent = nullptr);

		using ItemT = AssetBrowserTreeItem;

		AssetBrowserTree*  mAssetsTree;
		FilterWidget* mClassFilter;
		FilterWidget* mNameFilter;

		void CollectAssets(AssetBrowserTreeItem* parent, AssetFileInfo* srcFile);

		void CollectAssets(QString dir, AssetBrowserTreeItem* parent = nullptr, const QString& traversed = QString());

		void GetAssetsFile(QString path);

		QString GetEngineAssetPath() const;
		QString GetProjectAssetPath() const;

		AssetBrowserTreeItem* mEngineRoot = nullptr;
		AssetBrowserTreeItem* mProjectRoot = nullptr;

		void ReCreateAssetTree();

		//check name and class filter on the specified asset 
		bool CheckFilter(const AssetFileInfo* item);

		//is called when user double clicked on tree item to open an asset
		void OpenAsset(AssetBrowserTreeItem*);
		//is called when user wants to delete and item
		void DeleteItem(AssetBrowserTreeItem*);

		//context menu for asset tree requested
		void AssetTreeCtxMenuReuested(const QPoint& point);

		//this must fill the create asset menu
		//@menu		the menu add asset
		//@ folder	the folder we must create assets to
		void FillContextMenu_Create(QMenu* menu, AssetBrowserTreeItem* folder);
		
		void EvCreateFolder(AssetBrowserTreeItem* folder);
		//is called when we finish editing a tree element
		void EvTreeCommitData(QWidget* widget);
		//is called when the editor widget of the tree element is closed
		//SubmitModelCache is sent when accepting change, RevertModelCache is sent when pressing escape
		void EvTreeCloseEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint);

		void FocusOnAsset(const Asset* pAsset);
		
		void EvDuplicateAsset(const AssetBrowserTreeItem* pAsset);

		void CreatePrefab(const AssetBrowserTreeItem* pFolder, const Entity* pEntity);

		static QIcon GetFolderIcon();

	protected:
		void EvTreeCreateAssetFinish(QWidget *editor, QAbstractItemDelegate::EndEditHint hint, TSubClass<Asset> assetClass, AssetBrowserTreeItem* folder);
		void EvTreeCreatFolderFinish(QWidget *editor, QAbstractItemDelegate::EndEditHint hint, AssetBrowserTreeItem* folder);

		virtual void dragEnterEvent(QDragEnterEvent *event) override;
		virtual void dragMoveEvent(QDragMoveEvent *event) override;
		virtual void dragLeaveEvent(QDragLeaveEvent *event) override;
		virtual void dropEvent(QDropEvent *event) override;

		//if not null this is the editing item for new asset or folder
		AssetBrowserTreeItem* mPendingCreateItem = nullptr;
		//if not folder this is the target asset class we should create
		TSubClass<Asset> mPendingCreateItemClass; 
		//whether we are about to create folder or asset
		bool mPendingCreateItemIsFolder = false;
		
		
		enum EEditingItem
		{
			EEI_NewAsset,
			EEI_NewFolder,
			EEI_DuplicateAsset,
		};
	};

	
	class UEDITORCOMMON_API AssetBrowserDockWidget : public QDockWidget
	{
	public:
		AssetBrowserDockWidget(QWidget* parent = nullptr);

		QListWidget* mTestLV;
		virtual void Tick(){}

		AssetBrowserWidget* GetWidget() const { return (AssetBrowserWidget*)this->widget(); }
	};

};