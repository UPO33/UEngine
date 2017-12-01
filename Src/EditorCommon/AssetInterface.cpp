#include "AssetInterface.h"

namespace UEditor
{
	UCLASS_BEGIN_IMPL(AssetInterface)
		UCLASS_END_IMPL(AssetInterface)

		UCLASS_BEGIN_IMPL(AssetActionsBase)
		UCLASS_END_IMPL(AssetActionsBase)
};

namespace UEditor
{

	UEDITORCOMMON_API void UGetUserCratableAssets(TArray<TSubClass<Asset>>& out)
	{
		for (auto pType : GMetaSys()->GetAllTypes())
		{
			auto pClass = Cast<ClassInfo>(pType);
			pClass->IsBaseOf<Asset>();
		}
	}
	
	template<typename T> inline T* UNewClassDefault(const ClassInfo* pClass = T::GetClassStatic())
	{
		void* ins = MemAllocAligned(pClass->GetSize(), pClass->GetAlign());
		if (pClass->HasDefaultConstructor())
			pClass->CallDefaultConstructor(ins);
		return (T*)ins;
	}
	template<typename T> void UDeleteClass(void* pInstance, const ClassInfo* pClass = T::GetClassStatic())
	{
		pClass->CallDestructor(pInstance);
	}


	//////////////////////////////////////////////////////////////////////////
	struct AssetInterfaceContext
	{
		const TArray<AssetInterface*>& GetInterfaces()
		{
			if (mInterfacesClass.Length() == 0)
				CollectInterfaces();

			return mInterfacesInstance;
		}

		void CollectInterfaces()
		{
			mInterfacesInstance.RemoveAll();
			mInterfacesClass.RemoveAll();

			for (auto pType : GMetaSys()->GetAllTypes())
			{
				const ClassInfo* pClass = Cast<ClassInfo>(pType);
				if (pClass && pClass != AssetInterface::GetClassStatic() && pClass->IsBaseOf<AssetInterface>())
				{
					size_t index = mInterfacesClass.Find(pClass);
					if (index == INVALID_INDEX)
					{
						mInterfacesClass.Add(pClass);
						mInterfacesInstance.Add(UNewClassDefault<AssetInterface>(pClass));
					}
				}
			}
		}
		void RealseInterfaces()
		{
			mInterfacesInstance.RemoveAll();
			mInterfacesClass.RemoveAll();
		}

		TArray<AssetInterface*> mInterfacesInstance;
		TArray<TSubClass<AssetInterface>> mInterfacesClass;
		AssetInterface* mDefault;

		AssetInterfaceContext()
		{
			mDefault = new AssetInterface;
			CollectInterfaces();
		}
		~AssetInterfaceContext()
		{
			RealseInterfaces();
		}
		
		static AssetInterfaceContext* Get()
		{
			static AssetInterfaceContext Ins;
			return &Ins;
		}


	};

	const TArray<AssetInterface*>& AssetInterface::GetInterfaces()
	{
		 return AssetInterfaceContext::Get()->GetInterfaces();
	}

	void AssetInterface::CollectInterfaces()
	{
		AssetInterfaceContext::Get()->CollectInterfaces();
	}

	void AssetInterface::ReleaseInterfaces()
	{
		AssetInterfaceContext::Get()->RealseInterfaces();
	}

	AssetInterface* AssetInterface::GetInterface(TSubClass<Asset> assetClass)
	{
		if (!assetClass) return nullptr;

		const TArray<AssetInterface*>& interfaces = AssetInterfaceContext::Get()->GetInterfaces();
		
		for (AssetInterface* iter : interfaces)
		{
			if (iter->mClass == assetClass)
				return iter;
		}
		for (AssetInterface* iter : interfaces)
		{
			if (assetClass->IsBaseOf(iter->mClass))
				return iter;
		}

		return AssetInterfaceContext::Get()->mDefault;
	}

};