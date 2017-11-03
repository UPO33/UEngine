#include "Timer.h"
#include <time.h>

namespace UCore
{
	UCORE_API unsigned GetSecondsSince2000()
	{
		time_t now;
		time(&now);
		tm tm2000 = { 0,0,0,0,0, 100, 0,0,0 };
		return (unsigned)difftime(now, mktime(&tm2000));
	}
};