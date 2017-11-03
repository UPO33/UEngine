#pragma once

#include "Base.h"

struct TestStaticMesh
{
	struct Vertex
	{
		float mPosX, mPosY, mPosZ;
		float mNormalX, mNormalY, mNormalZ;
	};

	const char* mFileName = nullptr;
	Vertex* mVertices = nullptr;
	uint16* mIndices = nullptr;
	unsigned mNumVertex = 0;
	unsigned mNumIndex = 0;

	ID3D12Resource* mVB = nullptr;
	ID3D12Resource* mIB = nullptr;


	TestStaticMesh(const char* filename);

	void CreateGFXResources();

	void BindBuffers(ID3D12GraphicsCommandList* cmdList);
};