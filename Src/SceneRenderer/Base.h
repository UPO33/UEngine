#pragma once

#include "../Engine/Base.h"
#include "../GFXCore/Base.h"

#ifdef USCENERENDERER_SHARED
#ifdef USCENERENDERER_BUILD
#define USCENERENDERER_API UMODULE_EXPORT
#else
#define USCENERENDERER_API UMODULE_IMPORT
#endif
#else
#define USCENERENDERER_API
#endif


namespace UGFX
{
	using namespace UCore;
	using namespace UEngine;
};