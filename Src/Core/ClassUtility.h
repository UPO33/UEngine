#pragma once

#include "Base.h"
#include "Array.h"

namespace UCore
{
	//////////////////////////////////////////////////////////////////////////
	class ClassInfo;
	class Object;


	//////////////////////////////////////////////////////////////////////////
	struct UCORE_API ClassUtility
	{
		//returns the lowest common ancestor of two classes if any
		static const ClassInfo* GetLCA(const ClassInfo* a, const ClassInfo* b);
		//returns the lowest common ancestor from a set of classes
		static const ClassInfo* GetLCA(const ClassInfo** classes, size_t numClasses);
		//like above but get the classes from objects
		static const ClassInfo* GetLCA(const Object** objects, size_t numObjects);

		template<typename TA> static auto GetLCA(const TArray<const ClassInfo*, TA>& classes)
		{
			return GetLCA(classes.Elements(), classes.Length());
		}
		template<typename TA> static auto GetLCA(const TArray<const Object*, TA>& classes)
		{
			return GetLCA(classes.Elements(), classes.Length());
		}
	};


};
