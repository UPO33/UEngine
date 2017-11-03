#pragma once

#include "Base.h"
#include <windows.h>



namespace UCore
{
	class LockCriticalSection
	{
		CRITICAL_SECTION	mCriticleSection;

	public:
		LockCriticalSection()
		{
			::InitializeCriticalSection(&mCriticleSection);
		}
		~LockCriticalSection()
		{
			::DeleteCriticalSection(&mCriticleSection);
		}
		//If the critical section is successfully entered or the current thread already owns the critical section, 
		//the return value is nonzero. If another thread already owns the critical section, the return value is zero.
		bool TryEnter()
		{
			return ::TryEnterCriticalSection(&mCriticleSection) != 0;
		}
		void Enter()
		{
			::EnterCriticalSection(&mCriticleSection);
		}
		void Leave()
		{
			::LeaveCriticalSection(&mCriticleSection);
		}
	};


	//////////////////////////////////////////////////////////////////////////
	class Event
	{
		HANDLE	mHandle;
	public:
		/*
		@initSignaled
		indicates whether initial state is signaled or nonsignaled
		@manualRest
		If manualRest parameter is true, the function creates a manual - reset event object, which requires the use
		of the ResetEvent function to set the event state to nonsignaled. If this parameter is false, the function creates
		an auto-reset event object, and system automatically resets the event state to nonsignaled after a single waiting thread has been released.
		*/
		Event(bool initSignaled = false, bool manualRest = false)
		{
			mHandle = CreateEventA(nullptr, manualRest, initSignaled, nullptr);
			UASSERT(mHandle != nullptr);
		}
		~Event()
		{
			if (mHandle)
			{
				::CloseHandle(mHandle);
			}
		}
		// wait for event object to be signaled.
		unsigned Wait()
		{
			UASSERT(mHandle != nullptr);
			return ::WaitForSingleObject(mHandle, INFINITE);
		}
		//wait for event object to be signaled or out interval elapses.
		unsigned Wait(unsigned timeOutMiliseconds)
		{
			UASSERT(mHandle != nullptr);
			return ::WaitForSingleObject(mHandle, timeOutMiliseconds);
		}
		//Sets the event object to the nonsignaled state.
		//if is already nonsignaled has no effect.
		bool SetNonSignaled()
		{
			UASSERT(mHandle != nullptr);
			return ::ResetEvent(mHandle) != 0;
		}

		//Sets the event object to the signaled state.
		//Setting an event that is already set has no effect.
		//
		//manual-reset event remains signaled until it is set explicitly to the nonsignaled state by the SetNonSignaled function.
		//Any number of waiting threads, or threads that subsequently begin wait operations for the specified event 
		//object by calling one of the wait functions, can be released while the object's state is signaled.
		//
		//auto - reset event object remains signaled until a single waiting thread is released, at which time the system automatically
		//sets the state to nonsignaled.If no threads are waiting, the event object's state remains signaled.
		bool SetSignaled()
		{
			UASSERT(mHandle != nullptr);
			return ::SetEvent(mHandle) != 0;
		}
	};


	struct ZZZThreadExec
	{
		virtual void Exec() {};
	};

	static DWORD WINAPI ZZZThreadLambda(void* param)
	{
		ZZZThreadExec* ins = reinterpret_cast<ZZZThreadExec*>(param);
		ins->Exec();
		delete ins;
		return 0;
	}

	namespace Thread
	{
		UCORE_API void SetName(ThreadID threadID, const char* threadName);

		inline void Sleep(unsigned milliseconds) { ::Sleep(milliseconds); }
		inline ThreadID ID() { return (ThreadID)::GetCurrentThreadId(); }
		inline void Suspend(ThreadHandle thread)
		{
			//If the function succeeds, the return value is the thread's previous suspend count; otherwise, it is (DWORD) -1. 
			::SuspendThread((HANDLE)thread);
		}
		//Decrements suspend count. When the suspend count is decremented to zero, the execution of the thread is resumed.
		inline void Resume(ThreadHandle thread)
		{
			//If the function succeeds, the return value is the thread's previous suspend count.
			//If the function fails, the return value is(DWORD) - 1.
			::ResumeThread((HANDLE)thread);
		}
		inline bool Terminate(ThreadHandle thread, unsigned exitCode = 0)
		{
			if (::TerminateThread((HANDLE)thread, (DWORD)exitCode))
				return true;
			return false;
		}
		//create a new thread return null if failed
		template<typename Lambda> ThreadHandle CreateLambda(Lambda proc, bool runImmedialely = true)
		{
			struct Impl : ZZZThreadExec
			{
				Lambda mProc;
				Impl(Lambda& p) : mProc(p) {}
				void Exec() override { mProc(); }
			};

			DWORD id = 0;
			ThreadHandle handle = (ThreadHandle)CreateThread(
				nullptr,       // default security attributes
				0,          // default stack size
				(LPTHREAD_START_ROUTINE)ZZZThreadLambda,
				(void*)(new Impl(proc)),
				runImmedialely ? 0 : CREATE_SUSPENDED,          //creation flags
				&id);
			return handle;
		};

		//////////////////////////////////////////////////////////////////////////Thread local storage
		inline TLSIndex TLSAlloc()
		{
			return (TLSIndex)::TlsAlloc();
		}
		inline void TLSFree(TLSIndex index)
		{
			::TlsFree(index);
		}
		inline void TLSSetValue(TLSIndex index, void* value)
		{
			::TlsSetValue(index, value);
		}
		inline void* TLSGetValue(TLSIndex index)
		{
			return ::TlsGetValue(index);
		}

	};	
};