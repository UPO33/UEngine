#pragma once

#include "EditorCommonBase.h"
#include "AssetBrowser.h"

#include "../Engine/Asset.h"
#include "../Core/Log.h"


namespace UEditor
{
	//////////////////////////////////////////////////////////////////////////
	class AssetDataEd : public AssetData
	{
	public:
		AssetFileInfo* mInfo = nullptr;
	};

	//////////////////////////////////////////////////////////////////////////
	class AssetFileInfo
	{
	public:
		QString mAbsolutePath;	//	E:/Engine/Assets/Textures/MyTexture
		QString mFilename;		//	MyTexture
		QString mAssetName;		//	Textures/MyTexture
		bool mIsFolder = false;

		AssetDataEd* mAssetData = nullptr;
		TArray<AssetFileInfo*> mChildren;
		AssetFileInfo* mParent = nullptr;

		AssetFileInfo(AssetFileInfo* parent = nullptr)
		{
			if (parent)
			{
				mParent = parent;
				parent->mChildren.Add(this);
			}
		}
		
		bool IsFolder() const { return mIsFolder; }
		const ClassInfo* GetAssetClass() const 
		{
			return (mAssetData ? mAssetData->GetClass() : nullptr);
		}

		AssetFileInfo* HasChild(const QString& filename) const
		{
			for (AssetFileInfo* child : mChildren)
			{
				if (child && child->mFilename.compare(filename, Qt::CaseInsensitive) == 0)
					return child;
			}
			return nullptr;
		}
		bool Rename(const QString& newName);
		void PostRename();

		template<typename TLambda> void ForEachChild(TLambda proc)
		{
			for (auto child : mChildren)
				proc(child);
		}
	};

	//////////////////////////////////////////////////////////////////////////
	class UEDITORCOMMON_API AssetMgrEd : public AssetMgr
	{
	public:
		AssetFileInfo* mEngineRoot = nullptr;
		AssetFileInfo* mProjectRoot = nullptr;

		TArray<AssetData*>		mAssetsData;

		AssetMgrEd();
		void PrintDbg(AssetFileInfo* item, int indent);
		QString GetEngineAssetPath() const;
		QString GetProjectAssetPath() const;
		void CollectAssets();
		void CollectAssets(QString dir, AssetFileInfo* parent);

		virtual AssetData* FindAsset(Name assetName) override;
		virtual AssetData* FindAsset(AssetID assetID) override;
		virtual Asset* LoadAsset(AssetData* assetData) override;


		virtual Asset* CreateUnstableAsset(const ClassInfo* assetClass) override;

		//open and read the asset data, returns true if the specified file was asset 
		bool ReadAssetData(const QString& filepath, AssetID& outID, Name& ouAssetClassName);

				
		//@ folder		folder to create asset in
		//@ assetFilename	name of new asset file, if currently exist returns null
		//@pClone	if null, new asset will be default constructed
		//if succeeded returns a asset file info with valid mAssetData
		AssetFileInfo* CreateAsset(
			AssetFileInfo* folder,
			const QString& assetFilename,
			TSubClass<Asset> assetClass,
			const Asset* pClone = nullptr
		);

// 		Asset* DuplicateAsset(const AssetFileInfo* pAssetFile);
		//create a new folder return null if not possible or currently exist
		AssetFileInfo* CreateNewFolder(AssetFileInfo* folderToCreateFolderIn, const QString& folderName);
		bool RenameAsset(AssetFileInfo* assetOrFolderToRename, QString& newName);
		void SaveAsset(Asset* pAsset);
		void SaveAsset(AssetFileInfo* pAsset);
		Asset* LoadAsset(AssetFileInfo* assetFile);

	};

	
	inline AssetMgrEd* GAssetMgrEd() { return (AssetMgrEd*)GAssetMgr(); }
};