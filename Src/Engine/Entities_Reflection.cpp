#include "../Core/Meta.h"

#include "Entity.h"
#include "EntityNode.h"
#include "EntityPrimitive.h"
#include "EntityStaticMesh.h"
#include "EntityShape.h"

namespace UEngine
{
	UCLASS_BEGIN_IMPL(Entity, AttrIcon("Entity.png"), AttrEditorSpawnable("Entity"))
		UPROPERTY(mName)
	UCLASS_END_IMPL(Entity)


	UCLASS_BEGIN_IMPL(EntityPrimitive)
	UCLASS_END_IMPL(EntityPrimitive)

	UCLASS_BEGIN_IMPL(EntityNode, AttrIcon("EntityNode.png"), AttrEditorSpawnable("Node"))
	UCLASS_END_IMPL(EntityNode)


	UCLASS_BEGIN_IMPL(EntityShape)
	UCLASS_END_IMPL(EntityShape)

	UCLASS_BEGIN_IMPL(EntityBox, AttrIcon("EntityBox.png"), AttrEditorSpawnable("Box", "Physics"))
		UPROPERTY(mExtent, AttrMinMax(0, 9999999))
	UCLASS_END_IMPL(EntityBox)

	UCLASS_BEGIN_IMPL(EntitySphere, AttrIcon("EntitySphere.png"), AttrEditorSpawnable("Shpere", "Physics"))
		UPROPERTY(mRadius, AttrMinMax(0, 999999))
	UCLASS_END_IMPL(EntitySphere)
};