#pragma once

#include "Base.h"
#include "../Core/Queue.h"
#include "../Core/Memory.h"

namespace UEngine
{


enum EEngineThread
{
	EET_Game,
	EET_Render,
	EET_Any,
};


UENGINE_API bool UIsGameThread();
UENGINE_API bool UIsRenderThread();
UENGINE_API bool UIsWorkerThread();


//////////////////////////////////////////////////////////////////////////
class UENGINE_API TaskMgr
{
	static constexpr unsigned MAX_THREAD = 8;
	static constexpr unsigned TASK_CAPACITY_PER_THREAD = 4096 * 1024;

	//base class for task
	struct alignas(16) Task
	{
		Task*	mNext = nullptr;

		virtual void Execute() {}
		virtual ~Task() {}
	};

	//helper struct to take memory for a task
	struct TaskFinal : Task
	{
		char	mBuffer[8];
	};

	//////////////////////////////////////////////////////////////////////////
	struct UENGINE_API Queue
	{
		SpinLock	mRWLock;
		Task*		mRead = nullptr;
		Task*		mLastWrite = nullptr;
		byte*		mBufferHead = nullptr;
		byte*		mBufferEnd = nullptr;
		size_t		mCapacity = 0;
		size_t		mSize = 0;
		bool		mExit = false;
		Event		mSleepEvent;


		//write a task and return the pinter to it
		template<typename TLambda> Task* Write(TLambda task)
		{
			struct NewTask : Task
			{
				TLambda mTask;
				NewTask(const TLambda& copy) : mTask(copy) {}
				virtual void Execute() override
				{
					mTask();
				}
			};

			UASSERT(this->mBufferHead && this->mBufferEnd);
			UASSERT(!this->mExit);
				
			size taskSize = sizeof(NewTask);

			Task* pNewTask = nullptr;

			if (mLastWrite == nullptr)
			{
				pNewTask = new (mBufferHead) NewTask(task);
				mLastWrite = pNewTask;
				mRead = pNewTask;
			}
			else
			{
				//#TODO must check for not overriding to a written task 

				auto newWrite = ((byte*)mLastWrite) + taskSize;
				if (newWrite >= mBufferEnd) //overflow?
					newWrite = mBufferHead;

				pNewTask = new (newWrite) NewTask(task);
				mLastWrite->mNext = pNewTask;
				mLastWrite = pNewTask;
				if (mRead == nullptr)
					mRead = pNewTask;
			}

			mSleepEvent.SetSignaled();
			

			return pNewTask;
		}
		//read a task and get the pointer to it, null if empty
		Task* Read();
		Queue(size_t taskBufferCapacity);
		~Queue();

	};


public:

	TaskMgr() 
	{}
	~TaskMgr();
	//////////////////////////////////////////////////////////////////////////
	template <typename TLambda> void Enqueue(EEngineThread targetThread, TLambda task)
	{
		size_t targetThreadIndex = (size_t)targetThread;
		size_t thisThreadIndex = (size_t)Thread::TLSGetValue(mTLSIndex);
		if (thisThreadIndex == targetThreadIndex) //is same
		{
			task();
			return;
		}

		UASSERT(targetThreadIndex < mNumQueue);
		Queue& q = *mThreadsQueue[targetThreadIndex];

		q.mRWLock.Enter();
		{
			struct NewTask : Task
			{
				TLambda mTask;
				NewTask(const TLambda& copy) : mTask(copy) {}
				virtual void Execute() override
				{
					mTask();
				}
			};

			UASSERT(q.mBufferHead && q.mBufferEnd);
			UASSERT(!q.mExit);

			size taskSize = sizeof(NewTask);

			Task* pNewTask = nullptr;

			if (q.mLastWrite == nullptr)
			{
				pNewTask = new (q.mBufferHead) NewTask(task);
				q.mLastWrite = pNewTask;
				q.mRead = pNewTask;
			}
			else
			{
				//#TODO must check for not overriding to a written task 

				auto newWrite = ((byte*)q.mLastWrite) + taskSize;
				if (newWrite >= q.mBufferEnd) //overflow?
					newWrite = q.mBufferHead;

				pNewTask = new (newWrite) NewTask(task);
				q.mLastWrite->mNext = pNewTask;
				q.mLastWrite = pNewTask;
				if (q.mRead == nullptr)
					q.mRead = pNewTask;
			}


		}
		q.mRWLock.Leave();
		//wake up the target thread, we have new task for him :D
		q.mSleepEvent.SetSignaled();
	}


	//////////////////////////////////////////////////////////////////////////
	template <typename TLambda> void EnqueueWait(EEngineThread targetThread, TLambda task)
	{
		size_t targetThreadIndex = (size_t)targetThread;
		size_t thisThreadIndex = (size_t)Thread::TLSGetValue(mTLSIndex);
		if (thisThreadIndex == targetThreadIndex) //is same
		{
			task();
			return;
		}

		UASSERT(targetThreadIndex < mNumQueue);
		Queue& q = *mThreadsQueue[targetThreadIndex];

		q.mRWLock.Enter();
		{
			struct NewTask : Task
			{
				TLambda mTask;
				Event& mEvent; //the event that must be signaled when this task is finished to free the thread which created this task

				NewTask(const TLambda& copy, Event& producerEvent) : mTask(copy), mEvent(producerEvent) {}
				virtual void Execute() override
				{
					mTask();
					mEvent.SetSignaled();
				}
			};

			UASSERT(q.mBufferHead && q.mBufferEnd);
			UASSERT(!q.mExit);

			size taskSize = sizeof(NewTask);

			Task* pNewTask = nullptr;

			if (q.mLastWrite == nullptr)
			{
				pNewTask = new (q.mBufferHead) NewTask(task, mThreadWaits[thisThreadIndex]);
				q.mLastWrite = pNewTask;
				q.mRead = pNewTask;
			}
			else
			{
				//#TODO must check for not overriding to a written task 

				auto newWrite = ((byte*)q.mLastWrite) + taskSize;
				if (newWrite >= q.mBufferEnd) //overflow?
					newWrite = q.mBufferHead;

				pNewTask = new (newWrite) NewTask(task, mThreadWaits[thisThreadIndex]);
				q.mLastWrite->mNext = pNewTask;
				q.mLastWrite = pNewTask;
				if (q.mRead == nullptr)
					q.mRead = pNewTask;
			}

			
		}
		q.mRWLock.Leave();
		//wake up the target thread we have new task for him :D
		q.mSleepEvent.SetSignaled();

		mThreadWaits[thisThreadIndex].Wait();

	}
	size_t GetThisThreadIndex() const
	{
		return (size_t)Thread::TLSGetValue(mTLSIndex);
	}
	void Flush();
	void Run(void(*pfInitialProc)(), int numThread = -1);
	void ReqestExit();

	void ThreadProc(unsigned thisThreadIndex);


	//maps thread index to its id
	ThreadID	mThreadsID[MAX_THREAD] = {};
	//a single consumer multiple producer queue for each thread
	Queue*		mThreadsQueue[MAX_THREAD] = {};
	Event		mThreadWaits[MAX_THREAD];
	//TLS to get current thread index
	TLSIndex	mTLSIndex = 0;
	//number of queues that we have equal with the number of thread + 1
	unsigned	mNumQueue = 0;
};

extern UENGINE_API TaskMgr* gTaskMgr;





//enqueue a task to the specified thread
template<typename TLambda> void UEnqueue(EEngineThread targetThread, const TLambda& task)
{
	gTaskMgr->Enqueue(targetThread, task);
}
//enqueue a task to the specified thread and wait for it to be finished
template<typename TLambda> void UEnqueueWait(EEngineThread targetThread, const TLambda& task)
{
	gTaskMgr->EnqueueWait(targetThread, task);
}

//wait for all the tasks to be finished
inline void UFlushTasks()
{
	gTaskMgr->Flush();
}



};