#pragma once

#include "Base.h"
#include "../Core/Object.h"
#include "../Core/Name.h"
#include "../Core/Meta.h"
#include "../Core/ObjectPtr.h"
#include "../Core/Delegate.h"

namespace UEngine
{
	//////////////////////////////////////////////////////////////////////////
	class Asset;
	class AssetID;
	class AssetItem;
	class AssetExt;
	class AssetMgr;

	//assets are linked and loaded by ID so that if we rename or move a asset file there wont be any problem
	class UENGINEBASE_API AssetID
	{
		UCLASS(AssetID)

		//null initialize
		AssetID() : mID0(0), mID1(0), mID2(0) {}
		AssetID(uint32 id0, uint32 id1, uint32 id2) : mID0(id0), mID1(id1), mID2(id2) {}
		
		bool IsNull() const { return mID0 == 0 && mID1 == 0 && mID2 == 0; }

		bool operator == (const AssetID& other) const { return mID0 == other.mID0 && mID1 == other.mID1 && mID2 == other.mID2; }
		bool operator != (const AssetID& other) const { return !this->operator==(other); }

		static AssetID GenNew();

		void MetaSerialize(ByteSerializer&);
		void MetaDeserialize(ByteDeserializer&);

	private:
		uint32	mID0;
		uint32	mID1;
		uint32	mID2;
	};



	//actually an asset item contains the data that are required whether asset is not loaded to memory yet
	class UENGINEBASE_API AssetItem : public Object
	{
		UCLASS(AssetItem)

		friend AssetMgr;
		friend Asset;

		AssetID						GetID() const { return mID; }
		//return the name of asset, name are relative to assets folders e.g "Engine/DefaulTexture"
		Name						GetName() const { return mName; }
		Name						GetClassName() const { return mClassName; }
		//return the class of asset, null if the class is unknown
		const ClassInfo*			GetClass();
		//return the instance, null if not loaded
		Asset*						GetInstance() const { return mInstance; }
		//load the asset to memory and block the thread
		Asset*						LoadNow();

	private:
		AssetID					mID;
		Name					mName;
		Name					mClassName;
		Asset*					mInstance;
	};

	//using extension u would be able to add features to a asset class without changing source or inheriting from the asset class
	class UENGINEBASE_API AssetExt : public Object
	{
		UCLASS(AssetExt, Object)

		virtual void OnCreate() {}
		virtual void OnDestroy() {}

		template<class TAsset = Asset> TAsset* GetOwner() const
		{
			UASSERT(mOwner);
			return mOwner->Cast<TAsset>();
		}

	private:
		Asset*		mOwner;
	};

	typedef TObjectPtr<AssetExt>	AssetExtPtr;


	enum EAssetFlag
	{
		
		EAF_Dirty
	};

	//the base class for assets, anything like 'Texture, Mesh, Sound' that can be loaded is a asset
	class UENGINEBASE_API Asset : public Object
	{
		UCLASS(Asset, Object)
	public:

		AssetItem*			GetAssetItem() const { return mItem; }
		Name				GetName() const;
		AssetID				GetID() const;

		//if no one is using this asset this asset will be destroyed later on
		bool		IsAlone() const;


		virtual void OnCreate(){}
		virtual void OnDestroy() {}
		virtual void OnGameTick(float delta) {}
		virtual void OnRenderTick(float delta) {}
		virtual void OnBeforeSave(){}
		virtual void OnAfterSave(){}

		template<class TAssetExt> TAssetExt* GetExt() const
		{
			return nullptr;
		}


	private:
		AssetItem*				mItem = nullptr;
		TArray<AssetExt*>		mExtentions;
		TArray<Name>			mTags;
		String					mAssetImportFile;	//the full path of the file that asset was loaded from

	protected:
		//if false, this asset will be checked for release if there is no TObjectPtr pointing to it
		bool					mManualRelease = false;
		//the duration in seconds at which OnGameTick is called, -1 == never tick, 0 == every frame. must be initialized once in the constructor
		float					mGameTickInterval = -1;
		//the duration in seconds at which OnRenderTick is called, -1 == never tick, 0 == every frame. must be initialized once in the constructor
		float					mRenderTickInterval = -1;
	};

	typedef TObjectPtr<Asset> AssetPtr;


	//////////////////////////////////////////////////////////////////////////
	class UENGINEBASE_API AssetMgr
	{
	public:
		static AssetMgr* Get();

		virtual AssetItem* FindAsset(Name assetName) const = 0;
		virtual AssetItem* FindAsset(AssetID assetID) const = 0;
		
		virtual Asset* CreateUnstableAsset(const ClassInfo* assetClass) = 0;
	};

	inline AssetMgr* GAssetMgr() { return AssetMgr::Get(); }
};