#pragma once

#include "Base.h"
#include "../Core/Array.h"
#include "EntityCamera.h"
#include "CellCull.h"

namespace UEngine
{
	class Scene;
	class EntityPrimitiveRS;
	class EntityStaticMeshRS;

	static constexpr unsigned MAX_VIEW = 32;

	//////////////////////////////////////////////////////////////////////////
	struct SortedEntityList
	{
		struct Node
		{
			EntityPrimitiveRS* mEntity;
			Node* mNext;
			float mDistance;
		};

		Node*  mNodesPool = nullptr; //pool head
		size_t mPoolIter = 0;
		size_t mPoolSize = 0;	//maximum number of node we can alloc
		Node*  mHead = nullptr;
		size_t mNumEntity = 0;

		inline void* AllocNode()
		{
			return &mNodesPool[mPoolIter++];
		}
		SortedEntityList(size_t maxEntityCount)
		{
			mPoolSize = maxEntityCount + 2 /*for fake head and tail */;
			mNodesPool = new Node[mPoolSize];	//#TODO must allocates from a frame allocator

			//two fake nodes as minimum ans maximum that all elements will always be between 
			//heps to remove some redundant branch
			auto maxNode = new (AllocNode()) Node{ nullptr, nullptr, std::numeric_limits<float>::infinity() };
			auto minNode = new (AllocNode()) Node{ nullptr, maxNode, -std::numeric_limits<float>::infinity() };
			
			mHead = minNode;
		}
		//adds an entity to the list at the appropriate index
		void Add(EntityPrimitiveRS* ent, float dist)
		{
			auto pre = mHead;
			auto iter = mHead->mNext;
			while (iter)
			{
				if (dist < iter->mDistance)
				{
					auto newNode = new (AllocNode()) Node{ ent, iter, dist };
					pre->mNext = newNode;
					mNumEntity++;
					break;
				}

				pre = iter;
				iter = iter->mNext;
			}
		}
	};

	//////////////////////////////////////////////////////////////////////////
	struct ViewCulled
	{
		SceneView*				mView = nullptr;
		//visible entities in this view sorted by lower distance
		SortedEntityList*		mEntities = nullptr;
	};

	//////////////////////////////////////////////////////////////////////////
	class UENGINE_API SceneRS
	{
	public:
		
		struct EntPrimBaseData
		{
			unsigned		mLayers;
			unsigned		mFlags;
			CullingCell*	mCell;
			AABB			mBound;
		};

		Scene*	mGS;
		TArray<EntityPrimitiveRS*>		mEntities_RenderState;
		TArray<EntPrimBaseData>			mEntities_BaseData;
		//bits of an element indicate which views the entity is visible to
		//use mEntities_VisibilityMap[EntityIndex][1 << ViewIndex] to check the visibility
		TArray<unsigned>				mEntities_VisibilityMap;

		CullingScene*					mCulling;

		SceneRS(Scene* pScene) : mGS(pScene) {}

// 		TArray<EntPrimBaseData>		mEntities_PrimData;
// 
#if 0
		ViewCulled* DoViewsCull(SceneView* views, unsigned numView)
		{
			ViewCulled* outViews = new ViewCulled[numView];


			//#Optimize parallel for
			for(size_t iView = 0; iView < numView; iView++)
			{
				size_t viewBit = 1 << iView;
				SceneView& view = views[iView];
				for (size_t iEntity = 0; iEntity < mEntities_BaseData.Length(); iEntity++)
				{
					EntPrimBaseData& baseData = mEntities_BaseData[iEntity];
					if (baseData.mLayers & view.mRenderLayer)
					{
						bool isInside = false;
						if (baseData.mCell)
						{
							isInside = (baseData.mCell->mViewTest & viewBit) == viewBit;
						}
						else
						{
							isInside = view.mBoundWS.Intersects(baseData.mBound);
						}
						if (view.mFrustumWS.Intersects(baseData.mBound))
						{
							float dist = (baseData.mBound.GetCenter() - view.GetPosition()).LengthSquare();

							outViews[iView].mEntities->Add(mEntities_RenderState[iEntity], dist);

						}
					}
				}
			}

			return outViews;
		}
#endif // 
 
	};


};