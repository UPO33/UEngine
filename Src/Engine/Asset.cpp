 #include "Asset.h"
#include "../Core/ByteSerializer.h"
#include "../Core/Threading.h"
#include "../Core/Timer.h"

namespace UEngine
{
		UCLASS_BEGIN_IMPL(AssetID)
		UCLASS_END_IMPL(AssetID)


		UCLASS_BEGIN_IMPL(Asset)
		UCLASS_END_IMPL(Asset)


			};

namespace UEngine
{
	UENGINE_API AssetMgr* gAssetMgr = nullptr;

	const uint32 AssetFileHeader::SIGNATURE = *((uint32*)"UEAF");


	const ClassInfo* AssetData::GetClass()
	{
		return GMetaSys()->FindClass(mClassName);
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
		if (mAssetData) return mAssetData->GetName();
		return Name();
	}

	AssetID Asset::GetID() const
	{
		if (mAssetData) return mAssetData->GetID();
		return AssetID();
	}

	bool Asset::IsAlone() const
	{
		return false;
	}



};