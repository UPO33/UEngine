#pragma once

#include "Base.h"
#include "../EditorBaseIM/EditorBaseIM.h"
#include "../EditorBaseIM/ClassHierarchy.h"
//#include "../EditorBaseIM/PropertyBrowser.h"
#include "../EditorBaseIM/EntityBrowser.h"

using namespace UEditor;

//////////////////////////////////////////////////////////////////////////
class WorkSpace : public ISmartBase
{
public:
	Name mName;
	TArray<TSPtr<WBase>> mWindows;

	WorkSpace(const char* name) : mName(name)
	{

	}
	void AddWindow(TSPtr<WBase> newWindow)
	{
		mWindows.Add(newWindow);
	}

	virtual void Update();
};
using WorkSpacePtr = TSPtr<WorkSpace>;

//////////////////////////////////////////////////////////////////////////
class WorkSpaceDebug : public WorkSpace
{
public:

	struct WTest : WBase
	{
		DFilterBox mFilterBox;
		DClassTree mClassTree;

		WTest() : WBase(true, "Test")
		{

		}
		virtual void DrawContents()
		{
			mFilterBox.Draw();
			mClassTree.Draw();
		}
	};


	WTest mTest;

	WorkSpaceDebug() : WorkSpace("Debug") {}


	

	virtual void Update() override;
};



//////////////////////////////////////////////////////////////////////////
class WorkSpaceContext
{
public:
	
	TArray<WorkSpacePtr> mAllWorkSpaces;
	unsigned mCurrentWorkSpaceIndex = 0;

	void AddWorkSpace(WorkSpacePtr ws, bool makeCurrent);
	void Update();
};

extern WorkSpaceContext* gWorkSpace;