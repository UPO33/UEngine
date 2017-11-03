#pragma once

#include "Base.h"

#include <atomic>
#include <mutex>

namespace UCore
{
	typedef unsigned ThreadID;
	typedef unsigned TLSIndex;

	typedef void* ThreadHandle;
};



#define USCOPE_LOCK(lock)	UCore::TScopedLock<decltype(lock)>	ZZ_Lock(lock)


namespace UCore
{
	//////////////////////////////////////////////////////////////////////////
	template<typename TLock> class TScopedLock
	{
		TLock& mLock;
	public:
		TScopedLock(TLock& lock) : mLock(lock) { mLock.Enter(); }
		~TScopedLock() { mLock.Leave(); }
	};

	//null lock that do nothing
	class NullLock
	{
	public:
		void Enter()
		{}
		void Leave()
		{}
	};

	// efficient if threads are likely to be blocked for only short periods
	class SpinLock2
	{
	public:
		SpinLock2() : mLock(false) {}

		void Enter()
		{
			while(mLock.exchange(1, std::memory_order_acquire)){}
		}
		void Leave()
		{
			mLock.store(0, std::memory_order_release);
		}
	private:
		std::atomic<unsigned>	mLock;
		
	};

	// efficient if threads are likely to be blocked for only short periods
	class SpinLock
	{
	public:
		void Enter()
		{
			while (locked.test_and_set(std::memory_order_acquire)) { }
		}
		void Leave()
		{
			locked.clear(std::memory_order_release);
		}
	private:
		std::atomic_flag locked = ATOMIC_FLAG_INIT;
	};


	class StdLock
	{
	public:
		void Enter() { mLock.lock(); }
		void Leave() { mLock.unlock(); }
	private:
		std::mutex mLock;
	};
};

#ifdef UPLATFORM_WIN
#include "Threading_Win.h"
namespace UCore
{
	typedef LockCriticalSection Lock;
};

#else
#include "Threading_PThread.h"

namespace UCore
{
	typedef StdLock Lock;
};

#endif