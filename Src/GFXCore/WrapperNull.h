#pragma once

#include "Base.h"

namespace UGFX
{
	//////////////////////////////////////////////////////////////////////////
	struct HGFXNull
	{
		void SetName(const wchar_t* in)
		{
		}
	};

	//////////////////////////////////////////////////////////////////////////
	struct HGFXCmdAllocator : public HGFXNull
	{
		void Reset()
		{
		}
	};
	//////////////////////////////////////////////////////////////////////////
	struct HGFXResourceNull : public HGFXNull
	{
	};
	//////////////////////////////////////////////////////////////////////////
	struct HPipelineStateNull : public HGFXNull
	{
	};

	//////////////////////////////////////////////////////////////////////////
	struct HQueryHeapNull
	{
	};

	//////////////////////////////////////////////////////////////////////////
	struct HCmdListNull 
	{
	};

	//////////////////////////////////////////////////////////////////////////
	struct HGFXCmdListNull
	{
		void Close()
		{
		}
		void DrawInstanced(unsigned VertexCountPerInstance, unsigned InstanceCount, unsigned StartVertexLocation, unsigned StartInstanceLocation)
		{
		}
		void DrawIndexedInstanced(unsigned IndexCountPerInstance, unsigned InstanceCount, unsigned StartIndexLocation, int BaseVertexLocation, unsigned StartInstanceLocation)
		{
		}
		void Dispatch(unsigned ThreadGroupCountX, unsigned ThreadGroupCountY, unsigned ThreadGroupCountZ)
		{
		}
	};
};