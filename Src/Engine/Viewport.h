#pragma once

#include "Base.h"
#include "../Core/Vector.h"

namespace UEngine
{
//////////////////////////////////////////////////////////////////////////
class Canvas;

//////////////////////////////////////////////////////////////////////////
class UENGINE_API ViewportBase
{
public:
	//an array of all the Viewports
	TArray<ViewportBase*>	ViewportsArray;

	virtual ~ViewportBase() {}

	virtual void*		GetRenderTarget() = 0;
	virtual void*		GetWindowHandle() = 0;
	virtual void*		GetSwapChain() = 0;
	virtual Canvas*		GetCanvas() = 0;
	virtual Vec2I		GetSize() = 0;
};


//////////////////////////////////////////////////////////////////////////
class GameViewport : public ViewportBase
{

};

};
