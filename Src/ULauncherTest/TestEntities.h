#pragma once

#include "Base.h"
#include "../Engine/TaskMgr.h"
#include "../Engine/Scene.h"
#include "../Engine/EntityPrimitive.h"
#include "../Engine/PrimitiveBatch.h"
#include "GameInput.h"

class EntityTest0 : public EntityPrimitive
{
	UCLASS(EntityTest0, EntityPrimitive)

	virtual void Tick(int pass, float delta) override
	{
		ParentT::Tick(pass, delta);

		{

		}
		if (Scene* pScene = GetScene())
		{
			if (PrimitiveBatch* primBatch = pScene->GetPrimitiveBatch())
			{
				primBatch->DrawLine(Vec3(0.0), Vec3(100, 0, 0), Color32::RED);
				primBatch->DrawLine(Vec3(0.0), Vec3(0, 100, 0), Color32::GREEN);
				primBatch->DrawLine(Vec3(0.0), Vec3(0, 0, 100), Color32::BLUE);
				

				primBatch->DrawWireBox(AABB(Vec3(-64.0f), Vec3(64.0f)), Color32::YELLOW);
			}
			
		}
	}
};