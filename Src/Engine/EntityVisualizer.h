#pragma once


#include "Base.h"

namespace UEngine
{
	class PrimitiveBatch;
	class Canvas;
	class Entity;

	class EntityVisualizer
	{
		virtual void Draw(Entity* entity, PrimitiveBatch* batch, Canvas* canvas)
		{

		}
	};
};