#pragma once

#include "EditorCommonBase.h"

#include "Filter.h"
#include "../Engine/Entity.h"
#include "../Core/Meta.h"
#include "../Core/Delegate.h"

#include <QtWidgets/qtreeview.h>
#include <QtWidgets/qtreewidget.h>
#include <QtWidgets/qdockwidget.h>

namespace UEditor
{
	struct MimeEntityDrag : QMimeData
	{
		QList<QTreeWidgetItem*> mItems;
	};

	class UEDITORCOMMON_API EntityBrowserTreeItem : public QTreeWidgetItem
	{
	public:
		EntityBrowserTreeItem(Entity* pEntity, EntityBrowserTreeItem* parent);

		Entity* mEntity = nullptr;
		

		EntityBrowserTreeItem* FindByEntity(Entity* pEntity) const;
	};
	class UEDITORCOMMON_API EntityBrowserTree : public QTreeWidget
	{
	public:
		EntityBrowserTree(QWidget* p = nullptr);

		using SuperT = QTreeWidget;
	
		virtual bool dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action) override;
		virtual void startDrag(Qt::DropActions supportedActions) override;
		virtual void dropEvent(QDropEvent* event) override;
		virtual QMimeData *mimeData(const QList<QTreeWidgetItem*> items) const;

		TDelegate<void()> mPostDrop;

	protected:
		virtual void dragEnterEvent(QDragEnterEvent *event) override;
		virtual void dragMoveEvent(QDragMoveEvent *event) override;
		virtual void dragLeaveEvent(QDragLeaveEvent *event) override;

		void PostDrop(QTreeWidgetItem*);
		void PostDrop();
		EntityBrowserTreeItem* mCurrentlyHover = nullptr;
	};

	class UEDITORCOMMON_API EntityBrowserWidget : public QWidget
	{
	public:
		using SelfT = EntityBrowserWidget;
		static const unsigned ENTITY_NAME_COLUMN = 0;

		EntityBrowserWidget(QWidget* p = nullptr);

		FilterWidget* mNameFilter;
		FilterWidget* mClassFilter;
		EntityBrowserTree* mTree;
		
		Scene* mScene = nullptr;
		bool mIsPrefabEditor = false;

		void SetSene(Scene* pScene);
		Scene* GetScene() const { return mScene; }
		void ReBuildTree();
		void CollectEntities(EntityBrowserTreeItem* parent);
		bool CheckFilter(const Entity* pEntity) const;

		void EvDoubleClickItem(EntityBrowserTreeItem*);
		void EvEntityTreeCtxMenuRequested(const QPoint& p);
		void EvTreeCloseEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint);
		void EvItemSelectionChanged();
		void CreateEntity(TSubClass<Entity> entClass, EntityBrowserTreeItem* parent);
		void FillCtxMenu_Create(QMenu* menu, EntityBrowserTreeItem* selectedIfAny);

		void SelectEntity(Entity* pEntity, bool bSelect);
		void SelectEntity(EntityBrowserTreeItem* pItem, bool bSelect);

		EntityBrowserTreeItem* FindTreeItemByEntity(Entity*) const;

		TArray<Entity*> mCurrentlySelectedEntities;

		void Tick();
	};

	//////////////////////////////////////////////////////////////////////////
	class UEDITORCOMMON_API EntityBrowserDockWidget : public QDockWidget
	{
	public:
		EntityBrowserDockWidget(QWidget* p = nullptr);

		EntityBrowserWidget* GetWidget() const { return (EntityBrowserWidget*)this->widget(); }
		void Tick();
	};
};