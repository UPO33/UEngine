#pragma once

#include "EditorCommonBase.h"
#include "Filter.h"

#include "../Core/Object.h"
#include "../Core/ObjectPtr.h"
#include "../Core/Meta.h"


#include <QtWidgets/qwidget.h>
#include <QtWidgets/qdockwidget.h>
#include <QtWidgets/qtreewidget.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QtWidgets>
#include <QtCore/qabstractitemmodel.h>


namespace UEditor
{
	//////////////////////////////////////////////////////////////////////////
	class PropertyBrowserWidget;

	//////////////////////////////////////////////////////////////////////////
	struct PRPParam
	{
		void* GetValuePtr() {}
		void SetValuePtr(void* pValue) {}

		const PropertyInfo* mPropertyInfo;
		void* mInstance;
		int mArrayIndex = -1;

		void* GetValuePtr() const
		{

		}
	};
	//////////////////////////////////////////////////////////////////////////
	class PBBaseWidget : public QWidget
	{
	public:
		PBBaseWidget(PRPParam param, QWidget* parent = nullptr) : QWidget(parent), mParam(param)
		{
			setLayout(new QHBoxLayout);
			layout()->setMargin(1);
			layout()->setSpacing(1);

			//set disabled if has uueditable attribute
			if (param.mPropertyInfo->GetAttribute<AttrUneditable>())
				this->setDisabled(true);

			//set tooltip to name of property's type
			if (param.mArrayIndex != -1)
				setToolTip(UToQString(param.mPropertyInfo->GetArg0().GetTypeName()));
			else
				setToolTip(UToQString(param.mPropertyInfo->GetSelf().GetTypeName()));
		}
		PRPParam mParam;
	};


	//////////////////////////////////////////////////////////////////////////
	class PBItemBase
	{
	public:
		PBItemBase* mParent = nullptr;
		TArray<PBItemBase*> mChildren;

		PBItemBase(PBItemBase* parent = nullptr)
		{
			if (parent)
			{
				mParent = parent;
				parent->mChildren.Add(this);
			}
		}
		virtual ~PBItemBase()
		{
			for (auto pChild : mChildren)
				delete pChild;

			mChildren.RemoveAll();
		}
		unsigned GetRowIndex() const
		{
			if (mParent)
				return mParent->mChildren.Find((PBItemBase*)this);
			return 0;
		}
		virtual const QString& GetText(int column)
		{
			static QString EmptyString;
			return EmptyString;
		}
		virtual QWidget* CreateWidget(int columnIndex) { return nullptr; }

		template<typename TLambda> void ForEachChild(bool bDescendants, TLambda proc)
		{
			for (auto pChild : mChildren)
			{
				proc(pChild);
				if (bDescendants)
					pChild->ForEachChild(true, proc);
			}
		}
		virtual void CreateChildren() {}
		virtual PropertyBrowserWidget* GetPropertyBrowser() const;
	};
	//////////////////////////////////////////////////////////////////////////
	class PBItemCategory : public PBItemBase
	{
	public:
		PBItemCategory(PBItemBase* parent, const QString& categoryName) : PBItemBase(parent), mCategoryName(categoryName)
		{
		}
		QString mCategoryName;

		const QString& GetText(int column) override
		{
			return mCategoryName;
		}

	};
	//////////////////////////////////////////////////////////////////////////
	class PBItemRoot : public PBItemBase
	{
	public:
		Object* mObject = nullptr;
		PropertyBrowserWidget* mPropertyBrowser = nullptr;

		PBItemCategory* FindOrAddCategory(const QString& categoryName)
		{
			for (auto pChild : mChildren)
			{
				if(auto childAsCategory = dynamic_cast<PBItemCategory*>(pChild))
				{
					if (childAsCategory->mCategoryName.compare(categoryName, Qt::CaseInsensitive) == 0)
						return childAsCategory;
				}
			}

			auto newCategory = new PBItemCategory(this, categoryName);
			return newCategory;
		}
		void CreateChildren() override;
		virtual PropertyBrowserWidget* GetPropertyBrowser() const override;
	};
	//////////////////////////////////////////////////////////////////////////
	class PBItemProperty : public PBItemBase
	{
	public:
		QWidget* mPropertyWidget = nullptr;
		const PropertyInfo* mPropertyInfo = nullptr;
		int mArrayIndex = -1;
		QString mPropertyName;
		void* mInstance = nullptr;
		Object* mObject = nullptr;

		PBItemProperty(PBItemBase* parent) : PBItemBase(parent) 
		{
		}
		//returns the object this property belongs to
		Object* GetObject() const 
		{
			return mObject;
		}
		void* GetInstance() const 
		{
			return nullptr; 
		}

		virtual QString& GetText(int column) override
		{
			if (mPropertyName.isNull())
			{
				mPropertyName = UToQString(mPropertyInfo->GetName());
			}
			return mPropertyName;
		}
		virtual void InitProperty(const PropertyInfo* pProperty)
		{

		}

		virtual QWidget* CreateWidget(int column) override
		{
			if (nullptr == mPropertyWidget)
				mPropertyWidget = CreatePropertyWidget();
			return mPropertyWidget;
		}
		virtual QWidget* CreatePropertyWidget()
		{
			return nullptr;
		}
		bool IsArrayElement() const { return mArrayIndex != -1; }
		const TriTypeData& GetPropertyType() const
		{
			if (mArrayIndex != -1)
				return mPropertyInfo->GetSelf();
			else
				return mPropertyInfo->GetArg0();
		}
		virtual void UpdateWidgetValue(){}
		virtual void UpdateValue() {}
		
		void InvokeOnPropertyChange( bool bPre)
		{
		
		}
		void WidgetValueChanged()
		{
			InvokeOnPropertyChange(true);
			UpdateValue();
			InvokeOnPropertyChange(false);
		}
		template<typename T> void SetValueMapped(T newValue)
		{
			*((T*)GetValuePtr()) = newValue;
		}
		void* GetValuePtr() const;

		template<typename T> T* GetPropertyWidget() const
		{
			return dynamic_cast<T*>(mPropertyWidget);
		}
		static PBItemProperty* StaticCreateProperty(const TriTypeData& type, PBItemBase* pParent);
		static PBItemProperty* StaticCreateProperty(const PropertyInfo* pProperty, PBItemBase* pParent);
	};

	//////////////////////////////////////////////////////////////////////////
	class PBItemStruct : public PBItemProperty
	{
	public:
		PBItemStruct(PBItemBase* parent) : PBItemProperty(parent)
		{
		}
		void CreateChildren() override;
		//returns the class of this struct e.g Vec2, Vec3
		const ClassInfo* GetStructClass() const
		{
			return dynamic_cast<const ClassInfo*>(GetPropertyType().GetPtr());
		}
		//returns pointer to the head of struct
		void* GetStructPtr() const { return GetValuePtr(); }
		
	};
	//////////////////////////////////////////////////////////////////////////
	class PBTreeModel : public QAbstractItemModel
	{

	public:
		PBTreeModel()
		{
			mRoot = new PBItemRoot();
		}
		~PBTreeModel()
		{
			SafeDelete(mRoot);
		}
		//////////////////////////////////////////////////////////////////////////
		virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override
		{
			if (!hasIndex(row, column, parent))
				return QModelIndex();

			PBItemBase *parentItem;

			if (!parent.isValid())
				parentItem = mRoot;
			else
				parentItem = static_cast<PBItemBase*>(parent.internalPointer());

			PBItemBase *childItem = parentItem->mChildren[row];
			if (childItem)
				return createIndex(row, column, childItem);
			else
				return QModelIndex();

			if (parent.isValid())
				return QModelIndex();
		}
		//////////////////////////////////////////////////////////////////////////
		virtual QModelIndex parent(const QModelIndex& index) const override
		{
			if (!index.isValid())
				return QModelIndex();

			PBItemBase *childItem = static_cast<PBItemBase*>(index.internalPointer());
			PBItemBase *parentItem = childItem->mParent;

			if (parentItem == mRoot)
				return QModelIndex();

			return createIndex(parentItem->GetRowIndex(), 0, parentItem);
		}
		//////////////////////////////////////////////////////////////////////////
		virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override
		{
			PBItemBase *parentItem;
			if (parent.column() > 0)
				return 0;

			if (!parent.isValid())
				parentItem = mRoot;
			else
				parentItem = static_cast<PBItemBase*>(parent.internalPointer());

			return parentItem->mChildren.Length();
		}
		virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override
		{
			return 2;
// 			if (parent.isValid())
// 				return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
// 			else
// 				return rootItem->columnCount();
		}
		virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override
		{
			int row = index.row();
			int col = index.column();
	
			if (col == 1) return QVariant();

			auto pItem = (PBItemBase*)index.internalPointer();
			auto pCategory = dynamic_cast<PBItemCategory*>(pItem);

			switch (role) {
			case Qt::DisplayRole:
				return pItem->GetText(col);
				break;
			case Qt::FontRole:
				if (pCategory)
				{
					QFont boldFont;
					boldFont.setBold(true);
					return boldFont;
				}
				break;
			
//			case Qt::BackgroundRole:
// 				if (row == 1 && col == 2)  //change background only for cell(1,2)
// 				{
// 					QBrush redBackground(Qt::red);
// 					return redBackground;
// 				}
// 				break;
// 			case Qt::TextAlignmentRole:
// 
// 				if (row == 1 && col == 1) //change text alignment only for cell(1,1)
// 				{
// 					return Qt::AlignRight + Qt::AlignVCenter;
// 				}
// 				break;
// 			case Qt::CheckStateRole:
// 
// 				if (row == 1 && col == 0) //add a checkbox to cell(1,0)
// 				{
// 					return Qt::Checked;
// 				}
			}
			return QVariant();
		}

		PBItemRoot* mRoot = nullptr;
	public:
		QModelIndex CreateIndex(int arow, int acolumn, void* adata)
		{
			return this->createIndex(arow, acolumn, adata);
		}

	};
	//////////////////////////////////////////////////////////////////////////
	class UEDITORCOMMON_API PropertyBrowserTree : public QTreeView
	{
	public:
		PropertyBrowserTree(QWidget* parent = nullptr) : QTreeView(parent)
		{
			//this->setHeaderHidden(true);
			this->setAlternatingRowColors(true);
			this->setEditTriggers(QTreeWidget::EditTrigger::NoEditTriggers);
			this->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
			this->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
		}
		void SetModel(PBTreeModel* pModel)
		{
			this->setModel(pModel);
		}
		PBTreeModel* GetModle() const 
		{
			return static_cast<PBTreeModel*>(this->model());
		}
		void SetItemWidget(PBItemBase* item, int column, QWidget* w);
	};
	//////////////////////////////////////////////////////////////////////////
	class UEDITORCOMMON_API PropertyBrowserWidget : public QWidget
	{
	public:
		PropertyBrowserWidget(QWidget* parent = nullptr);
		void Test();
		void AttachObject(Object* pObject);
		void AttachObjects(const TArray<Object*>& objects);
		void RebuidTree();

		
		FilterWidget* mFilter;
		PropertyBrowserTree* mTree;
		TObjectPtr<Object>	mAttachedObject;

	private:
		static PBItemProperty* CreatePropertyItem(PBItemBase* pParent, const PropertyInfo* pProperty);
	};


	class ClassCustomizer
	{
		void OnCustmize(PropertyBrowserWidget* propertyBrowser, QTreeWidgetItem* invisibleRoot)
		{
			QWidget* entCutomHeader = new QWidget;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	class UEDITORCOMMON_API PropertyBrowserDock : public QDockWidget
	{
	public:
		PropertyBrowserDock(QWidget* parent) : QDockWidget(parent)
		{
			this->setWindowTitle("Property Browser");
			this->setWidget(new PropertyBrowserWidget);
		}

		PropertyBrowserWidget* GetWidget() const { return (PropertyBrowserWidget*)this->widget(); }

		void AttachObject(Object* pObject) { GetWidget()->AttachObject(pObject); }
		void AttachObjects(const TArray<Object*>& objects) { return GetWidget()->AttachObjects(objects); }

	};
	
};