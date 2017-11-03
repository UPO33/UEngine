#pragma once

#include "EditorBaseIM.h"
#include "../IMGUI/imgui.h"

namespace UEditor
{
	struct AssetBrowserFolderNode
	{

	};
	struct AssetBrowserAssetNode
	{

	};

	class WAssetBrowser : public WBase
	{
		DFilterBox mFolderFilter;
		DFilterBox mAssetsFilter;

		int mAssetNumColumn = 1;

	public:
		WAssetBrowser();
		virtual void DrawContents() override;
		void DrawFolders();
		void DrawAssets();
	};
};