#pragma once

#include "Entity.h"

namespace UEngine
{

//////////////////////////////////////////////////////////////////////////
class UENGINE_API EntityNode : public Entity
{
	UCLASS(EntityNode, Entity)

	friend Entity;

	EntityNode();
	~EntityNode();

	virtual void					SetWorldTransform(const Transform&) override;
	virtual void					SetLocalTransform(const Transform&) override;

	virtual const Transform&		GetLocalTransform() const override;
	virtual const Transform&		GetWorldTransform() const override;
	virtual const Transform&		GetInvWorldTransform() const override;

	virtual void OnPostClone() override;
protected:

	virtual const Transform& UpdateSelfWorld(const Transform& world) override;

private:


	Transform				mWorldTransform;
	Transform				mLocalTransform;
	mutable Transform		mInvWorldTransform;
};


};