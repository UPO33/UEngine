#pragma once

#include "Base.h"

namespace UCore
{
	class Rect
	{
		int	mLeft;
		int	mTop;
		int	mRight;
		int	mBottom;

		Rect(){}
		Rect(InitZero) mLeft(0), mRight(0), mTop(0), mBottom(0) {}
	};
};