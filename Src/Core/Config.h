#pragma once

#include "Base.h"
#include "Array.h"
#include "String.h"

namespace UCore
{
	//////////////////////////////////////////////////////////////////////////
	class ClassInfo;
	

	UCORE_API void UConfigClassWrite(const ClassInfo* pClass, const void* pInstance, String& outConfigString);
	UCORE_API void UConfigClassRead(const ClassInfo* pClass, void* pInstance, const String& inConfigString);
};