#pragma once

#include "Base.h"
#include "Threading.h"
#include <atomic>

namespace UCore
{
	//////////////////////////////////////////////////////////////////////////
	template<typename T> struct TRWBuffer
	{
		std::atomic<unsigned>	mIndex = 0;

		T mInstances[3];

		void Swap() { mIndex++; }
		T& GetRead() { return mInstances[mIndex.load(std::memory_order_acquire) % 3]; }
		T& GetWrite() { return mInstances[(mIndex.load(std::memory_order_acquire) + 1) % 3]; }
	};

	//////////////////////////////////////////////////////////////////////////
	template<typename T, unsigned Capacity> class TCircularQueueSQSP
	{
		static_assert(Capacity > 2, "Minimum acceptable Capacity is 2");

	public:
		typedef T ElementType;

		bool IsEmpty() const 
		{ 
			return mRead.load() == mWrite.load();
		}
		bool IsFull() const
		{
			return ((mWrite.load() + 1) % Capacity) == mRead.load();
		}
		static unsigned GetCapacity() 
		{
			return Capacity; 
		}
		
		template<typename... TArgs> T* Push(TArgs... args)
		{
			unsigned curWrite = mWrite.load(std::memory_order_relaxed);
			unsigned nextWrite = (curWrite + 1) % Capacity;
			if (nextWrite == mRead.load(std::memory_order_acquire)) return nullptr; //null if full

			T* ptr = (T*)(mElements + curWrite * sizeof(T));

			new ((void*)ptr) T(args...);

			mWrite.store(nextWrite, std::memory_order_release);
			return ptr;
		}
		bool Pop(T& out)
		{
			unsigned curRead = mRead.load(std::memory_order_relaxed);
			if (curRead == mWrite.load(std::memory_order_acquire)) return false;	//return false if empty

			T* ptr = (T*)(mElements + curRead * sizeof(T));
			out = *ptr;
			ptr->~T();
			mRead.store((curRead + 1) % Capacity, std::memory_order_release);
			return true;
		}
		//returns pointer to new pushing element null if queue is full
		T* BeginPush()
		{
			unsigned curWrite = mWrite.load(std::memory_order_relaxed);
			unsigned nextWrite = (curWrite + 1) % Capacity;
			if (nextWrite == mRead.load(std::memory_order_acquire)) return nullptr; //null if full

			T* ptr = (T*)(mElements + curWrite * sizeof(T));

			return ptr;
		}
		//this must be called if BeginPush() didn't return null
		void EndPush()
		{
			mWrite.store((mWrite.load(std::memory_order_relaxed) + 1) % Capacity, std::memory_order_release);
		}
		//returns pointer to the oldest element null if queue is empty
		T* BeginPop()
		{
			unsigned curRead = mRead.load(std::memory_order_relaxed);
			if (curRead == mWrite.load(std::memory_order_acquire)) return nullptr;	//return null if empty

			T* ptr = (T*)(mElements + curRead * sizeof(T));
			return ptr;
		}
		//this must be called if BeginPop() didn't return null
		void EndPop()
		{
			mRead.store((mRead.load(std::memory_order_relaxed) + 1) % Capacity, std::memory_order_release);
		}

		/*
		only producer writes to mWrite and only consumer writes to mRead
		*/

		std::atomic<unsigned>		mRead = 0;
		std::atomic<unsigned>		mWrite = 0;
		alignas(alignof(T)) char	mElements[sizeof(T[Capacity])];
	};



	template<unsigned MaxCommand = 1024, unsigned MaxLambdaSize = sizeof(void*) * 8> class TCommandQueueSPSC
	{
		//the base class for commands the virtual destructor invokes first the commend and then destroy the lambda
		struct alignas(16) ICMD
		{
			virtual ~ICMD() {}
		};
		struct alignas(16) CMDBase : ICMD
		{
			char mBuffer[MaxLambdaSize];
		};
		static_assert(sizeof(CMDBase) % 16 == 0, "should be 16 aligned");
		static const unsigned CMDSize = sizeof(CMDBase);

		using QueueT = TCircularQueueSQSP<CMDBase, MaxCommand>;

		QueueT			mQueue;
		Event			mWaitEvent;
		ThreadID		mProducerThread = 0;
		ThreadID		mConsumerThread = 0;
		bool			mRuning = false;

	public:

		bool IsEmpty() const { mQueue.IsEmpty(); }
		bool IsFull() const { mQueue.IsFull(); }


		TCommandQueueSPSC(const TCommandQueueSPSC&) = delete;
		TCommandQueueSPSC& operator = (const TCommandQueueSPSC&) = delete;

		TCommandQueueSPSC() : mWaitEvent(false, false) {}

		template<typename Lambda> void Enqueue(const Lambda& proc)
		{
			UASSERT(IsRuning());
			UASSERT(Thread::ID() == mProducerThread);

			struct NewCMD : public ICMD
			{
				Lambda mProc;
				NewCMD(const Lambda& p) : mProc(p) {}
				~NewCMD()
				{
					mProc();
				}
			};
			static_assert(sizeof(NewCMD) <= CMDSize, "command size is too big");
			static_assert(alignof(NewCMD) <= alignof(ICMD), "");

			void* pCmd = mQueue.BeginPush();
			UASSERT(pCmd, "queue is full");
			new (pCmd) NewCMD(proc);
			mQueue.EndPush();
		}
		template<typename Lambda> void EnqueueAndWait(const Lambda& proc)
		{
			UASSERT(IsRuning());
			UASSERT(Thread::ID() == mProducerThread);

			struct NewCMD : public ICMD
			{
				Lambda mProc;
				TCommandQueueSPSC*	mOwner;
				NewCMD(const Lambda& p, TCommandQueueSPSC* owner) : mProc(p), mOwner(owner) {}
				~NewCMD()
				{
					mProc();
					mOwner->mWaitEvent.SetSignaled();
				}
			};

			static_assert(sizeof(NewCMD) <= CMDSize, "commands size is too big");
			static_assert(alignof(NewCMD) <= alignof(ICMD), "");

			void* pCmd = mQueue.BeginPush();
			UASSERT(pCmd, "queue is full");
			new (pCmd) NewCMD(proc, this);
			mQueue.EndPush();

			mWaitEvent.Wait();
		}
		//producer must call this once
		void Run()
		{
			UASSERT(mRuning == false);

			mRuning = true;
			mProducerThread = Thread::ID();

			Thread::CreateLambda([this]() {

				mConsumerThread = Thread::ID();

				while (1)
				{
					if (ICMD* cmd = mQueue.BeginPop())
					{
						cmd->~ICMD();
						mQueue.EndPop();
					}
					else
					{
						if (!mRuning)
						{
							mProducerThread = 0;
							mConsumerThread = 0;
							return;
						}

						_mm_pause();
					}
				}
			});

			
		}
		//will ends the loop when no command is remaining
		void RequestExit() { mRuning = false; }
		bool IsRuning() const { return mRuning; }
	};



	template<unsigned NumConsumer, unsigned MaxCommand = 1024, unsigned MaxLambdaSize = sizeof(void*) * 8> class TCommandQueueSPNC
	{
		//the base class for commands the virtual destructor invokes first the commend and then destroy the lambda
		struct alignas(16) ICMD
		{
			virtual ~ICMD() {}
		};
		struct alignas(16) CMDBase : ICMD
		{
			char mBuffer[MaxLambdaSize];
		};
		static_assert(sizeof(CMDBase) % 16 == 0, "should be 16 aligned");
		static const unsigned CMDSize = sizeof(CMDBase);

		using QueueT = TCircularQueueSQSP<CMDBase, MaxCommand>;

		QueueT*			mQueues[NumConsumer];
		Event			mWaitEvent;
		ThreadID		mProducerThread = 0;
		bool			mRuning = false;

	public:

		TCommandQueueSPNC(const TCommandQueueSPNC&) = delete;
		TCommandQueueSPNC& operator = (const TCommandQueueSPNC&) = delete;

		TCommandQueueSPNC() : mWaitEvent(false, false) 
		{
			for (QueueT*& queue : mQueues)
				queue = new QueueT();
		}
		~TCommandQueueSPNC()
		{
			UASSERT(!IsRuning());
			for (QueueT*& queue : mQueues)
			{
				delete queue;
				queue = nullptr;
			}
		}

		template<typename Lambda> void Enqueue(unsigned consumerIndex, const Lambda& proc)
		{
			UASSERT(IsRuning());
			UASSERT(Thread::ID() == mProducerThread);

			struct NewCMD : public ICMD
			{
				Lambda mProc;
				NewCMD(const Lambda& p) : mProc(p) {}
				~NewCMD()
				{
					mProc();
				}
			};
			static_assert(sizeof(NewCMD) <= CMDSize, "command size is too big");
			static_assert(alignof(NewCMD) <= alignof(ICMD), "");

			void* pCmd = mQueues[consumerIndex]->BeginPush();
			UASSERT(pCmd, "queue is full");
			new (pCmd) NewCMD(proc);
			mQueues[consumerIndex]->EndPush();
		}
		template<typename Lambda> void EnqueueAndWait(unsigned consumerIndex, const Lambda& proc)
		{
			UASSERT(IsRuning());
			UASSERT(Thread::ID() == mProducerThread);

			struct NewCMD : public ICMD
			{
				Lambda mProc;
				TCommandQueueSPNC*	mOwner;
				NewCMD(const Lambda& p, TCommandQueueSPNC* owner) : mProc(p), mOwner(owner) {}
				~NewCMD()
				{
					mProc();
					mOwner->mWaitEvent.SetSignaled();
				}
			};

			static_assert(sizeof(NewCMD) <= CMDSize, "commands size is too big");
			static_assert(alignof(NewCMD) <= alignof(ICMD), "");

			void* pCmd = mQueues[consumerIndex]->BeginPush();
			UASSERT(pCmd, "queue is full");
			new (pCmd) NewCMD(proc, this);
			mQueues[consumerIndex]->EndPush();

			mWaitEvent.Wait();
		}
		
		void Run()
		{
			UASSERT(mRuning == false);

			mProducerThread = Thread::ID();

			for (unsigned threadIndex = 0; threadIndex < NumConsumer; threadIndex++)
			{
				Thread::CreateLambda([this, threadIndex]() {
					while (true)
					{
						if (ICMD* cmd = mQueues[threadIndex]->BeginPop())
						{
							cmd->~ICMD();
							mQueues[threadIndex]->EndPop();
						}
						else
						{
							if (!mRuning)
								return;
							_mm_pause();
						}
					}
				});
			}

			mRuning = true;
		}
		//will ends the loop when no command is remaining
		void RequestExit() { mRuning = false; }
		bool IsRuning() const { return mRuning; }
	};

};