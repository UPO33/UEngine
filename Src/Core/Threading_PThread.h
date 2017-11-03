#pragma once

#include <pthread.h>
#include <unistd.h>

#pragma once

namespace UCore
{
	namespace Thread
	{
		inline void SetName(ThreadID threadID, const char* threadName)
		{
			//#Note threadName must be less that 16 character 
			int error = pthread_setname_np((pthread_t)threadID, threadName) == 0;
			UASSERT(error);
		}

		inline void Sleep(unsigned milliseconds) { 
			::usleep(milliseconds); 
		}
		inline ThreadID ID() { 
			return (ThreadID)::pthread_self(); 
		}
		inline void Suspend(ThreadHandle thread)
		{
			//If the function succeeds, the return value is the thread's previous suspend count; otherwise, it is (DWORD) -1. 
			UASSERT(false);
		}
		//Decrements suspend count. When the suspend count is decremented to zero, the execution of the thread is resumed.
		inline void Resume(ThreadHandle thread)
		{
			//If the function succeeds, the return value is the thread's previous suspend count.
			//If the function fails, the return value is(DWORD) - 1.
			UASSERT(false);
		}
		inline bool Terminate(ThreadHandle thread, unsigned exitCode = 0)
		{
			UASSERT(false);
			return false;
		}
		//create a new thread return null if failed
		template<typename Lambda> ThreadHandle CreateLambda(Lambda proc)
		{
			struct ZZZThreadExec
			{
				virtual void Exec() {};
				virtual ~ZZZThreadExec() {}

				static void* ThreadProc(void* param)
				{
					ZZZThreadExec* ins = reinterpret_cast<ZZZThreadExec*>(param);
					ins->Exec();
					delete ins;
					return 0;
				}
			};
			struct Impl : ZZZThreadExec
			{
				Lambda mProc;
				Impl(Lambda& p) : mProc(p) {}
				void Exec() override { mProc(); }
			};

			pthread_t tid = 0;
			int err = pthread_create(&tid, nullptr, &ZZZThreadExec::ThreadProc, new Impl(proc));
			UASSERT(err == 0);
			return (ThreadHandle)tid;
		};

		//////////////////////////////////////////////////////////////////////////Thread local storage
		inline TLSIndex TLSAlloc()
		{
			pthread_key_t key = 0;
			UASSERT(pthread_key_create(&key, nullptr) == 0);
			return key;
		}
		inline void TLSFree(TLSIndex index)
		{
			pthread_key_delete(index);
		}
		inline void TLSSetValue(TLSIndex index, void* value)
		{
			pthread_setspecific(index, value);
		}
		inline void* TLSGetValue(TLSIndex index)
		{
			return pthread_getspecific(index);
		}

	};
};