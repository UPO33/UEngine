#include "TaskMgr.h"

namespace UEngine
{
	thread_local EEngineThread gThreadID;

	UENGINE_API bool UIsGameThread()
	{
		return Thread::TLSGetValue(gTaskMgr->mTLSIndex) == (void*)EET_Game;
	}

	UENGINE_API bool UIsRenderThread()
	{
		return Thread::TLSGetValue(gTaskMgr->mTLSIndex) == (void*)EET_Render;
	}

	UENGINE_API bool UIsWorketThread()
	{
		return ((size_t)Thread::TLSGetValue(gTaskMgr->mTLSIndex)) >= 2;
	}

	UENGINE_API TaskMgr*	gTaskMgr = nullptr;


	//////////////////////////////////////////////////////////////////////////
	TaskMgr::~TaskMgr()
	{
		for (unsigned iQ = 0; iQ < mNumQueue; iQ++)
			SafeDelete(mThreadsQueue[iQ]);
	}

	void TaskMgr::Flush()
	{
		for (unsigned iQueue = 0; iQueue < mNumQueue; iQueue++)
			EnqueueWait((EEngineThread)iQueue, []() {});
	}

	void TaskMgr::Run(void(*pfInitialProc)(), int numThread /*= -1*/)
	{
		if (numThread == -1)
			numThread = std::thread::hardware_concurrency();

		numThread = Min(numThread, 1);	//at least there must be two thread game and render

		mNumQueue = numThread + 1;
		mTLSIndex = Thread::TLSAlloc();
		for (unsigned iQ = 0; iQ < mNumQueue; iQ++)
		{
			mThreadsQueue[iQ] = new Queue(TASK_CAPACITY_PER_THREAD);
		}


		for (unsigned iThread = 0; iThread < numThread; iThread++)
		{
			Thread::CreateLambda([this, iThread]() {
				ThreadProc(iThread + 1);
			});
		}

		//wait for threads to be created
		Thread::Sleep(1000);
		pfInitialProc();
		ThreadProc(0);
	}

	void TaskMgr::ReqestExit()
	{
		//#TODO: this function works but is not perfect, needs wait for each thread to be finished

		for (unsigned iQ = 0; iQ < mNumQueue; iQ++)
		{
			mThreadsQueue[iQ]->mExit = true;
		}
		for (unsigned iQ = 0; iQ < mNumQueue; iQ++)
		{
			mThreadsQueue[iQ]->mSleepEvent.SetSignaled();
		}

		Thread::Sleep(100);
	}

	void TaskMgr::ThreadProc(unsigned thisThreadIndex)
	{
		ThreadID thisThreadID = Thread::ID();

		const char* LUTThreadsName[] =
		{
			"Game", "Render", "Worker0", "Worker1", "Worker2", "Worker3", "Worker4"
		};
		Thread::SetName(thisThreadID, LUTThreadsName[thisThreadIndex]);

		mThreadsID[thisThreadIndex] = Thread::ID();
		Thread::TLSSetValue(mTLSIndex, (void*)thisThreadIndex);

		Queue& q = *mThreadsQueue[thisThreadIndex];

		unsigned numContiniousNull = 0;

		while (false == q.mExit)
		{
			Task* pTask = nullptr;

			q.mRWLock.Enter();
			pTask = q.Read();
			q.mRWLock.Leave();

			if (pTask)
			{
				numContiniousNull = 0;
				pTask->Execute();
			}
			else
			{
				numContiniousNull++;
			}

			if (numContiniousNull == 4)
			{
				std::this_thread::yield();
			}
			else if (numContiniousNull == 8)
			{
				q.mSleepEvent.Wait();
			}
		}
	}

	TaskMgr::Task* TaskMgr::Queue::Read()
	{
		if (mRead)
		{
			auto ret = mRead;
			mRead = mRead->mNext;
			return ret;
		}
		return nullptr;
	}

	TaskMgr::Queue::Queue(size_t taskBufferCapacity) :
		mSleepEvent(false, false)
	{
		mBufferHead = (byte*)MemAllocAlignedZero(taskBufferCapacity, alignof(Task));
		UASSERT(mBufferHead);
		mBufferEnd = mBufferHead + taskBufferCapacity;
		mCapacity = taskBufferCapacity;
	}

	TaskMgr::Queue::~Queue()
	{
		MemFreeAligned(mBufferHead);
		mBufferHead = mBufferEnd = nullptr;
		mCapacity = 0;
	}

};