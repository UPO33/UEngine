#pragma once

#include "AssetViewer.h"
#include "../Engine/DebugAsset.h"

#include <QtWidgets/qlistwidget.h>

namespace UEditor
{
	class DebugAssetViewer : public AssetWindowBase
	{
	public:
		DebugAssetViewer(QWidget* p) : AssetWindowBase(p)
		{
			mList = new QListWidget(this);
			this->setCentralWidget(mList);
		}

		QListWidget* mList;
		ADebugAsset* GetAsset() const { return UCastSure<ADebugAsset>(AssetWindowBase::GetAsset()); }
	};
};