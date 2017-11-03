#define IMPL_SCENE_WRAPPER

#include "../GFXDirectX/Wrapper.h"

namespace UGFX
{
	using HGFXCmdList = UGFX::HCmdListDX12;
};

#include "SceneRenderer.h"

namespace UGFX
{
	USCENERENDERER_API SceneRendererTest* UCreateSceneRendererTestDX12()
	{
		struct Impl : SceneRendererTest
		{
		};
		return new Impl;
	}
};