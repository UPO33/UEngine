#pragma once

#include "EditorCommonBase.h"

namespace UEditor
{
	struct ProjectInfo
	{
		QString mName;
		QString mAbsoluteDir;
		QString mAbsoluteAssetsDir;
		QString mAbsoluteShadersDir;
	};

	extern UEDITORCOMMON_API ProjectInfo* gProject;

	inline QString UGetCurrentProjectDirectory()
	{
		return gProject ? gProject->mAbsoluteDir : QString();
	}
	inline QString UGetEngineDirectory() 
	{
		return QString("../../");
	}
		 
};