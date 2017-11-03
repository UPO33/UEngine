#pragma once

#include "../Core/Base.h"

#ifdef UENGINEBASE_SHARED
#ifdef UENGINEBASE_BUILD
#define UENGINEBASE_API UMODULE_EXPORT
#else
#define UENGINEBASE_API  UMODULE_IMPORT
#endif
#else
#define UENGINEBASE_API 
#endif

namespace UEngine
{
	using namespace UCore;
};