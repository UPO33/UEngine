#pragma once

#include "Asset.h"

namespace UEngine
{
//////////////////////////////////////////////////////////////////////////
class Entity;

//////////////////////////////////////////////////////////////////////////
class UENGINE_API APrefab : public Asset
{
	UCLASS(APrefab, Asset)


	Entity*	mRootEntity;

	//clones the entity hierarchy of this prefab and returns the root, the returned value is supposed to be added to a scene
	Entity* CloneEntity() const;

};


};
