#pragma once

#include "Base.h"
#include "../Core/Array.h"
#include "../Core/Name.h"


namespace UGFX
{
	//////////////////////////////////////////////////////////////////////////
	class GlobalShader;
	class ShaderMacros;

	class UGFXCORE_API GlobalShaderContext
	{
	public:
		GlobalShaderContext();
		~GlobalShaderContext();

		static GlobalShaderContext* Get();
		
		TArray<GlobalShader*>			mRegisteredShaders;
		size_t							Register(GlobalShader* pGlobalShader);
	};

	//////////////////////////////////////////////////////////////////////////
	class UGFXCORE_API GlobalShader
	{
	public:

		GlobalShader(const char* filename, const char* entrypoint, EShader type);
		~GlobalShader();

		GFXShader* Get(const ShaderMacros& macros);

	private:
		Name				mFilename;
		Name				mEntrypoint;
		EShader				mShaderType;

		size_t				mIndex;
	};

	//////////////////////////////////////////////////////////////////////////
	template<class TShader> class TGlobalShader : public GlobalShader
	{
	public:
		TGlobalShader(const char* filename, const char* entrypoint)
			: GlobalShader(filename, entrypoint, TShader::EnumType) {}

		TShader* Get(const ShaderMacros& macros) { return (TShader*) this->Get(macros); }
	};
};