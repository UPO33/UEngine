#pragma once

#include "../Core/Base.h"
#include "../GFXCore/Base.h"

#ifdef UGFXVULKAN_SHARED
#ifdef UGFXVULKAN_BUILD
#define UGFXVULKAN_API UMODULE_EXPORT
#else
#define UGFXVULKAN_API UMODULE_IMPORT
#endif
#else
#define UGFXVULKAN_API
#endif

#include <vulkan/vulkan.hpp>

namespace UGFX
{
	using namespace UCore;
	using namespace UGFX;

#if 0
	vkDestroyBuffer(device,)
#endif

		
	struct GraphicPiplineStateVK : GFXGraphicPiplineState
	{
		VkPipeline mHandle;
	};

	struct ComputePipelineStateVK : GFXComputePiplineState
	{
		VkPipeline mHandle;
	};


};