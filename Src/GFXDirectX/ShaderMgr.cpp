#include "ShaderMgr.h"
#include "PipelineStateMgr.h"
#include "Context.h"

namespace UGFX
{
	//////////////////////////////////////////////////////////////////////////
	//helper to construct an array of 'D3D_SHADER_MACRO' from 'ShaderMacros'
	struct D3DShaderMacros
	{
		D3D_SHADER_MACRO	mMacros[SHADER_MAX_MACRO + 1];

		D3DShaderMacros(const ShaderMacros& inMacros)
		{
			for (size_t i = 0; i < inMacros.Num(); i++)
			{
				mMacros[i] = { inMacros.Get(i).mName, inMacros.Get(i).mValue };
			}
			//must be null terminated
			mMacros[inMacros.Num()] = D3D_SHADER_MACRO{ nullptr, nullptr };
		}
	};

	ShaderMgrDX::ShaderMgrDX(GFXContextDX12* ctx) : mCtx(ctx)
	{

	}

	ShaderMgrDX::~ShaderMgrDX()
	{
		mCtx = nullptr;
	}

	//////////////////////////////////////////////////////////////////////////
	void ShaderMgrDX::RecompileShader(GFXShader* shader)
	{
	
	}

	void ShaderMgrDX::RecompileShader(Name filename)
	{
		PipelineStateMgrDX12* psMgr = dynamic_cast<PipelineStateMgrDX12*>(mCtx->GetPipelineStateMgr());

		TArray<GFXShader*> shadersNeedToBeRecompiled;
		shadersNeedToBeRecompiled.SetCapacity(1024);

		GetCompiledShadersByFilename(filename, shadersNeedToBeRecompiled);

		ULOG_MESSAGE("recompiling % shaders", shadersNeedToBeRecompiled.Length());

		for (GFXShader* pShader : shadersNeedToBeRecompiled)
		{
			if (ShaderDX12* pShaderDX = (ShaderDX12*)pShader)
			{
				pShaderDX->mByteCode = ((ShaderEntryDX*)pShader->mEntry)->CompileNewOne(pShader->mMacros);
			}
		}
#if 0
		//find and recompile the shaders
		{
			for (ShaderEntryDX* entry : this->mShadersEntry)
			{
				if (entry && entry->mFilename == filename)
				{
					for (unsigned iShader = 0; iShader < entry->mShaders.Length(); iShader++) //loop over shaders in an entry
					{
						ShaderDX12* pShader = (ShaderDX12*)(entry->mShaders[iShader]);

						shadersNeedToBeRecompiled.Add(pShader);

						pShader->mByteCode = entry->CompileNewOne(pShader->mMacros);
					}
				}
			}
		}
#endif // 

		TArray<GFXPiplineState*> piplinesNeedToBeRecreated;
		piplinesNeedToBeRecreated.SetCapacity(4096);

		//get graphics pipelines that need to be recreated
		for (GraphicsPiplineStateDX12* gps : psMgr->mGraphicStates)
		{
			if (gps)
			{
				for (GFXShader* pShader : shadersNeedToBeRecompiled)
				{
					if (gps->mDesc.HasSuchShader(pShader))
					{
						piplinesNeedToBeRecreated.AddUnique(gps);
						continue;
					}
				}
			}
		}

		//get compute pipelines
		for (ComputePipelineStateDX12* cps : psMgr->mComputeStates)
		{
			if (cps)
			{
				for (GFXShader* pShader : shadersNeedToBeRecompiled)
				{
					if (cps->mDesc.mCS == pShader)
					{
						piplinesNeedToBeRecreated.AddUnique(cps);
						continue;
					}
				}
			}
		}

		ULOG_MESSAGE("recreating % PiplineState ...", piplinesNeedToBeRecreated.Length());

		for (GFXPiplineState* pso : piplinesNeedToBeRecreated)
		{
			psMgr->RecreatePipelineState(pso);
		}

	}

	void ShaderMgrDX::ReleaseShder(GFXShader*)
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	GFXShader* ShaderMgrDX::GetShader(Name virtualFilename, Name entrypoint, EShader shader, const ShaderMacros& macros)
	{
		if (entrypoint.IsEmpty())
		{
			static Name DefaultEntryPoint = "Main";
			entrypoint = DefaultEntryPoint;
		}

		ShaderHashT baseHash = 0;
		//calculate base hash (filename + entry point + shader type)
		{
			HasherT hasher;
			hasher << virtualFilename << entrypoint << shader;
			baseHash = hasher.GetHash();
		}

		auto foundIndex = mShadersEntryHash.Find(baseHash);
		if (foundIndex == INVALID_INDEX)
		{
			char shaderFilePath[512];
			GetShaderFileFullPath(virtualFilename, shaderFilePath);

			GFXShader* errorShader = nullptr;

			if (!UFileExist(shaderFilePath))
			{
				ULOG_ERROR("failed to load shader [%]. file not found", virtualFilename);
				return errorShader;
			}

			mShadersEntryHash.Add(baseHash);
			mShadersEntry.Add(new ShaderEntryDX(virtualFilename, entrypoint, shader, this));
			foundIndex = mShadersEntry.Length() - 1;
			UASSERT(mShadersEntry.Length() == mShadersEntryHash.Length());
		}

		return mShadersEntry[foundIndex]->GetShader(macros);
	}

	//////////////////////////////////////////////////////////////////////////
	void ShaderMgrDX::GetCompiledShadersByFilename(Name filename, TArray<GFXShader*>& out)
	{
		out.RemoveAll();

		for (ShaderEntryDX* entry : this->mShadersEntry)
		{
			if (entry && entry->mFilename.Equal(filename, false))
			{
				out.Append(entry->mShaders);
			}
		}
	}

	const wchar_t* ShaderMgrDX::GetEngineShadersDirectory() const
	{
		return nullptr;
	}

	const wchar_t* ShaderMgrDX::GetProjectShadersDirectory() const
	{
		return nullptr;
	}

	bool ShaderMgrDX::GetShaderFileFullPath(Name shaderFileName, char out[UMAX_PATH]) const
	{
		sprintf(out, "%s%s", "../../Shaders/", shaderFileName.CStr());
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	GFXShader* ShaderEntryDX::GetShader(const ShaderMacros& macros)
	{
		auto macrosHash = macros.GetHash();

		auto foundIndex = mShadersMacrosHash.Find(macrosHash);
		if (foundIndex != INVALID_INDEX)
		{
			if (GFXShader* foundShader = mShaders[foundIndex])
			{
				ULOG_SUCCESS("found the Shader");
				return foundShader;
			}

			ULOG_MESSAGE("shader found but is null! try to recompile or what?");
			return nullptr;
		}
		else
		{
			if (SmartMemBlock* byteCode = CompileNewOne(macros))
			{
				GFXShader* newShader = new ShaderDX12(this, byteCode, macros);
				mShadersMacrosHash.Add(macrosHash);
				mShaders.Add(newShader);
				return newShader;
			}
			else // shader found but failed to compile?
			{
				mShadersMacrosHash.Add(macrosHash);
				mShaders.Add(nullptr);
			}
			return nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	SmartMemBlock* ShaderEntryDX::CompileNewOne(const ShaderMacros& macros)
	{
		char shaderFilePath[512];
		mShaderMgr->GetShaderFileFullPath(this->mFilename, shaderFilePath);

		TSPtr<SmartMemBlock> shaderFileContent = UFileOpenReadFull(shaderFilePath);
		if (!shaderFileContent)
		{
			ULOG_ERROR("faile to load shader file [%]", mFilename);
			return nullptr;
		}

		const char* target = mShaderMgr->GetCompileTargetByEnum(mType);

		ID3DBlob*	blobByteCode = nullptr;
		ID3DBlob*	blobError = nullptr;

		GFXShader* shader = nullptr;

		ID3DBlob* blobPreprocessed = nullptr;

		D3DShaderMacros dxMacros = D3DShaderMacros(macros);

		uint32 shaderContentHash = HashClassT::GetPrime();

		SmartMemBlock* result = nullptr;

		if (SUCCEEDED(D3DPreprocess(shaderFileContent->Memory(), shaderFileContent->Size(), shaderFilePath
			, dxMacros.mMacros, D3D_COMPILE_STANDARD_FILE_INCLUDE, &blobPreprocessed, &blobError)))
		{
			shaderContentHash = HashClassT::HashBuffer(blobPreprocessed->GetBufferPointer(), blobPreprocessed->GetBufferSize());
		}
		else
		{
			ULOG_ERROR("failed to preprocess shader [%][%][%]\n%\nErros:\n%\n", 
				UEnumToStr(mType), mFilename, mEntryPoint, macros, (const char*)blobError->GetBufferPointer());
			
			blobError->Release();
			return nullptr;
		}

		UINT compileFlag0 = D3DCOMPILE_DEBUG;

		if (SUCCEEDED(D3DCompile(blobPreprocessed->GetBufferPointer(), blobPreprocessed->GetBufferSize(), nullptr, nullptr, nullptr
			, mEntryPoint.CStr(), target, compileFlag0, 0, &blobByteCode, &blobError)))
		{
			result = new SmartMemBlock(blobByteCode->GetBufferPointer(), blobByteCode->GetBufferSize(), true);

			
			blobPreprocessed->Release();
			blobByteCode->Release();

			ULOG_SUCCESS("Shader [%][%][%] compiled", mFilename, mEntryPoint, UEnumToStr(mType));

			return result;
		}
		else
		{
			ULOG_ERROR("failed to compile shader [%][%][%]\n%\nErros:\n%\n",
				UEnumToStr(mType), mFilename, mEntryPoint, macros, (const char*)blobError->GetBufferPointer());
		}
		return nullptr;
	}

};