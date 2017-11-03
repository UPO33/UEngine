#pragma once

#include "Base.h"
#include "Entity.h"

namespace UEngine
{
	struct CullingCell
	{
		//bit mask that indicates which views are in this cell
		uint32		mViewTest;
	};
	struct int3
	{
		int x, y, z;
	};
	inline int3 operator + (const int3& a, const int3& b)
	{
		return int3{ a.x + b.x,  a.y + b.y,  a.z + b.z };
	}
	
	class CullingScene
	{
		int3						mCellSize;	//cell size in x, y, z axis
		int3						mCellCount;	//number of cell in x, y, z axis
		CullingCell***				mCells;	//access is [z][y][x]

		void AllocateCells(int3 size)
		{
			mCellCount = size;

			mCells = new CullingCell**[size.z];

			for (int z = 0; z < size.z; z++)
			{
				mCells[z] = new CullingCell*[size.y];
				for (int y = 0; y < size.y; y++)
				{
					mCells[z][y] = new CullingCell[size.x];
				}
			}
		}
		void DeleteCells()
		{
			for (int z = 0; z < mCellCount.z; z++)
			{
				for (int y = 0; y < mCellCount.y; y++)
				{
					delete[] mCells[z][y];
				}
				delete[] mCells[z];
			}
			delete[] mCells;

			mCellCount = { 0,0,0 };
		}
		void ZeroCells()
		{
			for (int z = 0; z < mCellCount.z; z++)
			{
				for (int y = 0; y < mCellCount.y; y++)
				{
					MemZero(mCells[z][y], sizeof(CullingCell) * mCellCount.x);
				}
			}
		}
// 		int CalCellIndex(int x, int y, int z) const
// 		{
// 			//x + width * (y + depth * z)
// 			return x + mCellSize.x * (y + mCellSize.z * z);
// 		}
		CullingCell* GetCell(int z, int y, int x) const
		{
			return &(mCells[z][y][x]);
			//return mCells + CalCellIndex(x, y, z);
		}

		Scene*						mScene;
		unsigned					mCellIDCounter;

// 		void SliceCell(AABB bound, TArray<CullingScene*>& outCells)
// 		{
// 			for (EntityPrimitiveRS* prs : mSceneRS->mPrimitiveEntities)
// 			{
// 				if (prs->mCullingCell->mCellID == currentCellID)
// 				{
// 
// 				}
// 			}
// 		}
		void CellsOr(int3 index, int3 size, unsigned value)
		{
			for (int z = index.z; z < index.z + size.z; z++)
			{
				for (int y = index.y; y < index.y + size.y; y++)
				{
					for (int x = index.x; x < index.x + size.x; x++)
					{
						mCells[z][y][x].mViewTest |= value;
					}
				}
			}
		}
		void ExtractEntitiesOfCells()
		{

		}
		void Perform(const TArray<AABB>& viewsBound)
		{
			this->ZeroCells();
			
		}
	};
};