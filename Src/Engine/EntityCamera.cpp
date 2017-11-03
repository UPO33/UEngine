#include "EntityCamera.h"
#include "Scene.h"

namespace UEngine
{

//////////////////////////////////////////////////////////////////////////
void EntityCamera::OnRegister()
{
	ParentT::OnRegister();

	GetScene()->AddCameraEntity(this);
}
///////////////////////////////////////////////////////////////////////////
void EntityCamera::OnDeregister()
{
	GetScene()->RemoveCameraEntity(this);

	ParentT::UnregisterEntity();
}

};