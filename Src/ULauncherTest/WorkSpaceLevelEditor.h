#include "WorkSpace.h"

#include "../EditorBaseIM/AssetBrowser.h"
#include "../EditorBaseIM/EntityBrowser.h"

class WorkSpaceLevelEditor : public WorkSpace
{
public:
	WorkSpaceLevelEditor() : WorkSpace("LevelEditor")
	{
		mEntityBrowser = new WEntityBrowser;
		mAssetBrowser = new WAssetBrowser;

		this->AddWindow(mEntityBrowser);
		this->AddWindow(mAssetBrowser);
	}

	TSPtr<WEntityBrowser> mEntityBrowser;
	TSPtr<WAssetBrowser> mAssetBrowser;
};