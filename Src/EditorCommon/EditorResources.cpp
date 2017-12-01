#include "EditorResources.h"

#include "../Core/Meta.h"
#include "Project.h"

namespace UEditor
{
	UEDITORCOMMON_API EditorResources* gEditorResources;
	//////////////////////////////////////////////////////////////////////////
	UEDITORCOMMON_API QIcon& UGetClassIcon(const ClassInfo* pClass)
	{
		return GEditorResources()->GetClassIcon(pClass);
	}
	//////////////////////////////////////////////////////////////////////////
	EditorResources::EditorResources()
	{
		mMapClassToIcon[nullptr] = new QIcon();
	}
	//////////////////////////////////////////////////////////////////////////
	QIcon& EditorResources::GetClassIcon(const ClassInfo* pClass)
	{
		if (mMapClassToIcon.contains(pClass))
			return *(mMapClassToIcon[pClass]);

		if (pClass)
		{
			if (auto attr = pClass->GetAttribute<AttrIcon>())
			{
				if (attr->mIconName)
				{
					QString iconPath;
					QIcon* icon = nullptr;

					iconPath = UGetCurrentProjectDirectory() + QString("/Resources/Icons/") + QString(attr->mIconName);
					if (QFile::exists(iconPath))
					{
						icon = new QIcon(iconPath);
					}
					else
					{
						iconPath = UGetEngineDirectory() + QString("/Resources/Icons/") + QString(attr->mIconName);
						icon = new QIcon(iconPath);
					}

					mMapClassToIcon[pClass] = icon;
					return *icon;
				}
			}
		}

		return *(mMapClassToIcon[nullptr]);
	}

};