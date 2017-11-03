#pragma once

#include "Base.h"



namespace UGFX
{
	class UGFXVULKAN_API GFXContextVulkan : public GFXContextBase
	{
		UCLASS(GFXContextVulkan, GFXContextBase)

		vk::Instance		mIntance;
		vk::Device			mDevice;
		vk::PhysicalDevice	mPhysicalDevice;	//aka adapter

		void Init() override
		{
			vk::InstanceCreateInfo ciInstance;

			mIntance = vk::createInstance(ciInstance);

			uint32_t numDevice = 0;
			vk::PhysicalDevice devices[32];
			mIntance.enumeratePhysicalDevices(&numDevice, devices);
		}
	};

	


	struct UGFXVULKAN_API ShaderVK : GFXShader
	{
		ShaderVK(vk::Device device, void* pCode, size_t codeSize, EShader shder)
			: GFXShader(shder)
		{
			mDevice = device;

			vk::ShaderModuleCreateInfo cinf;
			cinf.pCode = (uint32_t*)pCode;
			cinf.codeSize = codeSize;
			mShaderModule = device.createShaderModule(cinf);

			mShaderState = vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eVertex, mShaderModule, nullptr, nullptr);
		}
		void Release()
		{
			mDevice.destroyShaderModule(mShaderModule);
		}

		vk::Device			mDevice;
		vk::ShaderModule	mShaderModule;
		vk::PipelineShaderStageCreateInfo mShaderState;
	};



	struct PiplineStateVK : GFXPiplineState
	{
		vk::Pipeline mPSO;
		vk::GraphicsPipelineCreateInfo mDesc;
	};

};
