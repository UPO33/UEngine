#define IMPL_SCENE_WRAPPER

#include "../GFXVulkan/Wrapper.h"

namespace UGFX
{
	using HGFXCmdList = UGFX::HGFXCmdListVK;
};

#include "SceneRenderer.h"

namespace UGFX
{
	USCENERENDERER_API SceneRendererTest* UCreateSceneRendererTestVK()
	{
		struct Impl : SceneRendererTest
		{
		};
		return new Impl;
	}
};