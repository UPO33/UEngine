#pragma once

#include "Asset.h"
#include "../Core/Vector.h"
#include "../Core/Matrix.h"
#include "../Core/Bound.h"
#include "../Core/SmartMemory.h"

#include "PhysForward.h"

namespace UGFX
{
	struct GFXVertexBuffer;
	struct GFXIndexBuffer;
	struct GFXInputLayout;
	struct GFXResource;
	struct GFXBuffer;
};


namespace UEngine
{
	

	static const unsigned STATIC_MESH_MAX_LOD = 8;
	static const unsigned STATIC_MESH_MAX_MATERIAL = 32;
	static const unsigned STATIC_MESH_MAX_SELECTION = 16;


	//////////////////////////////////////////////////////////////////////////
	class AMaterialRS;
	class AMaterial;

	class AStaticMesh;
	class AStaticMeshRS;
	class StaticMeshLOD;
	class StaticMeshLODRS;
	



	//////////////////////////////////////////////////////////////////////////
	//a pair of a material and a tag to ease finding the material, (instead on dealing with index)
	struct UENGINE_API MaterialSlot
	{
		UCLASS(MaterialSlot)

		TObjectPtr<AMaterial>	mMaterial;
		Name					mSlot;
	};

	//////////////////////////////////////////////////////////////////////////
	struct UENGINE_API StaticMeshBaseVertex
	{
		UCLASS(StaticMeshBaseVertex)

		Vec3	mPosition;
		Vec3	mNormal;
		Vec3	mTangent;
		Vec2	mUV;

		bool IsNearlyEqual(const StaticMeshBaseVertex& other, float epsilon = FLOAT_EPSILON) const;
	};

	//////////////////////////////////////////////////////////////////////////
	struct UENGINE_API StaticMeshSectionInfo
	{
		UCLASS(StaticMeshSectionInfo)

		//the index of material
		unsigned						mMaterialIndex = 0;
		//offset from index buffer for this section in bytes
		unsigned						mIndexOffset = 0;
		//number of indices that this section draw
		unsigned						mNumIndices = 0;
	};

	//////////////////////////////////////////////////////////////////////////
	//the data of a single LOD in render side
	//we store all sections as a single buffer and use offset to draw the sections
	//each section is a mesh that has its own material but the buffers are used from its LOD
	class UENGINE_API StaticMeshLODRS
	{
	public:
		StaticMeshSectionInfo		mSelections[STATIC_MESH_MAX_SELECTION];
		unsigned					mNumSelections = 0;
		float						mDistanceBasedLODValue = 0;
		float						mSizeBasedLODValue = 0;

		//vertex color for this LOD, is any
		GFXBuffer*				mVertexColor = nullptr;
		GFXBuffer*				mVertexPositionNormalTangentUV = nullptr;
		//normal index buffer for drawing common vertex attributes
		GFXBuffer*				mIndexBuffer = nullptr;
		//index buffer for depth drawing only
		GFXBuffer*				mDepthOnlyIndexBuffer = nullptr;
		//vertex position for depth drawing only
		GFXBuffer*				mDepthOnlyVertexPosition = nullptr;
		
		
		void Init(StaticMeshLOD&);
	};

	//////////////////////////////////////////////////////////////////////////
	//contains the render side data of a static mesh
	class UENGINE_API AStaticMeshRS
	{
	public:
		using LODArrayT = TArray<StaticMeshLODRS, TArrayAllocStack<StaticMeshLODRS, STATIC_MESH_MAX_LOD>>;

		LODArrayT				mLODS;
		//the maximum LOD index this mesh is allowed to be rendered must be alway < NumLOD -1
		unsigned				mMaxLODIndex = 0;
		//the bounding box of the mesh that covers all the LODs
		BoundingBox				mBound;
		
		AStaticMeshRS(AStaticMesh*);
	};



	//////////////////////////////////////////////////////////////////////////
	//the data of a LOD in game side
	class UENGINE_API StaticMeshLOD
	{
		UCLASS(StaticMeshLOD)

		
		TArray<StaticMeshSectionInfo>	mSections;
		//will be empty when vertex buffers created
		TArray<StaticMeshBaseVertex>	mVerticesBase;
		//only one of these two must have value, will be empty when IndexBuffer created
		TArray<uint32>					mIndices32Bit;
		TArray<uint16>					mIndices16Bit;
		//whether the indices are uint16 or uint32
		bool							mHas16BitIndex = false;
		unsigned						mNumVertices = 0;
		unsigned						mNumIndices = 0;
		float							mDistanceBasedLODValue = 0;
		float							mSizeBasedLODValue = 0;
		BoundingBox						mBound;

		//returns number of vertices even if vertices were freed
		unsigned NumVertices() const
		{
			return mNumVertices;
		}
		//returns number of indices even if insides were freed
		unsigned NumIndices() const 
		{
			return mNumIndices;
		}
		//returns size of a index in bytes
		size_t IndexSize() const
		{
			return IsIndex16Bit() ? 2 : 4;
		}
		bool IsIndex16Bit() const
		{
			return mHas16BitIndex;
		}
		unsigned GetIndexAt(unsigned index) const
		{
			if (IsIndex16Bit())
				return (unsigned)mIndices16Bit[index];
			else
				return mIndices32Bit[index];
		}
		const void* GetIndicesRaw() const
		{
			if (IsIndex16Bit()) 
				return mIndices16Bit.Elements();
			return mIndices32Bit.Elements();
		}
		//calculates bounding box from vertices
		void CalcBound();
	};

	//////////////////////////////////////////////////////////////////////////
	class UENGINE_API StaticMeshPhysData
	{
		UCLASS(StaticMeshPhysData)

		physx::PxTriangleMesh*	mComplexMesh = nullptr;
	};

	//////////////////////////////////////////////////////////////////////////
	class UENGINE_API AStaticMesh : public Asset
	{
		UCLASS(AStaticMesh, Asset)


		unsigned	NumMaterial() const { return mMaterials.Length(); }
		unsigned	NumLOD() const { return mLODDs.Length(); }
		AMaterial*	GetMaterial(unsigned index);
		AMaterial*	GetMaterial(Name tag);
		AABB		GetBoundingBox() const { return AABB(mBound.mMin, mBound.mMax); }

		bool IsRenderValid() const;

		void CreateRS();
		virtual void OnPostPropertyChange(const PropertyChangedEvent&) override;
		virtual void OnPrePropertyChange(const PropertyChangedEvent&) override;

		void CalcBound(bool bRecalculateLODsBound = true);
	
		//render state data of this mesh
		AStaticMeshRS* mRS = nullptr;
		//there must be at least one LOD
		TArray<StaticMeshLOD>		mLODDs;
		//materials of this mesh, there must be at least one material
		TArray<MaterialSlot>		mMaterials;
		BoundingBox					mBound;
		//the maximized LOD index the mesh could be rendered at
		unsigned					mMaxLODIndex;
		unsigned					mCollisionLODIndex = 0;
		StaticMeshPhysData			mPhysData;

		AStaticMeshRS* GetRS() const { return mRS; }

		static AStaticMesh* ImportFromAssetFile(const TSPtr<SmartMemBlock>& file);
	};

	
	typedef TObjectPtr<AStaticMesh> AStaticMeshPtr;
};