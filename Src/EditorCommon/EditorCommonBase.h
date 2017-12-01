#pragma once

#include "../Core/Base.h"
#include "../Core/SmartPointers.h"
#include "../Engine/TaskMgr.h"
#include "../GFXCore/Base.h"



//QT without moc
//not working seems a C++14 feature needed
//#include <verdigris/wobjectdefs.h>

#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtGui/qicon.h>
#include <QtCore/QtCore>
#include <QtGui/QtGui>

#ifdef UEDITORCOMMON_SHARED
#ifdef UEDITORCOMMON_BUILD
#define UEDITORCOMMON_API UMODULE_EXPORT
#else
#define UEDITORCOMMON_API  UMODULE_IMPORT
#endif
#else
#define UEDITORCOMMON_API 
#endif

//////////////////////////////////////////////////////////////////////////
class QMenu;

//////////////////////////////////////////////////////////////////////////
inline UCore::StringStreamOut& operator << (UCore::StringStreamOut& stream, const QString str)
{
	stream << str.toStdString();
	return stream;
}


namespace UEditor
{
	using namespace UCore;
	using namespace UEngine;
	using namespace UGFX;



	inline Name UQString2Name(const QString& str)
	{
		return Name(str.toStdString().c_str());
	}
	inline QString UToQString(Name name)
	{
		return QString(name.CStr());
	}

	UEDITORCOMMON_API QIcon& UGetClassIcon(const ClassInfo* pClass);

	template <class T> QIcon& UGetClassIcon()
	{
		return UGetClassIcon(T::GetClassStatic());
	}

	//show a file or folder in explorer
	void UShowInExplorer(const QString& path);

	QMenu* UMenuFindMenu(const QMenu* pMenu, const QString& nameToFind);
};
