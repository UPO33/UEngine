#pragma once

#include "StaticMesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <regex>

namespace UEngine
{
	inline Color UFromAssimp(aiColor4D color)
	{
		return Color(color.r, color.g, color.b, color.a);
	}
	inline Vec3 UFromAssimp(aiVector3D v3)
	{
		return Vec3(v3.x, v3.y, v3.z);
	}
	inline Vec2 UFromAssimp(aiVector2D v2)
	{
		return Vec2(v2.x, v2.y);
	}

	//////////////////////////////////////////////////////////////////////////
	int UGetLODIndexFromName(const char* meshName /*e.g LOD0_MyMesh*/)
	{
		char LODPrefix[] = "LODN_";
		for (int iLOD = 0; iLOD < STATIC_MESH_MAX_LOD; iLOD++)
		{
			LODPrefix[3] = char('0' + iLOD);	//setting N to LODIndex
			if (UStrHasPrefix(meshName, LODPrefix))
				return iLOD;
		}
		return -1;
	}
	//////////////////////////////////////////////////////////////////////////
	const char* UGetMeshNameFromName(const char* meshName)
	{
		return strchr(meshName, '_') + 1;
	}
	//////////////////////////////////////////////////////////////////////////
	aiString UGetAssimpMaterialName(const aiMaterial* pMaterial)
	{
		aiString ret;
		if (pMaterial)
			pMaterial->Get(AI_MATKEY_NAME, ret);
		return ret;
	}
	//////////////////////////////////////////////////////////////////////////
	void UIndicesTo16Bit(const TArray<uint32>& in, TArray<uint16>& out)
	{
		out.RemoveAll();
		out.AddUnInit(in.Length());

		for (unsigned i = 0; i < in.Length(); i++)
		{
			UASSERT(in[i] <= 0xFFff);
			out[i] = (uint16)in[i];
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//@pSections must be valid and have mesh
	void UFillMeshLODFromAssimp(const aiMesh** pSections, unsigned numSections, StaticMeshLOD& out)
	{
		TArray<StaticMeshBaseVertex> vertices;
		vertices.SetCapacity(numSections * 1024);
		TArray<uint32> indices;
		indices.SetCapacity(numSections * 1024);

		out.mSections.AddDefault(numSections);

		for (unsigned iSection = 0; iSection < numSections; iSection++)
		{
			const aiMesh* pMesh = pSections[iSection];

			UASSERT(pMesh);
			UASSERT(pMesh->HasPositions() && pMesh->HasNormals() && pMesh->HasFaces());
			UASSERT(pMesh->mPrimitiveTypes == aiPrimitiveType::aiPrimitiveType_TRIANGLE);

			out.mSections[iSection].mMaterialIndex = pMesh->mMaterialIndex;
			out.mSections[iSection].mNumIndices = pMesh->mNumFaces * 3;
			out.mSections[iSection].mIndexOffset = indices.Length();

			for (unsigned iFace = 0; iFace < pMesh->mNumFaces; iFace++)
			{
				for(unsigned iTri = 0; iTri < 3; iTri++)
				{
					unsigned index = pMesh->mFaces[iFace].mIndices[iTri];

					StaticMeshBaseVertex vertex;
					
					vertex.mPosition = UFromAssimp(pMesh->mVertices[index]);
					vertex.mNormal = UFromAssimp(pMesh->mNormals[index]);
					//tangent is valid if mesh has valid UV
					vertex.mTangent = pMesh->HasTangentsAndBitangents() ? UFromAssimp(pMesh->mTangents[index]) : Vec3(0, 0, 0);
					vertex.mUV = pMesh->HasTextureCoords(0) ? UFromAssimp(pMesh->mTextureCoords[0][index]) : Vec2(0, 0);

					size_t newIndex = vertices.AddUniqueLamda([](StaticMeshBaseVertex& iter, StaticMeshBaseVertex& test) {
						return iter.IsNearlyEqual(test);
					}, vertex);

					indices.Add((uint32)newIndex);

				}
				
			}
		}

		out.mVerticesBase = vertices;
		out.mNumVertices = vertices.Length();
		out.mNumIndices = indices.Length();
		bool bUse16BitIndex = vertices.Length() < 0xFFff;
		out.mHas16BitIndex = bUse16BitIndex;
		if (bUse16BitIndex)
		{
			UIndicesTo16Bit(indices, out.mIndices16Bit);
		}
		else
		{
			out.mIndices32Bit = indices;
		}
		
		
	}
	//////////////////////////////////////////////////////////////////////////
	AStaticMesh* AStaticMesh::ImportFromAssetFile(const TSPtr<SmartMemBlock>& file)
	{
		Assimp::Importer importer;

		unsigned importFlags = aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded | aiProcess_RemoveRedundantMaterials | aiProcess_CalcTangentSpace;

		const aiScene* pScene = importer.ReadFileFromMemory(file->Memory(), file->Size(), importFlags);
		if (pScene == nullptr)
		{
			ULOG_ERROR("%", importer.GetErrorString());
			return nullptr;
		}

		if (pScene->mNumMeshes == 0)
		{
			ULOG_ERROR("imported model has no meshes");
			return nullptr;
		}
		
		aiNode* meshLODNodes[STATIC_MESH_MAX_LOD] = {};
		unsigned numLOD = STATIC_MESH_MAX_LOD;
		//extract LODS
		for (unsigned n = 0; n < pScene->mRootNode->mNumChildren; n++)
		{
			aiNode* child = pScene->mRootNode->mChildren[n];
			auto childName = child->mName.C_Str();
			
			ULOG_MESSAGE("NodeName: %", childName);

			int LODIndex = UGetLODIndexFromName(childName);
			if (LODIndex != -1)
			{
				meshLODNodes[LODIndex] = child;
			}
		}
		
		ArrayConditionalRemovePOD(meshLODNodes, numLOD, [&](aiNode* pNode) {
			if (pNode == nullptr)return true;
			if (pNode->mNumMeshes == 0) return true;
			return false;
		});

		AStaticMesh* outputStaticMesh = NewObject<AStaticMesh>();
		outputStaticMesh->mLODDs.SetCapacity(numLOD);

		//initializing materials
		outputStaticMesh->mMaterials.AddDefault(pScene->mNumMaterials);
		for (unsigned iMaterial = 0; iMaterial < pScene->mNumMaterials; iMaterial++)
		{
			aiMaterial* pMaterial = pScene->mMaterials[iMaterial];
			if (pMaterial)
			{
				aiString materialName = UGetAssimpMaterialName(pMaterial);
				outputStaticMesh->mMaterials[iMaterial].mSlot = Name(materialName.C_Str());
			}
		}

		//initialize LODs 
		for (unsigned iLOD = 0; iLOD < numLOD; iLOD++)
		{
			const aiMesh* sectionsMesh[STATIC_MESH_MAX_LOD];
			unsigned numSectionsMesh = 0;

			
			for (unsigned iSection = 0; iSection < meshLODNodes[iLOD]->mNumMeshes; iSection++)
			{
				if (const aiMesh* mesh = pScene->mMeshes[meshLODNodes[iLOD]->mMeshes[iSection]])
				{
					if (mesh->mNumVertices && mesh->mNumFaces)
					{
						sectionsMesh[numSectionsMesh] = mesh;
						numSectionsMesh++;
					}
				}
			}

			if (numSectionsMesh)
			{
				outputStaticMesh->mLODDs.Add();
				StaticMeshLOD& smLOD = outputStaticMesh->mLODDs.LastElement();
				UFillMeshLODFromAssimp(sectionsMesh, numSectionsMesh, smLOD);
			}
		}

		outputStaticMesh->CalcBound();
		outputStaticMesh->CreateRS();

		
		

		
		
		
		return outputStaticMesh;

	}
};