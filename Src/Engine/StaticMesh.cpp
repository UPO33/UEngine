#include "StaticMesh.h"
#include "../GFXCore/Base.h"

namespace UEngine
{
	
bool StaticMeshBaseVertex::IsNearlyEqual(const StaticMeshBaseVertex& other, float epsilon) const
{
	return
		mPosition.IsNearlyEqual(other.mPosition, epsilon) &&
		mNormal.IsNearlyEqual(other.mNormal, epsilon) &&
		mTangent.IsNearlyEqual(other.mTangent, epsilon) &&
		mUV.IsNearlyEqual(other.mUV, epsilon);
}

AMaterial* AStaticMesh::GetMaterial(unsigned index)
{
	if (index < mMaterials.Length())
	{
		return mMaterials[index].mMaterial;
	}
	return nullptr;
}

AMaterial* AStaticMesh::GetMaterial(Name tag)
{
	if (tag.IsEmpty()) return nullptr;

	for (const MaterialSlot& material : mMaterials)
	{
		if (material.mSlot == tag)
			return material.mMaterial.Get();
	}
	return nullptr;
}



bool AStaticMesh::IsRenderValid() const
{
	return mLODDs.Length() != 0;
}

void AStaticMesh::CreateRS()
{
	AStaticMeshRS* rs = new AStaticMeshRS(this);
}

void AStaticMesh::OnPostPropertyChange(const PropertyChangedEvent& pce)
{
	ParentT::OnPostPropertyChange(pce);
}

void AStaticMesh::OnPrePropertyChange(const PropertyChangedEvent& pce)
{
	ParentT::OnPrePropertyChange(pce);
}

void AStaticMesh::CalcBound(bool bRecalculateLODsBound)
{
	BoundingBox box;
	for (auto LOD : mLODDs)
	{
		if (bRecalculateLODsBound)
			LOD.CalcBound();
		box.Enclose(LOD.mBound);
	}

	mBound = box;
}


//////////////////////////////////////////////////////////////////////////
void StaticMeshLOD::CalcBound()
{
	mBound = BoundingBox();

	if (mVerticesBase.Length() >= 2)
		mBound = BoundingBox::MakeFromPoints(&(mVerticesBase[0].mPosition), mVerticesBase.Length(), sizeof(StaticMeshBaseVertex));
}
//////////////////////////////////////////////////////////////////////////
AStaticMeshRS::AStaticMeshRS(AStaticMesh* pOwner) :
	mBound(pOwner->mBound),
	mMaxLODIndex(pOwner->mMaxLODIndex)
{


	mLODS.AddDefault(pOwner->mLODDs.Length());

	for(size_t iLOD = 0; iLOD < pOwner->mLODDs.Length(); iLOD++)
	{
		mLODS[iLOD].Init(pOwner->mLODDs[iLOD]);
	}
}


//////////////////////////////////////////////////////////////////////////
void StaticMeshLODRS::Init(StaticMeshLOD& meshLOD)
{
	//copying sections data
	mNumSelections = meshLOD.mSections.Length();
	for (unsigned iSection = 0; iSection < mNumSelections; iSection++)
	{
		mSelections[iSection] = meshLOD.mSections[iSection];
	}

	//create depth only buffers
	{
		TArray<Vec3> vertices (meshLOD.NumVertices());
		//only one of these two is used
		TArray<uint32> indices32Bit (meshLOD.NumIndices());
		TArray<uint32> indices16Bit (meshLOD.NumIndices());

		//indexing position only vertices, dose not take other vertex attributes into account
		for (unsigned iIndex = 0;  iIndex < meshLOD.NumIndices(); iIndex++)
		{
			auto vertex = meshLOD.mVerticesBase[meshLOD.GetIndexAt(iIndex)];

			auto newIndex = vertices.AddUniqueLamda([](const Vec3& iter, const Vec3& cmp) {
				return iter.IsNearlyEqual(cmp);
			}, vertex.mPosition);

			if (meshLOD.IsIndex16Bit())
				indices16Bit.Add((uint16)newIndex);
			else
				indices32Bit.Add((uint32)newIndex);
		}

		size_t vertexBufferSize = vertices.Length() * sizeof(Vec3);
		mDepthOnlyVertexPosition = GetGFXContext()->CreateStaticVB(vertexBufferSize, vertices.Elements());
		UASSERT(mDepthOnlyVertexPosition);

		if(meshLOD.IsIndex16Bit())
		{
			size_t indexBufferSize = indices16Bit.Length() * 2;
			mDepthOnlyIndexBuffer = GetGFXContext()->CreateStaticIB(indexBufferSize, indices16Bit.Elements());
			UASSERT(mDepthOnlyVertexPosition);
		}
		else
		{
			size_t indexBufferSize = indices16Bit.Length() * 4;
			mDepthOnlyIndexBuffer = GetGFXContext()->CreateStaticIB(indexBufferSize, indices32Bit.Elements());
			UASSERT(mDepthOnlyIndexBuffer);
		}
	}

	//create index buffer
	{
		size_t bufferSize = meshLOD.IndexSize() * meshLOD.NumIndices();
		mIndexBuffer = GetGFXContext()->CreateStaticIB(bufferSize, meshLOD.GetIndicesRaw());
		UASSERT(mIndexBuffer);
	}

	//create vertex buffers
	{
		size_t bufferSize = meshLOD.mVerticesBase.Length() * sizeof(StaticMeshBaseVertex);
		mVertexPositionNormalTangentUV = GetGFXContext()->CreateStaticVB(bufferSize, meshLOD.mVerticesBase.Elements());
		UASSERT(mVertexPositionNormalTangentUV);
	}


}

};