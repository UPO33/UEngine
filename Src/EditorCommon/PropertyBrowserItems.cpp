#include "PropertyBrowserItems.h"

namespace UEditor
{
	//////////////////////////////////////////////////////////////////////////
	void PBItemArray::CreateChildren()
	{
		UASSERT(mPropertyInfo->GetType() == EMetaType::EPT_TArray);

		MetaArrayView arrayView = GetArrayView();

		for (int iElement = 0; iElement < arrayView.Length(); iElement++) //for each element
		{
			auto pElement = arrayView.GetElementAt(iElement);

			PBItemProperty* prpItem = PBItemProperty::StaticCreateProperty(mPropertyInfo->GetArg0(), this);
			if (prpItem)
			{
				prpItem->mObject = this->mObject;
				prpItem->mArrayIndex = iElement;
				prpItem->mPropertyInfo = mPropertyInfo;
				prpItem->mInstance = this->mInstance;
				prpItem->CreateChildren();
			}
		}
	}

	void PBItemArray::RemoveAllElements()
	{
		this->InvokeOnPropertyChange(true);
		GetArrayView().RemoveAll();
		this->InvokeOnPropertyChange(false);
		this->GetPropertyBrowser()->RebuidTree();
	}

	void PBItemArray::AddToEnd()
	{
		int newElementIndex = static_cast<int>(GetArrayView().AddDefault(1));

		PBItemProperty* pNewPropertyItem = PBItemProperty::StaticCreateProperty(mPropertyInfo->GetArg0(), this);
		if (pNewPropertyItem)
		{
			pNewPropertyItem->mObject = this->mObject;
			pNewPropertyItem->mArrayIndex = newElementIndex;
			pNewPropertyItem->mPropertyInfo = mPropertyInfo;
			pNewPropertyItem->mInstance = this->mInstance;
			pNewPropertyItem->CreateChildren();
		}
	}

	MetaArrayView PBItemArray::GetArrayView() const
	{
		return MetaArrayView(mPropertyInfo->Map(mInstance), mPropertyInfo->GetArg0());
	}

};