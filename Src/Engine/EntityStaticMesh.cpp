#include "EntityStaticMesh.h"
#include "StaticMesh.h"
#include "Material.h"

namespace UEngine
{
#ifdef TEST
	void EntityStaticMesh::SetMesh(AStaticMesh* newMesh)
	{

	}

	void EntityStaticMesh::SetMaterial(unsigned index, AMaterial* material)
	{
		if (mUseOverrideMaterials && mMesh && index < mOverrideMaterials.Length())
		{
			if (mOverrideMaterials[index].Get() != material)
			{
				mOverrideMaterials[index] = material;
				MarkRSStateDirty();
			}
		}
	}

	void EntityStaticMesh::SetMaterial(Name name, AMaterial* material)
	{
		if (mMesh)
		{
			UASSERT(mMesh->NumMaterial() == mOverrideMaterials.Length());

			unsigned materialIndex =  mMesh->FindMaterial(name);
			if (materialIndex != INVALID_INDEX)
			{
				if (mOverrideMaterials[materialIndex].Get() != material)
				{
					mOverrideMaterials[materialIndex] = material;
					MarkRSStateDirty();
				}
			}
		}
	}

	AMaterial* EntityStaticMesh::GetMaterial(unsigned index)
	{
		if (mMesh && index < mMesh->NumMaterial())
		{
			if (mUseOverrideMaterials)
			{
				return mOverrideMaterials[index];
			}
			else
			{
				mMesh->GetMaterial(index);
			}
		}
		return nullptr;
	}

	AMaterial* EntityStaticMesh::GetMaterial(Name tag)
	{
		if (mMesh)
		{
			if (mUseOverrideMaterials)
			{
				unsigned materialIndex = mMesh->FindMaterial(tag);
				if (materialIndex != INVALID_INDEX)
				{
					return mOverrideMaterials[materialIndex];
				}
			}
			else
			{
				return mMesh->GetMaterial(tag);
			}

		}
		return nullptr;
	}

	unsigned EntityStaticMesh::NumMaterial() const
	{
		if (mMesh) return mMesh->NumMaterial();
		return 0;
	}

	void EntityStaticMeshRS::Fetch(unsigned flag)
	{
		EntityPrimitiveRS::Fetch(flag);
	}
#endif
};



