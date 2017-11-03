#pragma once

#include "../Core/Base.h"

#ifdef UENGINE_SHARED
#ifdef UENGINE_BUILD
#define UENGINE_API UMODULE_EXPORT
#else
#define UENGINE_API UMODULE_IMPORT
#endif
#else
#define UENGINE_API
#endif

namespace UGFX
{

};

namespace UEngine
{
	using namespace UCore;
	using namespace UGFX;



};
