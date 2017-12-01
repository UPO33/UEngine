#pragma once

#include "Base.h"
#include "../Core/Object.h"
#include "../Core/ObjectPtr.h"
#include "../Core/Vector.h"
#include "../Core/Quat.h"
#include "../Core/Matrix.h"
#include "../Core/Bound.h"
#include "../Core/Meta.h"
#include "TaskMgr.h"

namespace UEngine
{
	//////////////////////////////////////////////////////////////////////////
	class Scene;
	class SceneRS;
	class Entity;
	class EntityNode;
	class EntityPrimitive;
	class EntityPrimitiveRS;
	class AMaterial;

	typedef uint64 EntityLayerT;
	typedef uint64 EntityFlagsT;

	enum EEntityFlag
	{
	};

	enum class EEndPlayCause
	{

	};

	
	struct AttrEditorSpawnable : AttrBase
	{
		const char* mName;
		const char* mCategory;

		AttrEditorSpawnable(const char* name, const char* category = nullptr) 
			: mName(name), mCategory(category)
		{}
	};

	//base class for anything that can be placed to a scene,  
	//Entity is hierarchical can have children and parent but doest have transform
	class UENGINE_API Entity : public Object
	{
		UCLASS(Entity, Object)

		friend EntityNode;
		friend Scene;
		friend SceneRS;
		friend EntityPrimitiveRS;

		Entity();
		~Entity();

		static EntityLayerT FindLayerByName(Name);

		Scene*							GetScene() const { return mScene; }
		Entity*							GetParentEntity() const;
		Entity*							GetRootEntity() const;
		bool							IsRootEntity() const;


		//attach this entity to a new parent , both entities must be registered and in the same scene
		virtual void AttachTo(Entity* newParent, bool keepLocalTransform = true);


		//u may loop the children of a entity using the following code:
		//Entity child = parentEntity->GetEntityFirstChild()
		//while(child)
		//{
		//	child = child->GetDownEntity();
		//}
		Entity* GetEntityFirstChild() const;
		Entity* GetUpEntity() const;
		Entity* GetDownEntity() const;

		size_t GetEntityChildCount() const;

		//Entity has not transform itself return the parent's transform instead
		virtual const Transform&		GetWorldTransform() const;
		virtual const Transform&		GetInvWorldTransform() const;
		virtual const Transform&		GetLocalTransform() const;

		Vec3							GetWorldPosition() const;
		Vec3							GetLocalPosition() const;
		Quat							GetWorldRotation() const;
		Quat							GetLocalRotation() const;
		

		//Entity has not transform itself so setting either local or world transform do nothing
		virtual void					SetWorldTransform(const Transform&){}
		virtual void					SetLocalTransform(const Transform&){}

		virtual void					SetWorldPosition(const Vec3&) {}
		virtual void					SetLocalPosition(const Vec3&) {}
		virtual void					SetWorldScale(const Vec3&) {}
		virtual void					SetLocalScale(const Vec3&) {}
		virtual void					SetWorldRotation(const Quat& rotation) {}
		virtual void					SetLocalRotation(const Quat& rotation) {}

		Name							GetName() const { return mName; }
		Name							GetTag() const { return mTag; }
		void							SetTag(Name newTag) { mTag = newTag; }
		void							SetName(Name newName) { mName = newName; }

		virtual void SetLayer(EntityLayerT newLayers)
		{
			mLayers = newLayers;
		}

		virtual void OnPostClone() override;


		//register this entity to @parent
		//@parent mus be a registered entity with a valid scene and this entity must be unregistered 
		//will all the registers children as well
		virtual void RegisterEntityTo(Entity* parent, bool bTop = false);
		virtual void RegisterEntityTo(Scene* pScene);

		virtual void UnregisterEntity();

		virtual void OnRegister();
		virtual void OnDeregister();

		virtual void OnBeginPlay();
		virtual void OnEndPlay();

		virtual void Tick(int pass, float delta) {}
		bool IsAlive() const { return mIsAlive; }
		bool IsPrefab() const { return mIsPrefab; }
	
		virtual void	SetHidden(bool hiddenInGame, bool hiddenInEditor, bool propagateToChildren);
		virtual bool	IsHiddenInGame() const;
		virtual bool	IsHiddenInEditor() const;
		
		//indicates whether entity is selected in editor or not
		bool IsSelected() const 
		{ 
			return mIsSelected == true;
		}
		//is called when user select or deselect the entity in editor
		virtual void SetSelected(bool select) 
		{
			mIsSelected = select;
		}
		virtual void DestroyEntity();

		inline bool FlagTest(unsigned test) const { return mEntityFlag & test == test; }
		inline bool FlagTestAndClear(unsigned test) 
		{
			if (mEntityFlag & test == test)
			{
				mEntityFlag &= ~test;
				return true;
			}
			mEntityFlag &= ~test;
			return false;
		}
		
	protected:
		virtual void OnTransformChanged(){}
		//is called before deregister
		virtual void OnDestroy(){}
		virtual void OnConstruct(){}

	private:
		void UpdateChildrendTransform(const Transform& worldTransform);
		virtual const Transform& UpdateSelfWorld(const Transform& world);
		void AddChildToList(Entity* child);
		void RemoveChildFromList(Entity* child);


		EntityFlagsT		mEntityFlag;
		//the layers that this entity belongs to, each bit implies a layer use test to check which layers this entity belongs to
		EntityLayerT		mLayers;
		//pointer to scene that this entity is registered to, maybe null
		Scene*				mScene;
		Name				mName;
		Name				mTag;

	public:
		size_t				mIsAlive : 1;
		size_t				mIsInvWorldTransformDirty : 1;
		size_t				mIsSelected : 1;
		size_t				mEditorIsExpanded : 1;
		size_t				mIsHover : 1;
		size_t				mBeginPlayWasCalled : 1;
		size_t				mEndPlayWasCalled : 1;
		size_t				mUseParentBound : 1;
		size_t				mUseParentLOD : 1;
		size_t				mRegistered : 1;
		size_t				mIsPrefab : 1;

		size_t				mRenderStateDirty : 1;
		size_t				mRenderStateTransformDirty : 1;
		size_t				mRenderStateMisDirty : 1;
		size_t				mRenderStateDynamicDirty : 1;

		size_t				mHiddenInGame : 1;
		size_t				mHiddenInEditor : 1;
		size_t				mCastShadow : 1;
		size_t				mRecieveShadow : 1;
	};



	
	typedef TObjectPtr<Entity> EntityPtr;
};