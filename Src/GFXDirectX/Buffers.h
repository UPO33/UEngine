#include "Base.h"

namespace UGFX
{
//////////////////////////////////////////////////////////////////////////
struct BufferDX12 : GFXBuffer
{
	BufferDX12(ID3D12Resource* handle)
	{
		mNativeHandle = handle;
	}

	ID3D12Resource* GetHandle() const { return reinterpret_cast<ID3D12Resource*>(mNativeHandle); }
};


};