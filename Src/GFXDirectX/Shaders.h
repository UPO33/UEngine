#pragma once

#include "Base.h"
#include "../Core/Hash.h"
#include "../Core/SmartMemory.h"
#include "../Core/SmartPointers.h"
#include "../Core/FileSys.h"

#include <d3dcompiler.h>

namespace UGFX
{

	//////////////////////////////////////////////////////////////////////////
	struct ShaderDX12 : GFXShader
	{
		TSPtr<SmartMemBlock> mByteCode;

		ShaderDX12(ShaderEntry* pEntry, TSPtr<SmartMemBlock> byteCode, const ShaderMacros& marcos)
		{
			mEntry = pEntry;
			mByteCode = byteCode;
			mMacros = mMacros;
		}
		D3D12_SHADER_BYTECODE GetDXByteCode() const
		{
			return D3D12_SHADER_BYTECODE{ mByteCode->Memory(), mByteCode->Size() };
		}
	};

	//////////////////////////////////////////////////////////////////////////
	struct ShaderDX11 : GFXShader
	{
		ID3D11DeviceChild* mShader;
	};


	
};
