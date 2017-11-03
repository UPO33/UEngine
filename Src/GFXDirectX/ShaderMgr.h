#include "Base.h"
#include "Shaders.h"

namespace UGFX
{
	//////////////////////////////////////////////////////////////////////////
	class GFXContextDX12;
	class ShaderMgrDX;

	//////////////////////////////////////////////////////////////////////////
	//a shader entry is a set of shaders unique by their name, type and entry. but have many shaders according to macros
	struct ShaderEntryDX : ShaderEntry
	{
		using HashClassT = CRC32;
		using HasherT = THasher<CRC32>;
		using ShaderHashT = uint32;

		//#Must mMacrosHash.Length() == mShaders.Length()
		TArray<ShaderMacrosHashT>			mShadersMacrosHash;
		TArray<GFXShader*>					mShaders;
		ShaderMgrDX*						mShaderMgr;

		//////////////////////////////////////////////////////////////////////////
		ShaderEntryDX(Name filename, Name entrypoint, EShader type, ShaderMgrDX* owner) : mShaderMgr(owner)
		{
			mFilename = filename;
			mEntryPoint = entrypoint;
			mType = type;
		}
		//////////////////////////////////////////////////////////////////////////
		GFXShader* GetShader(const ShaderMacros& macros);
		//////////////////////////////////////////////////////////////////////////
		//compile a new shader with specified macros and return the bytecode
		SmartMemBlock* CompileNewOne(const ShaderMacros& macros);
	};


	//////////////////////////////////////////////////////////////////////////
	class ShaderMgrDX : public IShaderMgr
	{
	public:
		using HashClassT = CRC32;
		using HasherT = THasher<CRC32>;
		using ShaderHashT = uint32;

		ShaderMgrDX(GFXContextDX12* ctx);
		~ShaderMgrDX();

		virtual void RecompileShader(GFXShader* shader) override;
		virtual void RecompileShader(Name filename) override;
		virtual void ReleaseShder(GFXShader*) override;
		virtual GFXShader* GetShader(Name virtualFilename, Name entrypoint, EShader shader, const ShaderMacros& macros) override;

		void GetCompiledShadersByFilename(Name filename, TArray<GFXShader*>& out);
			
		
		const wchar_t* GetEngineShadersDirectory() const;
		const wchar_t* GetProjectShadersDirectory() const;

		//////////////////////////////////////////////////////////////////////////
		bool GetShaderFileFullPath(Name shaderFileName, char out[UMAX_PATH]) const;
		//////////////////////////////////////////////////////////////////////////
		const char* GetCompileTargetByEnum(EShader shader) const
		{
			static const char* target[] = {
				"vs_5_0",
				"hs_5_0",
				"ds_5_0",
				"gs_5_0",
				"ps_5_0",
				"cs_5_0",
			};

			return target[(unsigned)shader];
		}

		
		GFXContextDX12*			mCtx = nullptr;
		TArray<ShaderHashT>		mShadersEntryHash;
		TArray<ShaderEntryDX*>	mShadersEntry;

	};
};