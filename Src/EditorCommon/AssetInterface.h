#pragma once

#include "EditorCommonBase.h"
#include "AssetViewer.h"
#include "AssetMgrEd.h"

#include <QtWidgets/qmenu.h>
#include <QtCore/qfileinfo.h>

#include "../Engine/Asset.h"

namespace UEditor
{
	//actions for an asset type
	class UEDITORCOMMON_API AssetActionsBase
	{
		UCLASS(AssetActionsBase)
			 
		//is called when context menu requested on this type of asset
		virtual void OnContextMenu(AssetData* asset, QMenu* menu) {}
	};
	

	//base widget for import options of assets
	class UEDITORCOMMON_API AssetImportOptionBase : public QWidget
	{

	};
	
	class AssetFileInfo;

	//
	class UEDITORCOMMON_API AssetInterface
	{
		UCLASS(AssetInterface)

		bool mIsUserCreatable = true;
		bool mIsImportable = false;
		TSubClass<Asset> mClass;
		
		virtual const QStringList& GetSupportedExts() { return QStringList(); };

		virtual void OnImport(const QString& importingFilePath, AssetFileInfo* dstFolder) {}
		
		//is called when context menu requested on this type of asset requested
		virtual void OnContextMenu(AssetFileInfo* PssetFile, QMenu* pMenu) {}
		virtual AssetWindowBase* OnCreateAssetWindow() { return new AssetWindowDefault; }

		static const TArray<AssetInterface*>& GetInterfaces();
		static void CollectInterfaces();
		static void ReleaseInterfaces();
		//returns the interface for specified asset's class, if not found return default interface
		static AssetInterface* GetInterface(TSubClass<Asset> assetClass);
	};

};