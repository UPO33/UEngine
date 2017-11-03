#pragma once

#include "Base.h"

namespace UGFX
{
	//////////////////////////////////////////////////////////////////////////
	struct HGFXCmdListVK
	{
		VkCommandBuffer mCmdBuffer;

		void Close()
		{
			
		}
		void DrawInstanced(unsigned VertexCountPerInstance, unsigned InstanceCount, unsigned StartVertexLocation, unsigned StartInstanceLocation)
		{
			vkCmdDraw(mCmdBuffer, VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
		}
		void DrawIndexedInstanced(unsigned IndexCountPerInstance, unsigned InstanceCount, unsigned StartIndexLocation, int BaseVertexLocation, unsigned StartInstanceLocation)
		{
			vkCmdDrawIndexed(mCmdBuffer, IndexCountPerInstance, InstanceCount, StartInstanceLocation, BaseVertexLocation, StartInstanceLocation);
		}
		void Dispatch(unsigned ThreadGroupCountX, unsigned ThreadGroupCountY, unsigned ThreadGroupCountZ)
		{
			vkCmdDispatch(mCmdBuffer, ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
		}
	};
};