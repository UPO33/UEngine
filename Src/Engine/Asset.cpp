 #include "Asset.h"
#include "../Core/ByteSerializer.h"
#include "../Core/Threading.h"
#include "../Core/Timer.h"

namespace UEngine
{
		UCLASS_BEGIN_IMPL(AssetID)
		UCLASS_END_IMPL(AssetID)


		UCLASS_BEGIN_IMPL(AssetItem)
		UCLASS_END_IMPL(AssetItem)

		UCLASS_BEGIN_IMPL(Asset)
		UCLASS_END_IMPL(Asset)


};

namespace UEngine
{
	const ClassInfo* AssetItem::GetClass()
	{
		return GMetaSys()->FindClass(mClassName);
	}

	UEngine::Asset* AssetItem::LoadNow()
	{
		//#TODO
		return nullptr;
	}

	AssetID AssetID::GenNew()
	{
		unsigned time = GetSecondsSince2000();
		unsigned tick = AppTickCount();

		return AssetID(time, tick, rand());
	}
	void AssetID::MetaSerialize(ByteSerializer& ser)
	{
		ser.Bytes(this, sizeof(*this));
	}
	void AssetID::MetaDeserialize(ByteDeserializer& ser)
	{
		ser.Bytes(this, sizeof(*this));
	}
	Name Asset::GetName() const
	{
		if (mItem) return mItem->GetName();
		return Name();
	}

	AssetID Asset::GetID() const
	{
		if (mItem) return mItem->GetID();
		return AssetID();
	}

	bool Asset::IsAlone() const
	{
		return false;
	}


	AssetMgr* AssetMgr::Get()
	{
		return nullptr;
	}
};