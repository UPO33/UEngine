
#include "ClassUtility.h"
#include "Meta.h"

namespace UCore
{
	//////////////////////////////////////////////////////////////////////////
	const ClassInfo* ClassUtility::GetLCA(const ClassInfo* a, const ClassInfo* b)
	{
		TArray<const ClassInfo*, TArrayAllocStack<const ClassInfo*, 128>> traversed;

		auto pa = a;
		while (pa)
		{
			traversed.Add(pa);

			pa = pa->GetParentClass();
		}

		auto pb = b;
		while (pb)
		{
			if (traversed.HasElement(pb))
				return pb;

			pb = pb->GetParentClass();
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	const ClassInfo* ClassUtility::GetLCA(const ClassInfo** classes, size_t numClasses)
	{
		if (numClasses == 0) return nullptr;
		if (numClasses == 1) return classes[0];
		if (numClasses == 2) return GetLCA(classes[0], classes[1]);

		const ClassInfo* lca = GetLCA(classes[0], classes[1]);

		for (size_t i = 2; i < numClasses; i++)
		{
			lca = GetLCA(lca, classes[i]);
		}

		return lca;
	}
	//////////////////////////////////////////////////////////////////////////
	const ClassInfo* ClassUtility::GetLCA(const Object** objects, size_t numObjects)
	{
#define GetObjClass(i) (objects[i] ? objects[i]->GetClass() : nullptr)

		if (numObjects == 0) return nullptr;
		if (numObjects == 1) return GetObjClass(0);
		if (numObjects == 2) return GetLCA(GetObjClass(0), GetObjClass(1));

		const ClassInfo* lca = GetLCA(GetObjClass(0), GetObjClass(1));

		for (size_t i = 2; i < numObjects; i++)
		{
			lca = GetLCA(lca, GetObjClass(i));
		}

		return lca;

#undef GetObjClass
	}

};