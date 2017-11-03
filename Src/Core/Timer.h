#pragma once

#include "Base.h"
#include <chrono>

#ifdef UPLATFORM_WIN
#include <windows.h>
#endif

namespace UCore
{
#ifdef UPLATFORM_WIN
	//get elapsed time since program start in milliseconds
	inline unsigned AppTickCount() { return ::GetTickCount(); }

	//////////////////////////////////////////////////////////////////////////
	struct Chronometer
	{
		unsigned mStart = 0;

		void Start()
		{
			mStart = AppTickCount();
		}
		//get elapsed time in milliseconds
		unsigned Since() const
		{
			return AppTickCount() - mStart;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	struct ChronometerAccurate
	{
		LARGE_INTEGER mFreq, mStart;

	public:
		void Start()
		{
			QueryPerformanceFrequency(&mFreq);
			QueryPerformanceCounter(&mStart);
		}
		double SinceSeconds()
		{
			LARGE_INTEGER end;
			QueryPerformanceCounter(&end);
			return ((end.QuadPart - mStart.QuadPart) / (double)mFreq.QuadPart);
		}
		double SinceMiliseconds()
		{
			LARGE_INTEGER end;
			QueryPerformanceCounter(&end);
			return ((end.QuadPart - mStart.QuadPart) * 1000.0 / (double)mFreq.QuadPart);
		}
		unsigned long long SinceMicroseconds()
		{
			LARGE_INTEGER end;
			QueryPerformanceCounter(&end);
			return (end.QuadPart - mStart.QuadPart) * 1000000 / (mFreq.QuadPart);
		}
	};

#else

#endif


	class ChronometerHR
	{
	public:
		ChronometerHR() : mBegin(ClockT::now()) {}

		void Reset() { mBegin = ClockT::now(); }

		double ElapsedSeconds() const
		{
			return std::chrono::duration_cast<SecondT>(ClockT::now() - mBegin).count();
		}
		double ElapsedMilliSeconds() const
		{
			return std::chrono::duration_cast<MillisecondsT>(ClockT::now() - mBegin).count();
		}

	private:
		typedef std::chrono::steady_clock ClockT;
		typedef std::chrono::duration<double> SecondT;
		typedef std::chrono::duration<double, std::milli> MillisecondsT;

		std::chrono::time_point<ClockT> mBegin;
	};


	UCORE_API unsigned GetSecondsSince2000();

};