#pragma once

#include "Base.h"
#include "../Core/Vector.h"

namespace UEngine
{
	struct HitetionBase
	{
	public:

		template<typename T> const T* Cast() const { return dynamic_cast<const T*>(this); }
		template<typename T> T* Cast() { return dynamic_cast<T*>(this); }

		static void* operator new(size_t size);
		static void operator delete(void* ptr);

		virtual const char* GetDebugStr() const { return nullptr; }
		virtual int GetPriority() const { return 0; }
	};

	template<typename T> const T* Cast(const HitetionBase* in)
	{
		if (in) return in->Cast<T>();
		return nullptr;
	}
	template<typename T> T* Cast(HitetionBase* in)
	{
		if (in) return in->Cast<T>();
		return nullptr;
	}

	class HitetionCanvas
	{
		HitetionBase*	GetAt(unsigned x, unsigned y);
		void			GetRect(Vec2I pos, Vec2I size, TArray<HitetionBase*>& out);

		Vec2I			mSize;
		HitetionBase*	mItems;
	};

	class PixelIDSys
	{

	};
};