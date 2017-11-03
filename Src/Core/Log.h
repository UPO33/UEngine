#pragma once

#include "Base.h"

namespace UCore
{
	struct LogIndent
	{
		int mValue;
		explicit LogIndent(int indent = 0) : mValue(indent) {}
	};
	UCORE_API StringStreamOut& operator << (StringStreamOut&, LogIndent);
};