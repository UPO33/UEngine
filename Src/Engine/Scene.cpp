#include "Scene.h"
#include "PrimitiveBatch.h"
#include "TaskMgr.h"
#include "SceneRS.h"
#include "EntityRoot.h"
#include "Prefab.h"
#include "Entity.h"
#include "EntityPrimitive.h"

namespace UEngine
{

UENGINE_API Scene* gScene = nullptr;

//////////////////////////////////////////////////////////////////////////
TArray<Scene*> Scene::ScenesArray;


//////////////////////////////////////////////////////////////////////////
Scene::Scene(const SceneCreationParam& creationParam)
{
	mSceneType = creationParam.mType;

	mPrimitiveBatch = new PrimitiveBatchScene(this);

	mRootEntity = NewObject<EntityRoot>();
	mRootEntity->mScene = this;
	mRootEntity->mRegistered = true;
	mRootEntity->mName = "Root";
	
	UEnqueueWait(EET_Render, [this]() {
		mRS = new SceneRS(this);
	});

	ScenesArray.Add(this);	
}
//////////////////////////////////////////////////////////////////////////
Scene::~Scene()
{
	UEnqueueWait(EET_Render, [this]() {
		SafeDelete(this->mRS);
	});

	UFlushTasks();

}
//////////////////////////////////////////////////////////////////////////
void Scene::Tick(float delta)
{
	TickEntities(delta);
	EndOfFrameTick();

	mPrimitiveBatch->mLines.Swap();
	mPrimitiveBatch->mLines.GetWrite().RemoveAll();
}
//////////////////////////////////////////////////////////////////////////
void Scene::TickEntities(float delta)
{
	this->GetRootEntity()->ForEachChild(true, true, [=](Object* obj) {
		if (auto ent = UCast<Entity>(obj))
		{
			ent->Tick(0, delta);
		}
	});
}

//////////////////////////////////////////////////////////////////////////
Entity* Scene::CreateEntity(const ClassInfo* entityClass, Name name, Entity* parent, const Transform& transform)
{
	if (entityClass)
	{
		UASSERT(entityClass->IsBaseOf<Entity>());
		if (parent) UASSERT(parent->mScene == this && parent->mRegistered);

		Entity* newEntity = NewObject<Entity>(entityClass);
		newEntity->SetName(name);
		newEntity->SetLocalTransform(transform);
		newEntity->RegisterEntityTo(parent ? parent : mRootEntity);
		return newEntity;
	}
	return nullptr;
}
//////////////////////////////////////////////////////////////////////////
Entity* Scene::CreateEntity(const APrefab* prefab, Name name, Entity* parent, const Transform& transform)
{
	if (prefab)
	{
		if(parent) UASSERT(parent->mScene == this && parent->mRegistered);

		Entity* pNewEntity = prefab->CloneEntity();
		pNewEntity->SetName(name);
		pNewEntity->SetLocalTransform(transform);
		pNewEntity->RegisterEntityTo(parent ? parent : mRootEntity);

		return pNewEntity;
	}
	return nullptr;
}


void Scene::EndOfFrameTick()
{
	//#TODO maybe I can make it concurrent,  seems it only copies row data
	for (EntityPrimitive* ent : mDirtyTransformEntities)
	{
		ent->mRenderStateTransformDirty = false;
		ent->mRenderState->UpdateTransform(ent);
	}
	mDirtyTransformEntities.RemoveAll();


	for (EntityPrimitive* ent : mDirtyRSEntities)
	{
		UEnqueue(EET_Render, [ent]() {
			ent->mRenderStateDirty = false;
			ent->mRenderState->DeInitilize();
			SafeDelete(ent->mRenderState);
			auto newRS = ent->CreateRS(); 
			newRS->Initilize();
			ent->mRenderState = newRS;
		});
	}
	mDirtyRSEntities.RemoveAll();
}

};