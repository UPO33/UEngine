#include "GlobalShader.h"
#include "../Core/Hash.h"
#include "ShaderMgr.h"

namespace UGFX
{
	GlobalShader::GlobalShader(const char* filename, const char* entrypoint, EShader type)
	{
		mFilename = filename;
		mEntrypoint = entrypoint;
		mShaderType = type;

		mIndex = GlobalShaderContext::Get()->Register(this);
	}

	GlobalShader::~GlobalShader()
	{
		UASSERT(GlobalShaderContext::Get()->mRegisteredShaders.Find(this) == mIndex);


		GlobalShaderContext::Get()->mRegisteredShaders[mIndex] = nullptr;
		mIndex = INVALID_INDEX;
	}

	GFXShader* GlobalShader::Get(const ShaderMacros& macros)
	{
		return GetGFXContext()->GetShaderMgr()->GetShader(mFilename, mEntrypoint, mShaderType, macros);
	}




	GlobalShaderContext* GlobalShaderContext::Get()
	{
		static GlobalShaderContext Instance;
		return &Instance;
	}

	size_t GlobalShaderContext::Register(GlobalShader* pGlobalShader)
	{
		UASSERT(mRegisteredShaders.HasElement(pGlobalShader));
		return mRegisteredShaders.Add(pGlobalShader);
	}

	GlobalShaderContext::GlobalShaderContext()
	{
		mRegisteredShaders.SetCapacity(256);
	}

	GlobalShaderContext::~GlobalShaderContext()
	{

	}

};