#pragma once

#include "Base.h"
#include "Scene.h"
#include "SceneRS.h"
#include "Entity.h"


namespace UEngine
{

	
//////////////////////////////////////////////////////////////////////////
class Scene;
class SceneRenderer;
class ViewportBase;

class UENGINE_API Engine
{
	UCLASS(Engine)
};

extern UENGINE_API size_t gFrameCount;
//second since engine start
extern UENGINE_API double gEngineTime;

};
