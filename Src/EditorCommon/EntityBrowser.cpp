#include "EntityBrowser.h"
#include "MainWindow.h"
#include "PropertyBrowser.h"

#include "../Engine/Scene.h"


#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qmenu.h>

namespace UEditor
{
	void UGetEdSwapnableEntities(TArray<TSubClass<Entity>>& out)
	{
		auto& types = GMetaSys()->GetAllTypes();
		for (auto type : types)
		{
			if (auto pClass = dynamic_cast<ClassInfo*>(type))
			{
				if (pClass->IsBaseOf<Entity>())
				{
					if (!pClass->IsDefaultConstructable()) continue;
					
					if (auto attr = pClass->GetAttribute<AttrEditorSpawnable>())
					{
						out.AddUnique(TSubClass<Entity>(pClass));
					}
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	EntityBrowserWidget::EntityBrowserWidget(QWidget* p /*= nullptr*/) : QWidget(p)
	{
		this->setObjectName("EntityBrowserWidget");
		this->setAcceptDrops(true);
		
		QVBoxLayout* vbl = new QVBoxLayout;
		this->setLayout(vbl);

		auto hl = new QHBoxLayout();

		mNameFilter = new FilterWidget;
		mNameFilter->setPlaceholderText("Name Filter");

		mClassFilter = new FilterWidget;
		mClassFilter->setPlaceholderText("Class Filter");

		hl->addWidget(mNameFilter);
		hl->addWidget(mClassFilter);
		hl->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));


		vbl->addLayout(hl);
		mTree = new EntityBrowserTree();
		vbl->addWidget(mTree);

		this->setLayout(vbl);

		mTree->setDragEnabled(true);
		mTree->viewport()->setAcceptDrops(true);
		mTree->setDropIndicatorShown(true);
		mTree->setAcceptDrops(true);
		mTree->setDragDropMode(QTreeWidget::DragDropMode::InternalMove);
		
		//filter change
		connect(mNameFilter, &QLineEdit::textChanged, this, [this](const QString& str) {
			this->ReBuildTree();
		});
		connect(mClassFilter, &QLineEdit::textChanged, this, [this](const QString& str) {
			this->ReBuildTree();
		});

		////////////dbl click on items
		connect(mTree, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem* item) {
			this->EvDoubleClickItem((EntityBrowserTreeItem*)item);
		});

		//tree ctx menu
		mTree->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
		connect(mTree, &QTreeWidget::customContextMenuRequested, this, &SelfT::EvEntityTreeCtxMenuRequested);

		//
		connect(mTree->itemDelegate(), &QAbstractItemDelegate::closeEditor, this, [this](QWidget *editor, QAbstractItemDelegate::EndEditHint hint) {
			EvTreeCloseEditor(editor, hint);
		});

		//expanding
		connect(mTree, &QTreeWidget::itemExpanded, this, [this](QTreeWidgetItem* item) {
			if (auto tmp = dynamic_cast<EntityBrowserTreeItem*>(item))
			{
				if (tmp->mEntity)
					tmp->mEntity->mEditorIsExpanded = true;
			}
		});
		//collapse
		connect(mTree, &QTreeWidget::itemCollapsed, this, [this](QTreeWidgetItem* item) {
			if (auto tmp = dynamic_cast<EntityBrowserTreeItem*>(item))
			{
				if (tmp->mEntity)
					tmp->mEntity->mEditorIsExpanded = false;
			}
		});

		//selection changed,
		connect(mTree, &QTreeWidget::itemSelectionChanged, this, &SelfT::EvItemSelectionChanged);

		mTree->mPostDrop.BindLambda( [this](){
			this->ReBuildTree();
		});
	}

	
	void EntityBrowserWidget::SetSene(Scene* pScene)
	{
		mScene = pScene;
		ReBuildTree();
	}

	//////////////////////////////////////////////////////////////////////////
	void EntityBrowserWidget::ReBuildTree()
	{
		mTree->clear();
		
		if (mScene)
		{
			if (mIsPrefabEditor)
			{
				UASSERT(false);
			}
			else
			{
				Entity* pChild = GetScene()->GetRootEntity()->GetEntityFirstChild();
				while (pChild)
				{
					if (CheckFilter(pChild))
					{
						auto pNewItem = new EntityBrowserTreeItem(pChild, (EntityBrowserTreeItem*)mTree->invisibleRootItem());
						CollectEntities(pNewItem);
						
					}

					pChild = pChild->GetDownEntity();
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void EntityBrowserWidget::CollectEntities(EntityBrowserTreeItem* parent)
	{
		Entity* pChild = parent->mEntity->GetEntityFirstChild();
		while (pChild)
		{
			if(CheckFilter(pChild))
			{
				auto pNewItem = new EntityBrowserTreeItem(pChild, parent);
				CollectEntities(pNewItem);
			}

			pChild = pChild->GetDownEntity();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool EntityBrowserWidget::CheckFilter(const Entity* pEntity) const
	{
		bool bName = mNameFilter->Check(pEntity->GetName());
		bool bClass = mClassFilter->Check(pEntity->GetClass()->GetName());

		if (bName && bClass)
			return true;

		const Entity* pChild = pEntity->GetEntityFirstChild();
		while (pChild)
		{
			if (CheckFilter(pChild))
				return true;
			pChild = pChild->GetDownEntity();
		}

		return false;
	}

	void EntityBrowserWidget::EvDoubleClickItem(EntityBrowserTreeItem*)
	{
		ULOG_MESSAGE("#TODO camera must focus on entity");
	}
	//////////////////////////////////////////////////////////////////////////
	void EntityBrowserWidget::EvEntityTreeCtxMenuRequested(const QPoint& point)
	{
		//point is local to mTree
		QMenu* menu = new QMenu(this);

		QPoint globalPoint = mTree->mapToGlobal(point);


		auto clickedItem = dynamic_cast<EntityBrowserTreeItem*>(mTree->itemAt(point));

		FillCtxMenu_Create(menu->addMenu("Create"), clickedItem);

		auto selectedItems = mTree->selectedItems();
		ULOG_MESSAGE("Selected items %", selectedItems.size());

		if (selectedItems.size() != 0)
		{
			menu->addAction("Delete");
		}
		menu->popup(globalPoint);
	}

	//////////////////////////////////////////////////////////////////////////
	void EntityBrowserWidget::EvTreeCloseEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint)
	{
		EntityBrowserTreeItem* item = (EntityBrowserTreeItem*)mTree->currentItem();
		
		if (!item || !item->mEntity)
			return;

		if (hint == QAbstractItemDelegate::SubmitModelCache)
		{
			item->mEntity->SetName(UQString2Name(item->text(ENTITY_NAME_COLUMN)));
		}
		else if(hint == QAbstractItemDelegate::RevertModelCache)
		{
			//revert name from entity
			item->setText(ENTITY_NAME_COLUMN, QString(item->mEntity->GetName()));
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void EntityBrowserWidget::EvItemSelectionChanged()
	{
		//deselect previously selected entities
		for (auto pEntity : mCurrentlySelectedEntities)
		{
			if (pEntity) pEntity->SetSelected(false);
		}
		mCurrentlySelectedEntities.RemoveAll();

		//set entities selected
		auto selecteds = this->mTree->selectedItems(); //returns the currently selected items
		for (auto item : selecteds)
		{
			Entity* pEntity = ((EntityBrowserTreeItem*)item)->mEntity;
			pEntity->SetSelected(true);
			mCurrentlySelectedEntities.Add(pEntity);

		}
		
		if (gMainWidget->GetPropertyBrowserDock())
		{
			gMainWidget->GetPropertyBrowserDock()->AttachObjects(mCurrentlySelectedEntities.CastedRef<Object*>());
		}
	}

	//////////////////////////////////////////////////////////////////////////
	void EntityBrowserWidget::CreateEntity(TSubClass<Entity> entClass, EntityBrowserTreeItem* parent)
	{
		static Name NewEntityName = "New Entity";

		Entity* pNewEntity = NewObject<Entity>(entClass);
		pNewEntity->SetName(NewEntityName);
		pNewEntity->RegisterEntityTo(parent ? parent->mEntity : GetScene()->GetRootEntity());

		auto newItem = new EntityBrowserTreeItem(pNewEntity, parent ? parent : (EntityBrowserTreeItem*)mTree->invisibleRootItem());

		mTree->scrollToItem(newItem);
	}

	//////////////////////////////////////////////////////////////////////////
	void EntityBrowserWidget::FillCtxMenu_Create(QMenu* pMenu, EntityBrowserTreeItem* pSelecteditem)
	{
		auto& types = GMetaSys()->GetAllTypes();
		for (auto type : types) //for each type
		{
			if (auto pClass = dynamic_cast<ClassInfo*>(type))
			{
				if (pClass->IsBaseOf<Entity>() && pClass->IsDefaultConstructable())
				{
					if (auto pAttr = pClass->GetAttribute<AttrEditorSpawnable>())
					{
						auto pTargetMenu = pMenu;
						if (pAttr->mCategory)
						{
							pTargetMenu = UMenuFindMenu(pMenu, pAttr->mCategory);
							if(pTargetMenu == nullptr)
								pTargetMenu = pMenu->addMenu(pAttr->mCategory);
						}

						QAction* pAction = pTargetMenu->addAction(UGetClassIcon(pClass), QString(pAttr->mName));
						connect(pAction, &QAction::triggered, this, [=]() {
							this->CreateEntity(pClass, pSelecteditem);
						});
						
					}
				}
			}
		}
	}

	void EntityBrowserWidget::SelectEntity(Entity* pEntity, bool bSelect)
	{
		SelectEntity(FindTreeItemByEntity(pEntity), bSelect);
	}

	void EntityBrowserWidget::SelectEntity(EntityBrowserTreeItem* pItem, bool bSelect)
	{
		if (pItem)
		{
			pItem->setSelected(bSelect); //selectionChanged will be raised
		}
	}

	EntityBrowserTreeItem* EntityBrowserWidget::FindTreeItemByEntity(Entity* pEntity) const
	{
		for (int i = 0; i < mTree->topLevelItemCount(); i++)
		{
			if (auto found = ((EntityBrowserTreeItem*)mTree->topLevelItem(i))->FindByEntity(pEntity))
				return found;
		}
		return nullptr;
	}

	void EntityBrowserWidget::Tick()
	{

	}

	EntityBrowserDockWidget::EntityBrowserDockWidget(QWidget* p) : QDockWidget(p)
	{
		this->setWindowTitle("Entity Browser");
		this->setMinimumSize(300, 300);
		this->setWidget(new EntityBrowserWidget);
	}

	void EntityBrowserDockWidget::Tick()
	{
		GetWidget()->Tick();
	}

	EntityBrowserTree::EntityBrowserTree(QWidget* p /*= nullptr*/) : QTreeWidget(p)
	{
		//this->setColumnCount(2);
		//this->setHeaderLabels(QStringList() << "Name" << "Class");
		this->setObjectName("EntityBrowserTree");
		this->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
		this->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
		this->setEditTriggers(QAbstractItemView::EditTrigger::EditKeyPressed);
		this->setIconSize(QSize(24, 24));
		this->setAlternatingRowColors(true);
		this->setHeaderHidden(true);
	}

	bool EntityBrowserTree::dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action)
	{
		ULOG_MESSAGE("");
		return SuperT::dropMimeData(parent, index, data, action);
	}

	void EntityBrowserTree::startDrag(Qt::DropActions supportedActions)
	{
		return SuperT::startDrag(supportedActions);
	}

	void EntityBrowserTree::dropEvent(QDropEvent* event)
	{
		auto selecteds = this->selectedItems();
		for (auto item : selecteds)
		{
			ULOG_MESSAGE("dropping %", item->text(0));
		}
		if (auto dst = this->itemAt(event->pos()))
		{
			ULOG_MESSAGE("dst %", dst->text(0));
		}
		
		SuperT::dropEvent(event);

		PostDrop();
		mPostDrop();
	}

	QMimeData * EntityBrowserTree::mimeData(const QList<QTreeWidgetItem*> items) const
	{
		//auto mime = new MimeEntityDrag;
		//mime->mItems = items;
		//return mime;

		QMimeData* md = SuperT::mimeData(items);
		return md;
	}
	
	static const QColor DROP_BACK_COLOR = QColor(200, 20, 10);

	void EntityBrowserTree::dragEnterEvent(QDragEnterEvent *event)
	{
		ULOG_MESSAGE("");
		SuperT::dragEnterEvent(event);
		return;

		if (auto mime = dynamic_cast<const MimeEntityDrag*>(event->mimeData()))
		{
			if (auto hoverItem = dynamic_cast<EntityBrowserTreeItem*>(this->itemAt(event->pos())))
			{
				hoverItem->setExpanded(true);
				hoverItem->setBackgroundColor(0, DROP_BACK_COLOR);
				event->acceptProposedAction();
			}
		}
	}

	void EntityBrowserTree::dragMoveEvent(QDragMoveEvent *event)
	{
		SuperT::dragMoveEvent(event);
		return;

		if (auto mime = dynamic_cast<const MimeEntityDrag*>(event->mimeData()))
		{
			if (mCurrentlyHover)
			{
				mCurrentlyHover->setBackgroundColor(0, this->invisibleRootItem()->backgroundColor(0));
				mCurrentlyHover = nullptr;
			}

			if (auto hoverItem = dynamic_cast<EntityBrowserTreeItem*>(this->itemAt(event->pos())))
			{
				mCurrentlyHover = hoverItem;
				hoverItem->setExpanded(true);
				hoverItem->setBackgroundColor(0, DROP_BACK_COLOR);
				event->acceptProposedAction();
			}
		}
	}

	void EntityBrowserTree::dragLeaveEvent(QDragLeaveEvent *event)
	{
		ULOG_MESSAGE("");
		SuperT::dragLeaveEvent(event);
	}

	//////////////////////////////////////////////////////////////////////////
	void EntityBrowserTree::PostDrop(QTreeWidgetItem* _item)
	{
		auto item = dynamic_cast<EntityBrowserTreeItem*>(_item);
		auto itemParent = dynamic_cast<EntityBrowserTreeItem*>(_item->parent());

		if (item && itemParent)
		{
			if (item->mEntity->GetParentEntity() != itemParent->mEntity)
			{
				item->mEntity->AttachTo(itemParent->mEntity);
			}
		}

		for(int iChild = 0; iChild < _item->childCount(); iChild++)
		{
			PostDrop(_item->child(iChild));
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void EntityBrowserTree::PostDrop()
	{
		for (int i = 0; i < this->topLevelItemCount(); i++)
		{
			auto item = this->topLevelItem(i);
			PostDrop(item);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	EntityBrowserTreeItem::EntityBrowserTreeItem(Entity* pEntity, EntityBrowserTreeItem* parent) : QTreeWidgetItem(parent)
	{
		mEntity = pEntity;

		this->setFlags(Qt::ItemFlag::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);

		if (pEntity)
		{
			this->setText(0, QString(pEntity->GetName()));
			this->setText(1, QString(pEntity->GetClass()->GetName()));
			this->setIcon(0, UGetClassIcon(pEntity->GetClass()));
			this->setExpanded(pEntity->mEditorIsExpanded);
		}
	}

	EntityBrowserTreeItem* EntityBrowserTreeItem::FindByEntity(Entity* pEntity) const
	{
		if (mEntity == pEntity)
			return (EntityBrowserTreeItem*)this;

		for (int i = 0; i < this->childCount(); i++)
		{
			if (auto pChild = dynamic_cast<EntityBrowserTreeItem*>(this->child(i)))
			{
				if (pChild->mEntity)
					if (auto found = FindByEntity(pChild->mEntity))
						return found;
			}
		}

		return nullptr;
	}

};
