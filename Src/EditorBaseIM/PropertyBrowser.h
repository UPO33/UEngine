#include "EditorBaseIM.h"
#include "../Core/Object.h"
#include "../Core/Meta.h"
#include "../Core/ClassUtility.h"
#include "../Core/MetaArray.h"

#include <map>

namespace UEditor
{

	struct PBPrpFloat
	{
		float	mValue = 0;
		bool	mValueChanged = false;

// 		void Draw()
// 		{
// 			const char* displayFormat = "0.5f";
// 			if (!HasSameValue())
// 			{
// 				displayFormat = "...";
// 				mValue = 0;
// 			}
// 			
// 			mValueChanged = ImGui::DragFloat("", &mValue, 1, 0, 9999, displayFormat);
// 		}
	};

	
	//////////////////////////////////////////////////////////////////////////
	//base class for an item in property tree,
	//an item may refer to a property of an Object or (multiple objects in multi-selection)
	// 
	//example single object tree:
	//PBItemRoot
	//--PBItemCategory
	//----PBItemFloat
	//----PBItemFloat
	//--PBItemCategory
	//----PBItemNode
	//---- PBItemFloat
	//---- PBItemBool
	//////////////////////////////////////////////////////////////////////////
	struct PBItemBase
	{
		PBItemBase*				mParent = nullptr;
		TArray<PBItemBase*>		mChildren;

		PBItemBase( PBItemBase* parent = nullptr)
		{
			if (parent)
			{
				mParent = parent;
				mParent->mChildren.Add(this);
			}
		}
		~PBItemBase()
		{
			for (PBItemBase* child : mChildren)
				delete child;
			
			mChildren.RemoveAll();
		}
		virtual const TArray<void*>* GetIntances() const
		{
			if (mParent) return mParent->GetIntances();
			return nullptr;
		}
		
		virtual const char* GetLable() { return ""; }
		virtual void Draw()
		{
			//in in derived class should draw his items first
			DrawName();
			ImGui::NextColumn();
			DrawValue();
			ImGui::NextColumn();

			for (PBItemBase* pChild : mChildren)
			{
				ImGui::PushID(pChild);
				pChild->Draw();
				ImGui::PopID();
			}

		}
		virtual void DrawName() {}
		virtual void DrawValue() {}
	};
	//////////////////////////////////////////////////////////////////////////
	struct PBItemProperty : PBItemBase
	{
		const PropertyInfo* mProperty;
		//if its a root property the instance is object
		TArray<void*> mInstances;
		size_t mArrayInedx;

		size_t NumInstances() const { return mInstances.Length(); }

		virtual void ResetToDefault()
		{

		}
		virtual bool CheckSimilarity(const void* pValue0, const void* pValue1) const
		{
			return false;
		}
		//returns pointer to value of property at specified instance index
		void* GetValuePtrAt(unsigned instanceIndex) const
		{
			UASSERT(mInstances.IsIndexValid(instanceIndex));

			if (mArrayInedx == INVALID_INDEX)
			{
				void* pSingleValue = mProperty->Map(mInstances[instanceIndex]);
				return pSingleValue;
			}
			else
			{
				void* pTArray = mProperty->Map(mInstances[instanceIndex]);
				MetaArrayView arrayView(pTArray, mProperty->GetArg0());
				void* pSingleValue = arrayView.GetElementAt(mArrayInedx);
				return pSingleValue;
			}
		}
		bool HasSameValue() const
		{
			if (mInstances.Length() == 0) return false;
			if (mInstances.Length() == 1) return true;


			for (size_t i = 0; i < (mInstances.Length() - 1); i++)
			{
				if (!CheckSimilarity(GetValuePtrAt(i), GetValuePtrAt(i + 1)))
					return false;
			}
			return true;
		}
		virtual void SetValue(const void* pNewValue)
		{
			for (size_t i = 0; i < mInstances.Length(); i++)
			{
				GetValuePtrAt(i);
			}
		}
	};
	//////////////////////////////////////////////////////////////////////////
	struct PBItemRoot : PBItemBase
	{
		
		PBItemRoot(PBItemBase* parent = nullptr) : PBItemBase(parent)
		{

		}
	};
	//////////////////////////////////////////////////////////////////////////
	struct PBItemArray : PBItemBase
	{
		
		PBItemArray(PBItemBase* parent) : PBItemBase(parent)
		{

		}
		virtual void Draw() override
		{

			ImGui::Text("Array");
			ImGui::SameLine();
			ImGui::Button("C");
			ImGui::NextColumn();

			PBItemBase::Draw();
		}
	};
	//////////////////////////////////////////////////////////////////////////
	struct PBItemCategory : PBItemBase
	{
		Name mCategoryName;

		PBItemCategory(Name categoryName, PBItemBase* parent) : PBItemBase(parent), mCategoryName(categoryName)
		{

		}
		virtual void Draw() override
		{
			if (ImGui::CollapsingHeader(mCategoryName))
			{
				ImGui::Columns(2);
				PBItemBase::Draw(); //drawing children
				ImGui::Columns(1);
			}
		}
	};

	//////////////////////////////////////////////////////////////////////////
	struct PBItemStruct : PBItemProperty
	{
		const PropertyInfo* mProperty = nullptr;
		void* mInstance = nullptr;
		size_t mArrayIndex;



		virtual const TArray<void*>* GetIntances() const
		{
			return &mInstances;
		}
		virtual void Draw() override
		{
			//#TODO must get user friendly name
			if (ImGui::TreeNode(mProperty->GetName()))
			{
				//if (ImGui::IsItemHovered())
				//	ImGui::SetTooltip(mProperty->GetComment());

				PBItemBase::Draw();
				ImGui::TreePop();
			}

			ImGui::NextColumn();
		}
	};
	//////////////////////////////////////////////////////////////////////////
	struct PBItemFloat : PBItemProperty
	{
		float	mValue = 0;
		bool	mValueChanged = false;

		const PropertyInfo * mProperty = nullptr;
		void* mInstance = nullptr;
		size_t mArrayIndex = INVALID_INDEX;



		using ParentT = PBItemBase;



		
		virtual bool CheckSimilarity(const void* pValue0, const void* pValue1) const override
		{
			UASSERT(pValue0 && pValue1);
			return *((float*)pValue0) == *((float*)pValue1);
		}
		float GetFirstValue() const
		{
			UASSERT(GetValuePtrAt(0));

			return  *((float*)GetValuePtrAt(0));
		}
		virtual void Draw() override
		{
			//drawing name
			ImGui::Text(mProperty->GetName());
			ImGui::NextColumn();


			//const char* format = "%.4f";

			if (mArrayIndex != INVALID_INDEX)
			{
				ImGui::Text("[%d]", mArrayIndex); ImGui::SameLine();
				ImGui::Button("D"); ImGui::SameLine();
			}

			const char* displayFormat = "0.5f";
			if (!HasSameValue())
			{
				displayFormat = "...";
				mValue = 0;
			}
			mValueChanged = ImGui::DragFloat("", &mValue, 1, FLT_MIN, FLT_MAX, displayFormat);

			ImGui::NextColumn();

			ParentT::Draw();	
		}
		void SetValue(float value)
		{
			
		}
	};
	
	//////////////////////////////////////////////////////////////////////////
	struct PBIemBool : PBItemBase
	{
		bool mValue;
		PBIemBool(PBItemBase* parent) : PBItemBase(parent)
		{

		}
		virtual void Draw() override
		{
			ImGui::Checkbox(GetLable(), &mValue);

			PBItemBase::Draw();
		}
	};
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	struct PBText : PBItemBase
	{
		char mTextBuffer[128];

		PBText(PBItemBase* parent) : PBItemBase(parent)
		{

		}
		virtual void Draw() override
		{
			ImGui::PushID(this);
			ImGui::InputText(GetLable(), mTextBuffer, sizeof(mTextBuffer));
			ImGui::PopID();

			PBItemBase::Draw();
		}
	};
	//////////////////////////////////////////////////////////////////////////
	struct DPropertyBrowser
	{
		DFilterBox			mFilter;
		PBItemRoot			mRoot;

		Object*				mAttachedObject = nullptr;

		void AttachObject(Object* pObject)
		{
			mRoot.~PBItemRoot();

			mAttachedObject = pObject;

			ExtractObjectProperties(pObject);
			ULOG_MESSAGE("");
		}

		

		PBItemCategory* GetCategory(Name categoryName)
		{
			for (PBItemBase* base : mRoot.mChildren)
				if (PBItemCategory* category = dynamic_cast<PBItemCategory*>(base))
					if (category->mCategoryName == categoryName)
						return category;

			return new PBItemCategory(categoryName, &mRoot);
		}
#if 0
		bool CategoryExist(Name name) const
		{
			for (const PBCategory& category : mCategories)
				if (category.mCategoryName == name) return true;
			return false;
		}
		PBCategory& GetCategory(Name categoryName)
		{
			for (PBCategory& category : mCategories)
				if (category.mCategoryName == categoryName)
					return category;

			mCategories.Add();
			mCategories.LastElement().mCategoryName = categoryName;
			return mCategories.LastElement();
		}
#endif // 
		void ExtractObjectProperties(Object* pObject)
		{
			if (pObject == nullptr) return;

			ExtractClassProperties(nullptr, pObject->GetClass(), pObject, true);

// 			for (const ClassInfo* pClass : pObject->GetClass()->GetClassChain())
// 			{
// 				for (const PropertyInfo* pProperty : pClass->GetProperties())
// 				{
// 					if (pProperty->GetAttribute<AttrHidden>()) continue;
// 
// 					PBCategory& tagetCategory = GetCategory(pClass->GetName());
// 
// 					if (const AttrCategory* pAttrCategory = pProperty->GetAttribute<AttrCategory>())
// 					{
// 						tagetCategory = GetCategory(pAttrCategory->mCategory);
// 					}
// 
// 					PBItemBase* parentItem = tagetCategory.mRootItem;
// 
// 					if (pProperty->GetType() == EMetaType::EPT_float)
// 					{
// 						new PBItemFloat(parentItem);
// 					}
// 					if (pProperty->GetType() == EMetaType::EPT_bool)
// 					{
// 						new PBIemBool(parentItem);
// 					}
// 				}
// 			}
		}
		void ExtractClassProperties(PBItemBase* parentItem, const ClassInfo* pClass, void* pInstance, bool bIsObject)
		{
			for (const ClassInfo* pClassIter : pClass->GetClassChain())
			{
				for (const PropertyInfo* pProperty : pClassIter->GetProperties())
				{
					if (pProperty->GetAttribute<AttrHidden>()) continue;

					PBItemBase* parent = parentItem;

					if (parentItem == nullptr) //is root property in object?
					{
						parent = GetCategory(pClassIter->GetName());

						if (const AttrCategory* pAttrCategory = pProperty->GetAttribute<AttrCategory>())
						{
							parent = GetCategory(pAttrCategory->mCategory);
						}
					}

					EMetaType type = pProperty->GetType();

				}
			}
		}

		void ExtractClassProperties2(PBItemBase* parentItem, const ClassInfo* pClass, void* pInstance, bool bIsObject)
		{
			for (const ClassInfo* pClassIter : pClass->GetClassChain())
			{
				for (const PropertyInfo* pProperty : pClassIter->GetProperties())
				{
					if (pProperty->GetAttribute<AttrHidden>()) continue;

					PBItemBase* parent = parentItem;

					if (parentItem == nullptr) //is root property in object?
					{
						parent = GetCategory(pClassIter->GetName());

						if (const AttrCategory* pAttrCategory = pProperty->GetAttribute<AttrCategory>())
						{
							parent = GetCategory(pAttrCategory->mCategory);
						}
					}

					EMetaType type = pProperty->GetType();

					
				}
			}
		}

		void Draw()
		{
			ImGui::Separator();
			ImGui::Columns(2);
			ImGui::Text("aaaaaa");
			ImGui::Text("bbbbbb");
			ImGui::Text("cccccccc");
			ImGui::NextColumn();
			ImGui::Text("bbbbbbbbb");
			ImGui::NextColumn();
			ImGui::AlignFirstTextHeightToWidgets();
			ImGui::Button("A");
			ImGui::NextColumn();
			ImGui::AlignFirstTextHeightToWidgets();
			ImGui::Button("B");
			ImGui::Button("C");
			ImGui::Columns(1);
			ImGui::Separator();


			mRoot.Draw();
		}
	};

	//property browser must be able to attach multiple objects, for instance a Entity and EntityCamera is selected in editor
	//property browser should expose shared properties between Entity and EntityCamera as a single property that changes multiple values in fly
	
	//1 extracting shared properties from objects
	//2 categorizing properties
	//3 crating property tree items
	//4 drawing items
	//5 handling property edit chain


	class WPropertyBrowser : public WBase
	{
	public:
		DPropertyBrowser mPropertyBrowser;

		WPropertyBrowser() : WBase(true, "PropertyBrowser")
		{
			
		}
		//////////////////////////////////////////////////////////////////////////
		void AttachObject(Object* pObject)
		{
			mPropertyBrowser.AttachObject(pObject);
		}
		virtual void DrawContents() override
		{
			mPropertyBrowser.Draw();
		}
	};
};