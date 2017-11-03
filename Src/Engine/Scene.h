#pragma once

#include "Base.h"
#include "../Core/Array.h"
#include "../Core/Object.h"
#include "../Core/Matrix.h"

namespace UEngine 
{


//////////////////////////////////////////////////////////////////////////
class Scene;
class SceneRS;
class Entity;
class EntityCamera;
class APrefab;
class ViewportBase;
class PhysScene;
class PrimitiveBatch;
class EntityPrimitive;
class EntityPrimitiveRS;


enum class ESceneType
{
	Game,
	Editor,
	PlayInEditor,
	Preview,
};


struct SceneCreationParam
{
	ESceneType mType = ESceneType::Editor;
};

//////////////////////////////////////////////////////////////////////////
class UENGINE_API Scene
{
public:
	//list of all created scenes
	static TArray<Scene*> ScenesArray;

	friend Entity;
	friend EntityCamera;
	friend EntityPrimitive;
	friend EntityPrimitiveRS;

	Scene(const SceneCreationParam& scp);
	~Scene();

	PhysScene* GetPhysScene() const { return mPhysScene; }
	PrimitiveBatch* GetPrimitiveBatch() const { return mPrimitiveBatch; }
	Entity* GetRootEntity() const { return mRootEntity; }
	SceneRS* GetRS() const { return mRS; }

	void Tick(float delta);
	void TickEntities(float delta);

	bool IsEditorScene() const { return mSceneType == ESceneType::Editor; }
	



	Entity* CreateEntity(const ClassInfo* entityClass, Name name, Entity* parent, const Transform& transform);
	Entity* CreateEntity(const APrefab* prefab, Name name, Entity* parent, const Transform& transform);


private:
	void EndOfFrameTick();
	void AddCameraEntity(EntityCamera*) {}
	void RemoveCameraEntity(EntityCamera*) {}

	SceneRS*				mRS = nullptr;
	Entity*					mRootEntity = nullptr;
	TArray<EntityCamera*>	mCamerasEntity;
	TArray<ViewportBase*>	mViewports;
	ESceneType				mSceneType;
	PhysScene*				mPhysScene = nullptr;
	PrimitiveBatch*			mPrimitiveBatch = nullptr;

	TArray<EntityPrimitive*> mDirtyRSEntities;
	TArray<EntityPrimitive*> mDirtyTransformEntities;

	TArray<Entity*>		mPendingKillEntities;

};


extern UENGINE_API Scene* gScene;




};
