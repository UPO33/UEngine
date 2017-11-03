#include "Base.h"
#include "../Core/Meta.h"
#include "../Core/Hash.h"

namespace UGFX
{
	UCLASS_BEGIN_IMPL(GFXContextBase)
	UCLASS_END_IMPL(GFXContextBase)

	


};
namespace UGFX
{
	GFXContextBase* gGFXContext = nullptr;

	UGFXCORE_API const char* UEnumToStr(EShader in)
	{
		static const char* LUT[] = 
		{
			"VertexShader", "HullShader", "DomainShader", "GeometryShader", "PixelShader", "ComputeShader"
		};
		
		return LUT[(unsigned)in];
	}

	UGFXCORE_API GFXContextBase* GetGFXContext()
	{
		return gGFXContext;
	}
	UGFXCORE_API void SetGFXContext(GFXContextBase* in)
	{
		gGFXContext = in;
	}


	ShaderMacros& ShaderMacros::Add(ShaderMacro newMacro)
	{
		mMacros.Add(newMacro);
		return *this;
	}

	ShaderMacrosHashT ShaderMacros::GetHash() const
	{
		THasher<CRC32>	hasher;
		for (size_t iMacro = 0; iMacro < mMacros.Length(); iMacro++)
		{
			ShaderMacro macro = mMacros[iMacro];
			hasher.Add(macro.mName);
			hasher.Add(macro.mValue);
		}
		return hasher.GetHash();
	}

	UGFXCORE_API StringStreamOut& operator<<(StringStreamOut& stream, const ShaderMacros& macros)
	{
		for (size_t i = 0; i < macros.Num(); i++)
		{
			stream << macros.Get(i).mName << " = " << macros.Get(i).mValue << '\n';
		}
		return stream;
	}



	GFXGraphicsPiplineStateDesc::HashT GFXGraphicsPiplineStateDesc::GetUniqueHash() const
	{
		char buffer[sizeof(GFXGraphicsPiplineStateDesc)];
		MemZero(buffer);
		new (buffer) GFXGraphicsPiplineStateDesc(*this);
	
		return CRC32::HashBuffer(buffer, sizeof(buffer));
	}

};