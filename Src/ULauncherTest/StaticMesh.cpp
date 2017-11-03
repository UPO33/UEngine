#include "StaticMesh.h"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

TestStaticMesh::Vertex UVertexFromAssimp(aiVector3D pos, aiVector3D normal)
{
	return TestStaticMesh::Vertex{ pos.x, pos.y, pos.z, normal.x, normal.y, normal.z };
}

TestStaticMesh::TestStaticMesh(const char* filename)
{
	mFileName = filename;

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(filename, aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded);
	UASSERT(scene);
	UASSERT(scene->mNumMeshes);

	aiMesh* pMesh = scene->mMeshes[0];
	UASSERT(pMesh);

	mNumVertex = pMesh->mNumVertices;
	mNumIndex = pMesh->mNumFaces * 3;

	mVertices = new Vertex[mNumVertex];
	mIndices = new uint16[mNumIndex];

	for (unsigned iVertex = 0; iVertex < mNumVertex; iVertex++)
	{
		mVertices[iVertex] = UVertexFromAssimp(pMesh->mVertices[iVertex], pMesh->mNormals[iVertex]);
	}

	for (unsigned iTriangle = 0; iTriangle < mNumIndex / 3; iTriangle++)
	{
		mIndices[iTriangle * 3 + 0] = pMesh->mFaces[iTriangle].mIndices[0];
		mIndices[iTriangle * 3 + 1] = pMesh->mFaces[iTriangle].mIndices[1];
		mIndices[iTriangle * 3 + 2] = pMesh->mFaces[iTriangle].mIndices[2];
	}

	CreateGFXResources();

}

void TestStaticMesh::CreateGFXResources()
{


	mVB = UCreateDefaultCommittedUploadBuffer(sizeof(Vertex) * mNumVertex, mVertices);
	mIB = UCreateDefaultCommittedUploadBuffer(sizeof(uint16) * mNumIndex, mIndices);
}

void TestStaticMesh::BindBuffers(ID3D12GraphicsCommandList* cmdList)
{
	D3D12_VERTEX_BUFFER_VIEW vbv[] = { mVB->GetGPUVirtualAddress(), sizeof(Vertex) * mNumVertex, sizeof(Vertex) };
	cmdList->IASetVertexBuffers(0, 1, vbv);
	D3D12_INDEX_BUFFER_VIEW ibv = { mIB->GetGPUVirtualAddress(), sizeof(uint16) * mNumIndex, DXGI_FORMAT::DXGI_FORMAT_R16_UINT };
	cmdList->IASetIndexBuffer(&ibv);
}

