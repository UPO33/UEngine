#pragma once

#include "../Core/Base.h"
#include "../Core/SmartPointers.h"

#include "../EngineBase/Base.h"

#include <QtCore/QtCore>
#include <QtGui/qcolor.h>
#include <QtWidgets/QDockWidget>

//QT without moc
//not working seems a C++14 feature needed
//#include <verdigris/wobjectdefs.h>



#ifdef UEDITORBASE_SHARED
#ifdef UEDITORBASE_BUILD
#define UEDITORBASE_API UMODULE_EXPORT
#else
#define UEDITORBASE_API  UMODULE_IMPORT
#endif
#else
#define UEDITORBASE_API 
#endif

namespace UEditor
{
	using namespace UCore;
	using namespace UEngine;

	
};
