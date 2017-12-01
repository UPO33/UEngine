#include "Entity.h"
#include "Scene.h"
#include "SceneRS.h"

namespace UEngine
{
	

	Entity::Entity()
	{
		mHiddenInGame = false;
		mHiddenInEditor = false;
		mIsSelected = false;
		mIsInvWorldTransformDirty = false;
		
		mBeginPlayWasCalled = false;
		mIsAlive = true;
		mEndPlayWasCalled = false;
		mRegistered = false;

		{
			static unsigned EntCounter = 0;
			char buff[128];
			sprintf(buff, "Entity%d", EntCounter++);
			mName = buff;
		}
	}

	Entity::~Entity()
	{

	}

	Entity* Entity::GetParentEntity() const
	{
		return (Entity*)GetObjectParent();
	}

	Entity* Entity::GetRootEntity() const
	{
		return nullptr;
	}

	bool Entity::IsRootEntity() const
	{
		return GetRootEntity() == this;
	}


	void Entity::AttachTo(Entity* newParent, bool keepLocalTransform)
	{
		if (newParent == this) return;
			
		ObjectAattachTo(newParent);

		return;
	}

	

	Entity* Entity::GetUpEntity() const
	{
		//#Note entity may have non-entity object as child so we cast each object in the loop
		Object* iter = GetObjectUp();
		while (iter)
		{
			if (Entity* asEntity = iter->Cast<Entity>())
				return asEntity;
			else
				iter = iter->GetObjectUp();
		}
		return nullptr;
	}

	Entity* Entity::GetDownEntity() const
	{
		Object* iter = GetObjectDown();
		while (iter)
		{
			if (Entity* asEntity = iter->Cast<Entity>())
				return asEntity;
			else
				iter = iter->GetObjectDown();
		}
		return nullptr;
	}



	size_t Entity::GetEntityChildCount() const
	{
		size_t n = 0;

		Entity* iter = GetEntityFirstChild();
		while (iter)
		{
			n++;

			iter = iter->GetDownEntity();
		}

		return n;
	}

	Entity* Entity::GetEntityFirstChild() const
	{
		if (Object* head = GetObjectFirstChild())
		{
			if (Entity* asEntity = head->Cast<Entity>())
				return asEntity;

			Object* iter = head->GetObjectDown();
			while (iter)
			{
				if (Entity* asEntity = iter->Cast<Entity>())
					return asEntity;
				else
					iter = iter->GetObjectDown();
			}
		}
		return nullptr;
	}

	const Transform& Entity::GetWorldTransform() const
	{
// 		if (mParent) return mParent->GetWorldTransform();

		return Transform::IDENTITY;
	}

	const Transform& Entity::GetInvWorldTransform() const
	{
// 		if (mParent) return mParent->GetInvWorldTransform();

		return Transform::IDENTITY;
	}

	const Transform& Entity::GetLocalTransform() const
	{
// 		if (mParent) mParent->GetLocalTransform();

		return Transform::IDENTITY;
	}



	Vec3 Entity::GetWorldPosition() const
	{
		return GetWorldTransform().GetTranslation();
	}

	Vec3 Entity::GetLocalPosition() const
	{
		return GetLocalTransform().GetTranslation();
	}

	Quat Entity::GetWorldRotation() const
	{
		return GetWorldTransform().GetRotationQuat();
	}

	Quat Entity::GetLocalRotation() const
	{
		return GetLocalTransform().GetRotationQuat();
	}


	//////////////////////////////////////////////////////////////////////////
	void Entity::OnPostClone()
	{
		ParentT::OnPostClone();

		mScene = nullptr;
		mRegistered = false;

		this->mEndPlayWasCalled = false;
		this->mBeginPlayWasCalled = false;
		this->mIsAlive = false;
		this->mIsInvWorldTransformDirty = false;
		this->mIsPrefab = false;
		this->mIsSelected = false;
		this->mRenderStateDirty = false;
	}

	void Entity::RegisterEntityTo(Entity* parent, bool bTop)
	{
		UASSERT(parent);
		UASSERT(parent->mScene);

		UASSERT(mRegistered == false);
		UASSERT(mScene == nullptr && this->GetObjectParent() == nullptr);

		//check children are not registered either
		this->ForEachChild(true, true, [](Object* child) {
			if (auto childEntity = UCast<Entity>(child))
				UASSERT(childEntity->mScene == nullptr && childEntity->mRegistered == false);
		});

		ObjectAattachTo(parent, bTop);

		Entity* parentEntity = GetParentEntity();
		UASSERT(parentEntity && parentEntity->mScene);

		mScene = parentEntity->mScene;
		mRegistered = true;

		OnRegister();

		this->ForEachChild(true, true, [this](Object* child) {
			if (auto childEntity = UCast<Entity>(child))
			{
				childEntity->mRegistered = true;
				childEntity->mScene = this->mScene;
				childEntity->OnRegister();
			}
		});
	}

	void Entity::RegisterEntityTo(Scene* pScene)
	{
		RegisterEntityTo(pScene->GetRootEntity());
	}

	void Entity::UnregisterEntity()
	{
		if (mScene == nullptr) return; //has no scene , must not be a registered entity
		
		OnDeregister();
		ObjectDetach();
		mScene = nullptr;
		mRegistered = false;
		

		this->ForEachChild(true, true, [this](Object* child) {
			if (auto entityChild = UCast<Entity>(child))
			{
				entityChild->OnDeregister();
				entityChild->mScene = nullptr;
				entityChild->mRegistered = false;
			}
		});
	}

	void Entity::OnRegister()
	{
		ULOG_MESSAGE("");
	}

	void Entity::OnDeregister()
	{

	}

	void Entity::OnBeginPlay()
	{
		UASSERT(mBeginPlayWasCalled == false);
		
		mBeginPlayWasCalled = true;
	}

	void Entity::OnEndPlay()
	{
		UASSERT(mBeginPlayWasCalled);

		mEndPlayWasCalled = true;
	}

	void Entity::SetHidden(bool hiddenInGame, bool hiddenInEditor, bool propagateToChildren)
	{

	}

	bool Entity::IsHiddenInGame() const
	{
		return mHiddenInGame;
	}

	bool Entity::IsHiddenInEditor() const
	{
		return mHiddenInEditor;
	}
	void Entity::DestroyEntity()
	{	
		if (mScene == nullptr)
		{
			UASSERT(!mRegistered);

			this->AttachTo(nullptr);
			DeleteObject(this);
			return;
		}

		if (!mIsAlive)
		{
			ULOG_MESSAGE("Is deaed");
			return;
		}



		mIsAlive = false;
		//recursively call on destroy
		OnDestroy();

		UnregisterEntity();
	}

	void Entity::UpdateChildrendTransform(const Transform& worldTransform)
	{
// 		for (Entity* child : mChildren)
// 		{
// 			UASSERT(child);
// 			if (child->IsAlive())
// 			{
// 				child->UpdateChildrendTransform(child->UpdateSelfWorld(worldTransform));
// 			}
// 		}
	}

	const Transform& Entity::UpdateSelfWorld(const Transform& world)
	{
		return world;
	}

	void Entity::AddChildToList(Entity* child)
	{
// 		UASSERT(child->mParent == nullptr);
// 		UASSERT(!mChildren.HasElement(child));
// 
// 		mChildren.Add(child);
	}

	void Entity::RemoveChildFromList(Entity* child)
	{
// 		UASSERT(mChildren.HasElement(child));
// 		UASSERT(child->mParent == this);
// 
// 		mChildren.RemoveShift(mChildren.Find(child));
	}





};