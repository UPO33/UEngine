#include "Prefab.h"
#include "Entity.h"


namespace UEngine
{
	UCLASS_BEGIN_IMPL(APrefab)
	UCLASS_END_IMPL(APrefab)
};


namespace UEngine
{



//////////////////////////////////////////////////////////////////////////
Entity* APrefab::CloneEntity() const
{
	if (mRootEntity)
		return (Entity*)mRootEntity->Clone();
	return nullptr;
}

};

