#include "PropertyBrowser.h"
#include "PropertyBrowserItems.h"

namespace UEditor
{



	PropertyBrowserWidget::PropertyBrowserWidget(QWidget* parent /*= nullptr*/) : QWidget(parent)
	{
		this->setLayout(new QVBoxLayout);
		mFilter = new FilterWidget(this);
		this->layout()->addWidget(mFilter);
		mTree = new PropertyBrowserTree;
		this->layout()->addWidget(mTree);

		Test();
	}

	void PropertyBrowserWidget::Test()
	{
// 		auto i0 = new QTreeWidgetItem();
// 		auto i1 = new QTreeWidgetItem();
// 		auto i2 = new QTreeWidgetItem();
// 
// 		QColor catBackColor = QColor(10, 10, 10);
// 
// 		i0->setText(0, "Entity");
// 		i1->setText(0, "Physics");
// 		i2->setText(0, "Rendering");
// 
// 		i0->setBackgroundColor(0, catBackColor);
// 		i0->setBackgroundColor(1, catBackColor);
// 
// 		mTree->addTopLevelItem(i0);
// 		mTree->addTopLevelItem(i1);
// 		mTree->addTopLevelItem(i2);
	}

	void PropertyBrowserWidget::AttachObject(Object* pObject)
	{
		mAttachedObject = pObject;
		RebuidTree();
	}

	void PropertyBrowserWidget::AttachObjects(const TArray<Object*>& objects)
	{
		ULOG_WARN("mutiple attachment is not supported yet. uses the first object");
		AttachObject(objects.Length() ? objects[0] : nullptr);
	}

	QString UGetClassNameFreindly(Name className)
	{
		QString qstr = UToQString(className);
		int indexQQ = qstr.lastIndexOf("::");
		if (indexQQ == -1) return qstr;
		return qstr.mid(indexQQ + 2);
	}
	//////////////////////////////////////////////////////////////////////////
	void PropertyBrowserWidget::RebuidTree()
	{
		PBTreeModel* newModle = new PBTreeModel;
		PBItemRoot* invisibleRootItem = newModle->mRoot;

		invisibleRootItem->mPropertyBrowser = this;
		invisibleRootItem->mObject = mAttachedObject;
		invisibleRootItem->CreateChildren();

		{
			auto oldModle = mTree->GetModle();
			SafeDelete(oldModle);
			mTree->SetModel(newModle);

			//assigning  widgets
			invisibleRootItem->ForEachChild(true, [this](PBItemBase* pItem) {
				this->mTree->SetItemWidget(pItem, 1, pItem->CreateWidget(1));
			});

			mTree->expandAll();
		}
	}






	PBItemProperty* PropertyBrowserWidget::CreatePropertyItem(PBItemBase* pParent, const PropertyInfo* pProperty)
	{
		EMetaType prpType = pProperty->GetType();
		switch (prpType)
		{
		case EMetaType::EPT_bool:
			return new PBItemBool(pParent);
			break;

		case EMetaType::EPT_float:
		case EMetaType::EPT_int32:
			return new PBItemNumberic(pParent);
		
		case EMetaType::EPT_TSubClass:
		{
			return new PBItemSubClass(pParent);
		}
		case  EMetaType::EPT_TObjectPtr:
		case EMetaType::EPT_ObjectPoniter:
		{
			return new PBItemObjectPtr(pParent);
		}
		case  EMetaType::EPT_TArray:
		{
			return new PBItemArray(pParent);
		}
		case EMetaType::EPT_Class:
		{
			if (auto pClass = pProperty->GetTypePtr())
			{
				if (pClass == Name::GetClassStatic())
					return new PBItemString(pParent);
				
				if (pClass == Vec2::GetClassStatic())
					return new PBItemVecN(pParent);
				if (pClass == Vec3::GetClassStatic())
					return new PBItemVecN(pParent);
				if (pClass == Vec4::GetClassStatic())
					return new PBItemVecN(pParent);

				if (pClass == Color::GetClassStatic())
					return new PBItemColor(pParent);
				if (pClass == Color32::GetClassStatic())
					return new PBItemColor(pParent);

				//#TODO must get property item for the class somehow if not exist proceed the default way

				
			}
		}
		}

		return new PBItemProperty(pParent);
	}



	void PropertyBrowserTree::SetItemWidget(PBItemBase* item, int column, QWidget* w)
	{
		this->setIndexWidget(this->GetModle()->CreateIndex(item->GetRowIndex(), column, item), w);
	}

	//////////////////////////////////////////////////////////////////////////
	void* PBItemProperty::GetValuePtr() const
	{
		if (IsArrayElement())
		{
			MetaArrayView arrayView(mPropertyInfo->Map(mInstance), mPropertyInfo->GetArg0());
			return arrayView.GetElementAt(mArrayIndex);
		}
		else
		{
			return mPropertyInfo->Map(this->mInstance);
		}
		return nullptr;
	}

	//////////////////////////////////////////////////////////////////////////
	PBItemProperty* PBItemProperty::StaticCreateProperty(const TriTypeData& type, PBItemBase* pParent)
	{
		EMetaType prpType = type.GetType();
		switch (prpType)
		{
		case EMetaType::EPT_bool:
			return new PBItemBool(pParent);
			break;

		case EMetaType::EPT_float:
		case EMetaType::EPT_int32:
			return new PBItemNumberic(pParent);

		case EMetaType::EPT_TSubClass:
		{
			return new PBItemSubClass(pParent);
		}
		case  EMetaType::EPT_TObjectPtr:
		case EMetaType::EPT_ObjectPoniter:
		{
			return new PBItemObjectPtr(pParent);
		}
		case  EMetaType::EPT_TArray:
		{
			return new PBItemArray(pParent);
		}
		case EMetaType::EPT_Class:
		{
			if (auto pClass = dynamic_cast<const ClassInfo*>(type.GetPtr()))
			{
				if (pClass == Name::GetClassStatic())
					return new PBItemString(pParent);

				if (pClass == Vec2::GetClassStatic())
					return new PBItemVecN(pParent);
				if (pClass == Vec3::GetClassStatic())
					return new PBItemVecN(pParent);
				if (pClass == Vec4::GetClassStatic())
					return new PBItemVecN(pParent);

				if (pClass == Color::GetClassStatic())
					return new PBItemColor(pParent);
				if (pClass == Color32::GetClassStatic())
					return new PBItemColor(pParent);

				//#TODO must get property item for the class somehow if not exist proceed the default way


			}
		}
		}

		return new PBItemProperty(pParent);
	}

	PBItemProperty* PBItemProperty::StaticCreateProperty(const PropertyInfo* pProperty, PBItemBase* pParent)
	{
		return PBItemProperty::StaticCreateProperty(pProperty->GetSelf(), pParent);
	}

	//////////////////////////////////////////////////////////////////////////
	void PBItemRoot::CreateChildren()
	{
		if (mObject == nullptr)return;

		for (const ClassInfo* pSingleClass : mObject->GetClass()->GetClassChain())
		{
			for (const PropertyInfo* pProperty : pSingleClass->GetProperties())
			{
				//the category we are gonna add properties to
				auto targetCategory = FindOrAddCategory(UToQString(pSingleClass->GetName()));
				if (auto attrCategory = pProperty->GetAttribute<AttrCategory>())
				{
					targetCategory = FindOrAddCategory(attrCategory->mCategory);
				}

				PBItemProperty* pPropertyItem = PBItemProperty::StaticCreateProperty(pProperty, targetCategory);

				if (pPropertyItem)
				{
					pPropertyItem->mPropertyInfo = pProperty;
					pPropertyItem->mObject = mObject;
					pPropertyItem->mInstance = mObject;
					pPropertyItem->CreateChildren();
				}
			}
		}
	}

	PropertyBrowserWidget* PBItemRoot::GetPropertyBrowser() const
	{
		UASSERT(mPropertyBrowser);
		return mPropertyBrowser;
	}

	//////////////////////////////////////////////////////////////////////////
	void PBItemStruct::CreateChildren()
	{
		if (!GetStructClass() || !GetStructPtr())return;

		for (const ClassInfo* pClass : GetStructClass()->GetClassChain())
		{
			for (const PropertyInfo* pProperty : pClass->GetProperties())
			{
				PBItemProperty* pPropertyItem = PBItemProperty::StaticCreateProperty(pProperty, this);

				if (pPropertyItem)
				{
					pPropertyItem->mObject = mObject;
					pPropertyItem->mPropertyInfo = pProperty;
					pPropertyItem->mInstance = this->GetStructPtr();
					pPropertyItem->CreateChildren();
				}
			}
		}
	}

	PropertyBrowserWidget* PBItemBase::GetPropertyBrowser() const
	{
		if (mParent)
			return mParent->GetPropertyBrowser();

		return nullptr;
	}

};