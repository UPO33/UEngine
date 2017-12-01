#include "AssetMgrEd.h"
#include "../Core/ByteSerializer.h"
#include "../Engine/ObjectArchive.h"

namespace UEditor
{

	bool UAssetWriteHeader(ByteSerializer& ser, AssetID id, Name assetClassName)
	{
		ser << AssetFileHeader::SIGNATURE;
		id.MetaSerialize(ser);
		assetClassName.MetaSerialize(ser);
		return true;
	}
	bool UAssetReadHeader(ByteDeserializer& dser, AssetID& outID, Name& outAssetClassName)
	{
		uint32 sinature = 0;
		dser >> sinature;
		if (sinature == AssetFileHeader::SIGNATURE)
		{
			outID.MetaDeserialize(dser);
			outAssetClassName.MetaDeserialize(dser);
			return !dser.HasError();
		}
		return false;
	}


	AssetMgrEd::AssetMgrEd()
	{
		CollectAssets();
		PrintDbg(mEngineRoot, 0);
		PrintDbg(mProjectRoot, 0);
		
	}

	void AssetMgrEd::PrintDbg(AssetFileInfo* item, int indent)
	{
		ULOG_WARN("% FN %", LogIndent(indent), item->mFilename);
		ULOG_WARN("% AP %", LogIndent(indent), item->mAbsolutePath);
		
		for (auto child : item->mChildren)
		{
			PrintDbg(child, indent + 1);
		}
	}

	QString AssetMgrEd::GetEngineAssetPath() const
	{
		return QDir("../../Assets/").absolutePath();
	}

	QString AssetMgrEd::GetProjectAssetPath() const
	{
		return QDir("../../TestProject/Assets/").absolutePath();
	}

	void AssetMgrEd::CollectAssets()
	{
		SafeDelete(mEngineRoot);
		SafeDelete(mProjectRoot);

		mProjectRoot = new AssetFileInfo();
		mProjectRoot->mIsFolder = true;
		mProjectRoot->mAbsolutePath = GetProjectAssetPath();

		mEngineRoot = new AssetFileInfo();
		mEngineRoot->mIsFolder = true;
		mEngineRoot->mAbsolutePath = GetEngineAssetPath();

		CollectAssets(GetEngineAssetPath(), mEngineRoot);
		CollectAssets(GetProjectAssetPath(), mProjectRoot);
	}
	//////////////////////////////////////////////////////////////////////////
	void AssetMgrEd::CollectAssets(QString dir, AssetFileInfo* parent)
	{
		QDirIterator iter(dir, QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);

		while (iter.hasNext())
		{
			//eg: ../content/file.txt
			auto filepath = iter.next();

			QFileInfo fileInfo = iter.fileInfo();

			if (fileInfo.isFile())
			{
				AssetID assetID;
				Name assetClassName;

				if(this->ReadAssetData(fileInfo.absoluteFilePath(), assetID, assetClassName)) //is asset file ?
				{
					auto assetInfo = new AssetFileInfo(parent);
					assetInfo->mAbsolutePath = fileInfo.absoluteFilePath();

					if (parent->mAssetName.isEmpty())
						assetInfo->mAssetName = fileInfo.fileName();
					else
						assetInfo->mAssetName = parent->mAssetName + '/' + fileInfo.fileName();
					
					assetInfo->mFilename = fileInfo.fileName();
					assetInfo->mIsFolder = false;

					auto assetData = new AssetDataEd;
					assetData->mInfo = assetInfo;
					assetData->mID = assetID;
					assetData->mClassName = assetClassName;
					assetData->mName = UQString2Name(assetInfo->mAssetName);

					assetInfo->mAssetData = assetData;
				}
			}
			else
			{
				auto item = new AssetFileInfo(parent);
				item->mAbsolutePath = fileInfo.absoluteFilePath();
				item->mFilename = fileInfo.fileName();
				if (parent->mAssetName.isEmpty())
					item->mAssetName = fileInfo.fileName();
				else
					item->mAssetName = (parent->mAssetName + '/' + fileInfo.fileName());
				item->mIsFolder = true;

				CollectAssets(fileInfo.absoluteFilePath(), item);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	AssetData* AssetMgrEd::FindAsset(Name assetName)
	{
		for (AssetData* pIter : mAssetsData)
		{
			if (pIter && pIter->mName.GetHashLowerCase() == assetName.GetHashLowerCase())
				return pIter;
		}
		return nullptr;
	}

	//////////////////////////////////////////////////////////////////////////
	AssetData* AssetMgrEd::FindAsset(AssetID assetID)
	{
		for (AssetData* pIter : mAssetsData)
		{
			if (pIter && pIter->GetID() == assetID)
				return pIter;
		}
		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	Asset* AssetMgrEd::LoadAsset(AssetData* pAssetData)
	{
		if (!pAssetData) return nullptr;

		if (pAssetData->mInstance) //is already loaded
			return pAssetData->mInstance;

		UASSERT(dynamic_cast<AssetDataEd*>(pAssetData));

		return LoadAsset(((AssetDataEd*)pAssetData)->mInfo);
	}

	//////////////////////////////////////////////////////////////////////////
	Asset* AssetMgrEd::LoadAsset(AssetFileInfo* pAssetFile)
	{
		if (!pAssetFile) return nullptr;
		if (pAssetFile->IsFolder()) return nullptr;
		
		UASSERT(pAssetFile->mAssetData);

		if (pAssetFile->mAssetData->GetInstance()) //is already loaded?
			return pAssetFile->mAssetData->GetInstance();

		QFile file(pAssetFile->mAbsolutePath);

		if (file.open(QFile::OpenModeFlag::ReadOnly))
		{
			QByteArray fileContent = file.readAll();

			ByteDeserializer dserHead(fileContent.data(), fileContent.size());

			AssetID assetID;
			Name assetClassName;

			if (!UAssetReadHeader(dserHead, assetID, assetClassName))
			{
				ULOG_ERROR("failed to read asset header");
				return nullptr;
			}

			if (Asset* loadedAset = UCast<Asset>(ULoadArchive(dserHead)))
			{
				UASSERT(assetID == pAssetFile->mAssetData->GetID());
				UASSERT(assetClassName == pAssetFile->mAssetData->GetClassName());

				loadedAset->mAssetData = pAssetFile->mAssetData;
				pAssetFile->mAssetData->mInstance = loadedAset;
				
				ULOG_SUCCESS("asset [%] loaded", pAssetFile->mAssetName);
				return loadedAset;
			}
			else
			{
				ULOG_ERROR("failed to lead asset [%]", pAssetFile->mAssetName);
				return nullptr;
			}

		}
		return nullptr;
	}

	Asset* AssetMgrEd::CreateUnstableAsset(const ClassInfo* assetClass)
	{
		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AssetMgrEd::ReadAssetData(const QString& filepath, AssetID& outID, Name& ouAssetClassName)
	{
		QFile file(filepath);
		if (file.open(QFile::OpenModeFlag::ReadOnly))
		{
			uint32 sign = 0;
			file.peek(((char*)&sign), sizeof(AssetFileHeader::SIGNATURE));
			
			if (sign != AssetFileHeader::SIGNATURE) return false;

			QByteArray bytes = file.read(AssetFileHeader::MAX_POSSIBLE_SIZE);
			ByteDeserializer dser(bytes.data(), bytes.size());
			
			return  UAssetReadHeader(dser, outID, ouAssetClassName);
		}
		return false;
	}




#if 0
	AssetData* AssetMgrEd::CreateAssetData(AssetFileInfo* folder, const QString& assetFilename, const ClassInfo* pClass)
	{
		if (assetFilename.isEmpty()) return nullptr;

		UASSERT(folder && folder->IsFolder());
		UASSERT(pClass && pClass->IsBaseOf<Asset>());

		if (folder->HasChild(assetFilename)) return nullptr;

		AssetData* assetData = new AssetData;

		AssetFileInfo* assetFile = new AssetFileInfo(folder);
		assetFile->mFilename = assetFilename;
		assetFile->mAssetData = assetData;
		assetFile->mIsFolder = false;
		assetFile->mAssetName = folder->mAssetName + '/' + assetFilename;
		assetFile->mAbsolutePath = folder->mAbsolutePath + '/' + assetFilename;

		assetData->mName = UQString2Name(assetFile->mAssetName);
		assetData->mID = AssetID::GenNew();
		assetData->mClassName = pClass->GetName();

		mAssetsData.Add(assetData);

		return assetData;
	}
#endif // 



	AssetFileInfo* AssetMgrEd::CreateAsset(AssetFileInfo* folder, const QString& assetFilename, TSubClass<Asset> assetClass, const Asset* pClone)
	{
		if (assetFilename.isEmpty()) return nullptr;

		UASSERT(folder && folder->IsFolder());
		UASSERT(assetClass);

		if (folder->HasChild(assetFilename)) return nullptr;

		AssetDataEd* assetData = new AssetDataEd;

		AssetFileInfo* assetFile = new AssetFileInfo(folder);
		assetFile->mFilename = assetFilename;
		assetFile->mAssetData = assetData;
		assetFile->mIsFolder = false;
		assetFile->mAssetName = folder->mAssetName.isEmpty() ? assetFilename : (folder->mAssetName + '/' + assetFilename);
		assetFile->mAbsolutePath = folder->mAbsolutePath + '/' + assetFilename;

		assetData->mName = UQString2Name(assetFile->mAssetName);
		assetData->mID = AssetID::GenNew();
		assetData->mClassName = assetClass->GetName();

		


		Asset* newAsset = nullptr;
		if (pClone)
		{
			UASSERT(assetClass == pClone->GetClass());
			newAsset = UCastSure<Asset>(pClone->Clone());
		}
		else
		{
			newAsset = NewObject<Asset>(assetClass);
		}

		assetData->mInstance = newAsset;
		newAsset->mAssetData = assetData;


		mAssetsData.Add(assetData);

		this->SaveAsset(assetFile);

		return assetFile;
	}

// 	AssetFileInfo* AssetMgrEd::DuplicateAsset(const AssetFileInfo* pAssetFile)
// 	{
// 		if (pAssetFile && pAssetFile->mParent && pAssetFile->mAssetData)
// 		{
// 			//#TODO we generating smart name
// 			QString dupName = pAssetFile->mFilename + "_dup";
// 
// 			auto assetClass = pAssetFile->GetAssetClass();
// 			Asset* pSrcCLone = pAssetFile->mAssetData->LoadNow();
// 			if (pSrcCLone)
// 			{
// 				return CreateAsset(pAssetFile->mParent, dupName, assetClass, pSrcCLone);
// 			}
// 
// 		}
// 		return nullptr;
// 	}

	//////////////////////////////////////////////////////////////////////////
	AssetFileInfo* AssetMgrEd::CreateNewFolder(AssetFileInfo* folderToCreateFolderIn, const QString& folderName)
	{
		if (folderToCreateFolderIn == nullptr || folderName.isEmpty()) return nullptr;

		if (auto folder = folderToCreateFolderIn->HasChild(folderName))
			return nullptr;



		QDir(folderToCreateFolderIn->mAbsolutePath).mkdir(folderName);

		AssetFileInfo* newFolder = new AssetFileInfo(folderToCreateFolderIn);

		newFolder->mAbsolutePath = folderToCreateFolderIn->mAbsolutePath;
		newFolder->mAssetName = folderToCreateFolderIn->mAssetName.isEmpty() ? folderName : (folderToCreateFolderIn->mAssetName + '/' + folderName);
		newFolder->mFilename = folderName;
		newFolder->mIsFolder = true;

		return newFolder;
	}

	bool AssetMgrEd::RenameAsset(AssetFileInfo* assetOrFolderToRename, QString& newName)
	{
		UASSERT(assetOrFolderToRename);
		UASSERT(assetOrFolderToRename->mParent);

		return assetOrFolderToRename->Rename(newName);

	}

	void AssetMgrEd::SaveAsset(Asset* pAsset)
	{
		if (pAsset == nullptr) return;

		if (pAsset->GetAssetData())
		{
			SaveAsset(((AssetDataEd*)pAsset->GetAssetData())->mInfo);
		}
		else
		{
			ULOG_ERROR("Failed. needs AssetData to save the asset");
		}
	}

	void AssetMgrEd::SaveAsset(AssetFileInfo* pAssetFile)
	{

		if (pAssetFile && pAssetFile->mAssetData)
		{
			if (!pAssetFile->mAssetData->GetInstance())
			{
				ULOG_ERROR("cant save asset beause its not loaded");
				return;
			}

			ByteSerializer serHeader;
			UAssetWriteHeader(serHeader, pAssetFile->mAssetData->GetID(), pAssetFile->mAssetData->GetClassName());

			//#TODO 
			bool bCompresed = false;
			bool bUseCockedArchive = true;

			ByteSerializer serData;
			if (USaveArchive(pAssetFile->mAssetData->GetInstance(), serData, bUseCockedArchive, bCompresed))
			{
				QFile file(pAssetFile->mAbsolutePath);
				if (file.open(QFile::OpenModeFlag::WriteOnly))
				{
					file.write((const char*)serHeader.GetData(), serHeader.GetSize());
					file.write((const char*)serData.GetData(), serData.GetSize());
					file.close();

					ULOG_SUCCESS("Asset [%] Saved", pAssetFile->mAbsolutePath);
				}
			}
			else
			{
				ULOG_ERROR("failed to serialize asset");
			}

		}
	}




	bool AssetFileInfo::Rename(const QString& newName)
	{
		QString newAbsolutePath = mParent->mAbsolutePath + '/' + newName;

		if (QDir(mParent->mAbsolutePath).rename(mFilename, newName))
		{
			ULOG_SUCCESS("renamed from % to %", mAbsolutePath, newAbsolutePath);

			mFilename = newName;
			PostRename();
			return true;
		}

		ULOG_WARN("renaming falied");
		return false;
	}

	void AssetFileInfo::PostRename()
	{
		mAbsolutePath = mParent->mAbsolutePath + '/' + mFilename;
		mAssetName = mParent->mAssetName + '/' + mFilename;

		if (mAssetData)
			mAssetData->mName = UQString2Name(mAssetName);

		for (AssetFileInfo* child : mChildren)
		{
			child->PostRename();
		}
	}

};