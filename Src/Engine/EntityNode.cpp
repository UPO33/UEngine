#include "EntityNode.h"



namespace UEngine
{

	EntityNode::EntityNode()
	{
		mWorldTransform = mLocalTransform = mInvWorldTransform = Transform::IDENTITY;
	}

	EntityNode::~EntityNode()
	{

	}

	void EntityNode::SetWorldTransform(const Transform& wt)
	{
		// 		mWorldTransform = wt;
		// 		mIsInvWorldTransformDirty = true;
		// 		if (mParent)
		// 		{
		// 			mLocalTransform = mWorldTransform  * mParent->GetInvWorldTransform();
		// 		}
		// 		else
		// 		{
		// 			mLocalTransform = wt;
		// 		}
		// 		OnTransformChanged();
		// 		UpdateChildrendTransform(mWorldTransform);
	}

	void EntityNode::SetLocalTransform(const Transform& lt)
	{
		// 		mLocalTransform = lt;
		// 
		// 		if (mParent)
		// 		{
		// 			mWorldTransform = lt * mParent->GetWorldTransform();
		// 			mIsInvWorldTransformDirty = true;
		// 		}
		// 		else
		// 		{
		// 			mWorldTransform = mLocalTransform;
		// 			mIsInvWorldTransformDirty = true;
		// 		}
		// 		OnTransformChanged();
		// 		UpdateChildrendTransform(mWorldTransform);
	}

	const Transform& EntityNode::GetLocalTransform() const
	{
		return mLocalTransform;
	}

	const Transform& EntityNode::GetWorldTransform() const
	{
		return mWorldTransform;
	}

	const Transform& EntityNode::GetInvWorldTransform() const
	{
		if (mIsInvWorldTransformDirty)
		{
			mInvWorldTransform = mWorldTransform;
			mInvWorldTransform.InvertAffine();
		}
		return mInvWorldTransform;
	}

	void EntityNode::OnPostClone()
	{
		ParentT::OnPostClone();
	}

	const Transform& EntityNode::UpdateSelfWorld(const Transform& world)
	{
		mWorldTransform = mLocalTransform * world;
		mIsInvWorldTransformDirty = true;
		OnTransformChanged();
		return mWorldTransform;
	}
};

