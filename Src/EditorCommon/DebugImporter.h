#pragma once

#include "AssetInterface.h"
#include "../Engine/DebugAsset.h"

namespace UEditor
{
	class DebugAssetInterface : public AssetInterface
	{
		UCLASS(DebugAssetInterface, AssetInterface)

		DebugAssetInterface()
		{
			this->mIsImportable = true;
			this->mIsUserCreatable = true;

			mClass = ADebugAsset::GetClassStatic();
		}
		virtual const QStringList& GetSupportedExts() override
		{
			static QStringList Exts = { QString("txt") };
			return Exts;
		}
		//is called when context menu requested on this type of asset
		virtual void OnContextMenu(AssetFileInfo* pAssetFile, QMenu* pMenu) 
		{
			pMenu->addAction("Log Hello", pMenu, [=]() {
				ULOG_SUCCESS("Hello World from [%]", pAssetFile->mAbsolutePath);
			});
		}
		
	};
};