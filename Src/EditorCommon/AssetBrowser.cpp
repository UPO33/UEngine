#include "AssetBrowser.h"

#include <QtWidgets/qlistview.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qtreewidget.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/QLabel>
#include <QtWidgets/QtWidgets>
#include <QtWidgets/qcolordialog.h>
#include <QtWidgets/qfiledialog.h>

#include "FlowLayout.h"
#include "Filter.h"
#include "Project.h"
#include "AssetMgrEd.h"
#include "AssetInterface.h"
#include "AssetViewer.h"
#include "MainWindow.h"
#include "EntityBrowser.h"

#include <QtWidgets/qstyleditemdelegate.h>

namespace UEditor
{
//////////////////////////////////////////////////////////////////////////

class TestItemDelegate : public QStyledItemDelegate
{
public:

	TestItemDelegate(QObject* parent) : QStyledItemDelegate(parent)
	{
	}

	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
	{
		QStyledItemDelegate::paint(painter, option, index);
	}


	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
	{
		return QStyledItemDelegate::sizeHint(option, index);
	}

};
//////////////////////////////////////////////////////////////////////////
AssetBrowserDockWidget::AssetBrowserDockWidget(QWidget* parent)
{
	this->setObjectName("AssetBrowserDockWidget");
	this->setWindowTitle("Asset Browser");
	this->setMinimumSize(QSize(444, 444));
	this->setWidget(new AssetBrowserWidget(nullptr));
}





//////////////////////////////////////////////////////////////////////////
AssetBrowserWidget::AssetBrowserWidget(QWidget* parent) : QWidget(parent)
{
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
	mAssetsTree = new AssetBrowserTree();
	vbl->addWidget(mAssetsTree);

	this->setLayout(vbl);
	
	mAssetsTree->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);
	mAssetsTree->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
	mAssetsTree->setEditTriggers(QAbstractItemView::EditTrigger::EditKeyPressed);
	mAssetsTree->setIconSize(QSize(32, 32));
		
// 	connect(mAssetsTree->itemDelegate(), &QAbstractItemDelegate::commitData, this, [this](QWidget* widget) {
// 		ULOG_MESSAGE("commit data");
// 		EvTreeCommitData(widget);
// 		//QString strNewText = reinterpret_cast<QLineEdit*>(widget)->text();
// 		//EVItemEditingFinished(strNewText);
// 	});

	connect(mAssetsTree->itemDelegate(), &QAbstractItemDelegate::closeEditor, this, [this](QWidget *editor, QAbstractItemDelegate::EndEditHint hint) {
		ULOG_MESSAGE("CloseEditor %", (int)hint);
		EvTreeCloseEditor(editor, hint);
	});



	////////////dbl click on items
	connect(mAssetsTree, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem* item) {
		this->OpenAsset((AssetBrowserTreeItem*)item);
	});

	//filter change
	connect(mNameFilter, &QLineEdit::textChanged, this, [this](const QString& str) {
		this->ReCreateAssetTree();
	});
	connect(mClassFilter, &QLineEdit::textChanged, this, [this](const QString& str) {
		this->ReCreateAssetTree();
	});


	mAssetsTree->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	connect(mAssetsTree, &QTreeWidget::customContextMenuRequested, this, &AssetBrowserWidget::AssetTreeCtxMenuReuested);

	connect(mAssetsTree, &QTreeWidget::itemChanged, this, [this](QTreeWidgetItem* item, int column) {
		ULOG_MESSAGE("ItemChanged %", item->text(0));
	});

	ReCreateAssetTree();


}

void AssetBrowserWidget::CollectAssets(QString dir, AssetBrowserTreeItem* parent, const QString& traversed)
{
#if 0
	QDirIterator iter(dir, QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);

	while (iter.hasNext())
	{
		//eg: ../content/file.txt
		auto filepath = iter.next();

		QFileInfo fileInfo = iter.fileInfo();

		if (fileInfo.isFile())
		{
			ULOG_MESSAGE("AD %", fileInfo.absoluteDir().absolutePath().toStdString());
			ULOG_MESSAGE("BN %", fileInfo.baseName().toStdString());
			ULOG_MESSAGE("FN %", iter.fileName().toStdString());

			//filter check
			auto item = new AssetBrowserTreeItem(parent);
			item->mAbolutePath = fileInfo.absoluteFilePath();
				
			//e.g StaticMeshes/Cube
			QString assetName = traversed.isEmpty() ? fileInfo.fileName() : traversed + "/" + fileInfo.fileName();
			item->mAssetName = UQString2Name(assetName);

			item->setToolTip(0, item->mAssetName.CStr());
			item->setText(0, fileInfo.fileName());
				
			mAssetsTree->addTopLevelItem(item);
		}
		else
		{
			auto item = new AssetBrowserTreeItem(parent);
			item->mAbolutePath = fileInfo.absoluteFilePath();
			item->setText(0, fileInfo.fileName());
			item->setIcon(0, GetFolderIcon());
				
			mAssetsTree->addTopLevelItem(item);

			CollectAssets(fileInfo.absoluteFilePath(), item, traversed + QDir::separator() + fileInfo.fileName());
		}
	}
#endif
}

void AssetBrowserWidget::CollectAssets(AssetBrowserTreeItem* parent,  AssetFileInfo* srcFile)
{
	for (auto child : srcFile->mChildren)
	{
		if (CheckFilter(child))
		{
			auto newItem = new AssetBrowserTreeItem(child, parent);
			CollectAssets(newItem, child);
		}
	}
}

void AssetBrowserWidget::GetAssetsFile(QString rootDir)
{
	QDirIterator iter(rootDir, QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
		
	while (iter.hasNext()) 
	{
		//eg: ../content/file.txt
		auto filepath = iter.next();
			
		QFileInfo fileInfo = iter.fileInfo();
			
		if (fileInfo.isFile())
		{
			ULOG_MESSAGE("AD %", fileInfo.absoluteDir().absolutePath().toStdString());
			ULOG_MESSAGE("BN %", fileInfo.baseName().toStdString());
			ULOG_MESSAGE("FN %", iter.fileName().toStdString());

			//filter check
		}
		else
		{
			ULOG_MESSAGE("AD %", fileInfo.absoluteFilePath().toStdString());
			GetAssetsFile(fileInfo.absoluteFilePath());
		}
			
// 
// 			QString filename = fileInfo.fileName();		//eg file.txt
// 			QString assetName = prefix.isEmpty() ? filename : prefix + '/' + filename;
// 			QString absFilePath = fileInfo.absoluteFilePath();
// 
// 			if (fileInfo.isFile())
// 			{
// 				if (FilterCheck(assetName))
// 				{
// 					if (GAssetSys()->CheckFile(ToString(absFilePath))) // is asset file ?
// 					{
// 						AssetBrowserItem2* newItem = new AssetBrowserItem2(assetName, absFilePath, filename, false, parentItem);
// 						ui->assetsTree->addTopLevelItem(newItem);
// 					}
// 				}
// 			}
// 			else
// 			{
// 				AssetBrowserItem2* newItem = new AssetBrowserItem2(assetName, absFilePath, filename, true, parentItem);
// 				ui->assetsTree->addTopLevelItem(newItem);
// 				AddDirContents(filepath, newItem, assetName);
// 			}
	}

}

QString AssetBrowserWidget::GetEngineAssetPath() const
{
	return QDir("../../Assets/").absolutePath();
}

QString AssetBrowserWidget::GetProjectAssetPath() const
{
	return QDir("../../TestProject/Assets/").absolutePath();
}

void AssetBrowserWidget::ReCreateAssetTree()
{
	SafeDelete(mEngineRoot);
	SafeDelete(mProjectRoot);

	mAssetsTree->clear();

	mProjectRoot = new AssetBrowserTreeItem(GAssetMgrEd()->mProjectRoot, nullptr);
	mProjectRoot->setFlags(Qt::ItemFlag::ItemIsEnabled);
	mProjectRoot->setText(0, "Project");
	mProjectRoot->setIcon(0, GetFolderIcon());

	mEngineRoot = new AssetBrowserTreeItem(GAssetMgrEd()->mEngineRoot, nullptr);
	mEngineRoot->setFlags(Qt::ItemFlag::ItemIsEnabled);
	mEngineRoot->setText(0, "Engine");
	mEngineRoot->setIcon(0, GetFolderIcon());

	CollectAssets(mEngineRoot, GAssetMgrEd()->mEngineRoot);
	CollectAssets(mProjectRoot, GAssetMgrEd()->mProjectRoot);

	mAssetsTree->addTopLevelItem(mEngineRoot);
	mAssetsTree->addTopLevelItem(mProjectRoot);
	mAssetsTree->expandItem(mProjectRoot);
	mAssetsTree->expandItem(mEngineRoot);

	mAssetsTree->expandAll();
}

	

//////////////////////////////////////////////////////////////////////////
bool AssetBrowserWidget::CheckFilter(const AssetFileInfo* item)
{
	bool bName = mNameFilter->Check(item->mFilename);
	
	bool bClass = true;

	if (item->mAssetData)
	{
		bClass = mClassFilter->Check(item->mAssetData->GetClassName());
	}
	
	if (bName && bClass) 
		return true;

	for (const AssetFileInfo* child : item->mChildren)
	{
		if (CheckFilter(child))
			return true;
	}

	return false;
}

void AssetBrowserWidget::OpenAsset(AssetBrowserTreeItem* pItem)
{
	if (pItem && !pItem->IsFolder())
	{
		if (Asset* pAsset = GAssetMgrEd()->LoadAsset(pItem->mInfo))
		{
			gMainWidget->OpenAssetWindow(pAsset);
		}
	}
}
	
void AssetBrowserWidget::DeleteItem(AssetBrowserTreeItem*)
{

}

void AssetBrowserWidget::AssetTreeCtxMenuReuested(const QPoint& point)
{
	QMenu* menu = new QMenu(this);

	QPoint globalPoint = mAssetsTree->mapToGlobal(point);


	if (auto selectedItem = dynamic_cast<AssetBrowserTreeItem*>(mAssetsTree->itemAt(point)))
	{
		if(selectedItem != mEngineRoot && selectedItem != mProjectRoot)
		{
			QAction* actDelete = menu->addAction("Delete");
			connect(actDelete, &QAction::triggered, this, [this, selectedItem](bool) {
				this->DeleteItem(selectedItem);
			});
		}


		QAction* actShowInExplorer = menu->addAction("Show In Explorer");
		connect(actShowInExplorer, &QAction::triggered, this, [selectedItem]() {
			UShowInExplorer(selectedItem->mInfo->mAbsolutePath);
		});
			
		//////rename
		menu->addAction(QIcon(), "Rename", this, [selectedItem, this]() {
			if (selectedItem)
				this->mAssetsTree->editItem(selectedItem);
		});

		if (selectedItem->IsFolder()) //we selected folder?
		{
			QMenu* createMenu = menu->addMenu("Create");
			FillContextMenu_Create(createMenu, selectedItem);

			createMenu->addAction("Folder", this, [this, selectedItem](bool) {
				this->EvCreateFolder(selectedItem);
			});
		}
		else
		{
			QAction* actOpen = menu->addAction("Open");
			connect(actOpen, &QAction::triggered, this, [this, selectedItem](bool) {
				this->OpenAsset(selectedItem);
			});
			

			menu->addAction("Duplicate", this, [this, selectedItem]() {
				EvDuplicateAsset(selectedItem);
			});

			//per asset context menu
			{
				menu->addSeparator();

				UASSERT(selectedItem->mInfo->mAssetData);

				TSubClass<Asset> selectedAssetClass = selectedItem->mInfo->mAssetData->GetClass();
				if (AssetInterface* pSelectedAssetInterface = AssetInterface::GetInterface(selectedAssetClass))
				{
					pSelectedAssetInterface->OnContextMenu(selectedItem->mInfo, menu);
				}

			}
		}

	}
	else
	{

	}


	menu->popup(globalPoint);
}

	
void AssetBrowserWidget::FillContextMenu_Create(QMenu* pMenu, AssetBrowserTreeItem* pFolder) 
{
	auto interfaces = AssetInterface::GetInterfaces();
	for (AssetInterface* pIterAssetInterface : interfaces)
	{
		if (pIterAssetInterface && pIterAssetInterface->mIsUserCreatable) //for each user create able asset
		{
			if (const ClassInfo* pAssetClass = pIterAssetInterface->mClass)
			{
				pMenu->addAction(UGetClassIcon(pAssetClass), pAssetClass->GetName().CStr(), this, [=]() {
					{
						pFolder->setSelected(false);
						pFolder->setExpanded(true);

						auto pending = new AssetBrowserTreeItem(nullptr, pFolder);
						pending->setText(0, "New Asset");
						pending->setIcon(0, UGetClassIcon(pAssetClass));

						this->mAssetsTree->editItem(pending);
						this->mAssetsTree->setItemSelected(pending, true);

						this->mPendingCreateItem = pending;
						this->mPendingCreateItemClass = pAssetClass;
						this->mPendingCreateItemIsFolder = false;

					}
				});
			}
		}
	}
}


void AssetBrowserWidget::EvCreateFolder(AssetBrowserTreeItem* folder)
{
	AssetBrowserTreeItem* newFolder = new AssetBrowserTreeItem(nullptr, folder);
	newFolder->setText(0, "New Folder");
	newFolder->setIcon(0, GetFolderIcon());

	folder->setExpanded(true);
	mAssetsTree->clearSelection();
	mAssetsTree->setItemSelected(newFolder, true);
	mAssetsTree->editItem(newFolder);
	
	this->mPendingCreateItem = newFolder;
	this->mPendingCreateItemClass = nullptr;
	this->mPendingCreateItemIsFolder = true;
}

void AssetBrowserWidget::EvTreeCommitData(QWidget* widget)
{

}

void AssetBrowserWidget::EvTreeCloseEditor(QWidget* editor, QAbstractItemDelegate::EndEditHint hint)
{
	QLineEdit* editorLineEdit = dynamic_cast<QLineEdit*>(editor);

	if (mPendingCreateItem) //were we typing for the name of new asset or folder?
	{
		if (mPendingCreateItemIsFolder)
		{
			EvTreeCreatFolderFinish(editor, hint, mPendingCreateItem);
		}
		else //is asset
		{
			EvTreeCreateAssetFinish(editor, hint, mPendingCreateItemClass, mPendingCreateItem);
		}
	}
	else //renaming existing item
	{
		if (AssetBrowserTreeItem* curItem = (AssetBrowserTreeItem*)mAssetsTree->currentItem())
		{
			if (hint == QAbstractItemDelegate::EndEditHint::SubmitModelCache)
			{
				if (GAssetMgrEd()->RenameAsset(curItem->mInfo, curItem->text(0)))
				{
				}
				else
				{
					//renaming failed so retrieve the previous name
					curItem->setText(0, curItem->mInfo->mFilename);
				}
			}
		}

	}

	mPendingCreateItem = nullptr;
}



void AssetBrowserWidget::FocusOnAsset(const Asset* pAsset)
{
	if (pAsset && pAsset->GetAssetData())
	{
		AssetBrowserTreeItem* foundItem = mEngineRoot->FindByAsset(pAsset);
		if (foundItem == nullptr)
			foundItem = mProjectRoot->FindByAsset(pAsset);

		if (foundItem)
		{
			mAssetsTree->clearSelection();
			//mAssetsTree->setItemSelected(foundItem, true);
			auto flag = QItemSelectionModel::SelectionFlag::ClearAndSelect;
			mAssetsTree->setCurrentItem(foundItem, 0, flag);
			mAssetsTree->scrollToItem(foundItem);
		}
	}
}


//////////////////////////////////////////////////////////////////////////
void AssetBrowserWidget::EvDuplicateAsset(const AssetBrowserTreeItem* pAssetItem)
{
	UASSERT(pAssetItem);

	QString dupName;
	dupName = pAssetItem->mInfo->mFilename + QString("Dup");
	
	auto pAsset = GAssetMgrEd()->LoadAsset(pAssetItem->mInfo);
	GAssetMgrEd()->CreateAsset(pAssetItem->GetParent()->mInfo, dupName, pAssetItem->mInfo->GetAssetClass(), pAsset);
	
	this->ReCreateAssetTree();


// 	{
// 		AssetBrowserTreeItem* dupItem = new AssetBrowserTreeItem(nullptr, pAssetItem->GetParent());
// 		dupItem->setText(0, pAssetItem->mInfo->mFilename + QString("_Dup"));
// 		dupItem->setIcon(0, GetFolderIcon());
// 
// 		mAssetsTree->clearSelection();
// 		mAssetsTree->setItemSelected(dupItem, true);
// 		mAssetsTree->scrollToItem(dupItem);
// 		mAssetsTree->editItem(dupItem);
// 
// 		this->mPendingCreateItem = dupItem;
// 		this->mPendingCreateItemClass = nullptr;
// 		this->mPendingCreateItemIsFolder = false;
// 	}
}

void AssetBrowserWidget::CreatePrefab(const AssetBrowserTreeItem* pFolder, const Entity* pEntity)
{
	auto clonedEntity = pEntity->Clone();
	
}

QIcon AssetBrowserWidget::GetFolderIcon()
{
	static QIcon S("../../Resources/Icons/Folder.png");
	return S;
}

void AssetBrowserWidget::EvTreeCreateAssetFinish(QWidget *editor, QAbstractItemDelegate::EndEditHint hint, TSubClass<Asset> assetClass, AssetBrowserTreeItem* pending)
{
	QLineEdit* editorLineEdit = dynamic_cast<QLineEdit*>(editor);
	auto assetFileName = editorLineEdit->text();

	if (hint == QAbstractItemDelegate::EndEditHint::RevertModelCache)
	{
		SafeDelete(pending);
		return;
	}
	
	if (AssetFileInfo* pNewlyCreatedAsset = GAssetMgrEd()->CreateAsset(pending->GetParent()->mInfo, assetFileName, assetClass))
	{
		//created asset must have Asset data
		UASSERT(pNewlyCreatedAsset->mAssetData);
		pending->mInfo = pNewlyCreatedAsset;
	}
	else
	{
		ULOG_WARN("failed to create asset. class:[%], filename:[%]", assetClass->GetName(), assetFileName);
		SafeDelete(pending);
		return;
	}
}

void AssetBrowserWidget::EvTreeCreatFolderFinish(QWidget *editor, QAbstractItemDelegate::EndEditHint hint, AssetBrowserTreeItem* pending)
{
	QLineEdit* editorLineEdit = dynamic_cast<QLineEdit*>(editor);
	auto newFolderText = editorLineEdit->text();

	if (hint == QAbstractItemDelegate::EndEditHint::RevertModelCache)
	{
		SafeDelete(pending);
		return;
	}
	else
	{
		if (AssetFileInfo* newFolder = GAssetMgrEd()->CreateNewFolder(pending->GetParent()->mInfo, newFolderText))
		{
			pending->mInfo = newFolder;
			ULOG_SUCCESS("folder [%] created", newFolderText);
		}
		else
		{
			ULOG_WARN("failed to create folder [%]", newFolderText);
			SafeDelete(pending);
			return;
		}
	}
}

void AssetBrowserWidget::dragEnterEvent(QDragEnterEvent * e)
{
	if (e->mimeData()->hasUrls())
	{
		auto urls = e->mimeData()->urls();
		e->acceptProposedAction();

// 		if (urls.size() == 1)
// 		{
// 			QString filename = urls[0].toLocalFile();
// 			if (!QDir(filename).exists()) // is not folder?
// 			{
// 				String ext = PathGetExt(ToString(filename));
// 
// 				if (AssetConverter::ExtIsSupported(ext))
// 
// 					e->acceptProposedAction();
// 
// 			}
// 		}
	}
	else
	{
		if (e->source())
			ULOG_WARN("%", e->source()->objectName());

		ULOG_WARN("%", e->mimeData()->metaObject()->className());
		
		e->acceptProposedAction();
	}
}

void AssetBrowserWidget::dragMoveEvent(QDragMoveEvent *event)
{
	QPoint reletivePos = mAssetsTree->mapFromGlobal(mapToGlobal(event->pos()));	//(event->pos() is relative to widget
	AssetBrowserTreeItem* itemUnderCursor = (AssetBrowserTreeItem*)mAssetsTree->itemAt(reletivePos);
	if (itemUnderCursor &&  itemUnderCursor->IsFolder()) //are we dropping on folder ?
	{
		event->setDropAction(Qt::DropAction::CopyAction);
		mAssetsTree->expandItem(itemUnderCursor);
		mAssetsTree->setCurrentItem(itemUnderCursor);
		//item->setTextColor(0, QColor(255, 0, 0));
		//item->setBackgroundColor(0, QColor(1,0,0));
		event->accept();
	}
	else
	{
		event->ignore();
	}
}

void AssetBrowserWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
	QWidget::dragLeaveEvent(event);
}
	
void AssetBrowserWidget::dropEvent(QDropEvent* event)
{
	QPoint reletivePos = mAssetsTree->mapFromGlobal(mapToGlobal(event->pos()));	//(event->pos() is relative to widget
	AssetBrowserTreeItem* item = (AssetBrowserTreeItem*)mAssetsTree->itemAt(reletivePos);
	if (item && item->IsFolder() && event->mimeData()) 
	{
		if(event->mimeData()->hasUrls()) //importing asset
		{
			auto urls = event->mimeData()->urls();
			for (const QUrl& url : urls)
			{
				QString filename = url.toLocalFile();
				if (!QDir(filename).exists()) // is not folder?
				{
					// 					if (AssetImporterBase* importer = AssetImporterBase::GetImporter(filename))
					// 					{
					// 						if (auto importOption = importer->OnCreateImportOptionWidget())
					// 						{
					// 
					// 						}
					// 
					// 					}
					ULOG_MESSAGE("Importing asset %", filename);
				}
			}
		}
		else if (auto src = dynamic_cast<EntityBrowserTree*>(event->source()))
		{
			ULOG_MESSAGE("Make prefab");
			
			auto droppedItems = src->selectedItems();
			for (auto iter : droppedItems)
			{
				auto drpItem = reinterpret_cast<EntityBrowserTreeItem*>(iter);
				if(drpItem->mEntity)
					CreatePrefab(item, drpItem->mEntity);
			}
		}
	}
}

AssetBrowserTree::AssetBrowserTree(QWidget* parent) : QTreeWidget(parent)
{
	this->setObjectName("AssetBrowserTree");
	this->setHeaderHidden(true);
	this->setColumnCount(1);
	this->setDropIndicatorShown(true);
	this->setAcceptDrops(true);
	this->setDragEnabled(true);
	this->setDragDropMode(QAbstractItemView::InternalMove);
}

AssetBrowserTreeItem::AssetBrowserTreeItem(AssetFileInfo* af, AssetBrowserTreeItem* p /*= nullptr*/) : QTreeWidgetItem(p)
{
	mInfo = af;
	this->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
	if (af)
	{
		this->setIcon(0, af->IsFolder() ? AssetBrowserWidget::GetFolderIcon() : UGetClassIcon(af->GetAssetClass()));
		this->setText(0, af->mFilename);
		this->setToolTip(0, af->mAbsolutePath);

		if (af->IsFolder())
			this->setFlags(this->flags() | Qt::ItemIsDropEnabled);
	}
}

bool AssetBrowserTreeItem::IsFolder() const
{
	return mInfo->mIsFolder;
}

AssetBrowserTreeItem* AssetBrowserTreeItem::FindByAsset(const Asset* pAsset) const
{
	if (mInfo && mInfo->mAssetData)
		if (mInfo && mInfo->mAssetData == pAsset->GetAssetData())
			return (SelfT*)this;

	for (int iChild = 0; iChild < this->childCount(); iChild++)
	{
		SelfT* pChild = (SelfT*)this->child(iChild);
		if (auto found = pChild->FindByAsset(pAsset))
			return found;
	}

	return nullptr;
}

};