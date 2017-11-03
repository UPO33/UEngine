#include "Base.h"
#include "../Engine/SceneView.h"

#ifndef IMPL_SCENE_WRAPPER
#include "../GFXCore/WrapperNull.h"
namespace UGFX
{
	using HGFXCmdList = HGFXCmdListNull;
};
#endif

namespace UGFX
{


	class SceneRendererTest
	{
	public:
		GFXResource* mDepthRT = nullptr;

		SceneRendererTest()
		{
			
		}
		
		virtual void Render(SceneViewGroup* views) 
		{

		}
	};





	USCENERENDERER_API SceneRendererTest* UCreateSceneRendererTestDX12();
	USCENERENDERER_API SceneRendererTest* UCreateSceneRendererTestVK();
};