#pragma once

#include "EditorCommonBase.h"
#include <QtCore/qmap.h>
#include <QtGui/qicon.h>

namespace UEditor
{
	

	class UEDITORCOMMON_API EditorResources
	{
	public:

		QMap<const ClassInfo*, QIcon*> mMapClassToIcon;

		EditorResources();

		QIcon& GetClassIcon(const ClassInfo* pClass);
	};

	extern UEDITORCOMMON_API EditorResources* gEditorResources;
	inline EditorResources* GEditorResources() { return gEditorResources; }
};