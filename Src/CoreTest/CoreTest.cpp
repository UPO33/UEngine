#pragma once

#include "../Core/All.h"
#include "../Core/SmartMemory.h"
#include "../Core/Bound.h"

#include "MetaTest.h"

namespace UCoreTest
{
 	using namespace UCore;

	template<typename TChar> bool UStrHasAnyOf(const TChar* str, TArray<TStrRange<TChar>>& ranges)
	{
		for (const auto range : ranges)
		{
			if (UStrStr(str, range))
				return true;
		}
		return false;
	}

	void TestString()
	{
		TArray<TStrRange<char>> ranges;
		USplitString("asd qwe zxc", " \t,", ranges);
		for(const auto& r : ranges)
		{
			ULOG_MESSAGE("Split [%] empty %", r, r.IsEmpty());
		}
		UASSERT(UStrHasAnyOf("test string asd test", ranges));
	}
	Vec3 TruncDecimal(const Vec3& v, float mul)
	{
		return Trunc(v * mul) * (1 / mul);
	}
	void TestSPMQ()
	{
		static const unsigned NumConsumer = 3;
		static unsigned NumTest[NumConsumer] = { 0, 0, 0};
		static unsigned NumTest2[NumConsumer] = { 0, 0, 0 };

		TCommandQueueSPNC<NumConsumer, 1028, 32> cmdQueue;
		cmdQueue.Run();

		Sleep(2000);

		for (size_t i = 0; i < 1024; i++)
		{
			unsigned targetThread = rand() % NumConsumer;
			NumTest[targetThread]++;

			cmdQueue.Enqueue(targetThread, [targetThread](){
				NumTest2[targetThread]++;
				//ULOG_MESSAGE("msg %", targetThread);
			});
		}
		

		cmdQueue.EnqueueAndWait(0, [] {});
		cmdQueue.RequestExit();


		Sleep(1000);
		for (size_t i = 0; i < NumConsumer; i++)
		{
			UASSERT(NumTest[i] == NumTest2[i]);
		}
		ULOG_MESSAGE("finished");
	}
	void QueueTest()
	{
// 		ULOG_MESSAGE("start");

		TCommandQueueSPSC<4> cmdQueue;
		cmdQueue.Run();

		Sleep(100);

		Vec3 v0 = Vec3(0.0);
		Vec3 v1 = Vec3(1.0);
		Vec3 v2 = Vec3(2.0);

		for(int i = 0; i < 100; i ++)
		{
			cmdQueue.Enqueue([i, v0, v1, v2] {
				ULOG_MESSAGE("% msg % % %", i, v0, v1, v2);
			});
		}
		cmdQueue.EnqueueAndWait([v0, v1, v2] {
			ULOG_MESSAGE("2nth msg % % %", v0, v1, v2);
		});
		cmdQueue.RequestExit();

		struct QElem
		{
			int buffer0[8];
			int id;
			int buffer[32];
		};

		TCircularQueueSQSP<QElem, 100> testQueue;

		Thread::CreateLambda([&]() {
			int idCounter = 0;
			for (size_t i = 0; i < 1000; i++)
			{
				if (QElem* elem = testQueue.BeginPush())
				{
					elem->id = idCounter;
					idCounter++;
					Thread::Sleep(rand() % 2);
					testQueue.EndPush();
					
				}
			}
		});

		//Thread::Sleep(99);

		int lastId =  -1;
		for (size_t i = 0; i < 1000; i++)
		{
			if (QElem* elem = testQueue.BeginPop())
			{
				UASSERT(elem->id == lastId + 1);
				ULOG_MESSAGE("%", elem->id);
				//printf("%d\n", elem->id);
				lastId = elem->id;
				Thread::Sleep(rand() % 2);
				testQueue.EndPop();
				
			}
		};
		Sleep(2000);
		ULOG_MESSAGE("end");

	}
	void MathTest()
	{
	}
	void ConfTest()
	{

	}
	void TestArray()
	{
		TArray<int> arr0;
		arr0.AddUnInit(32);
		for (size_t i = 0; i < 32; i++)
		{
			arr0[i] = i;
		}
		TArray<int, TArrayAllocStack<int, 128>> arr1;
		arr1 += arr0;
		UASSERT(MemCmp(arr0.Elements(), arr1.Elements(), arr0.Length() * sizeof(int)) == 0);
		arr0.ConditionalRemovePOD([](int value)
		{
			return value % 2 == 0;	//remove all even numbers
		});
		for (int value : arr0)
		{
			UASSERT(value % 2 != 0);	//we removed even numbers above
		}

		//testing ctor
		{
			TArray<TObjectPtr<Object>>	arrayObject0;
			arrayObject0.AddDefault(32);
			TObjectPtr<Object>& obj = arrayObject0[0];
			obj = nullptr;
		}
	}
	int DLG0()
	{
		return 1;
	}


	class MyObject : public Object
	{
	public:
		bool MousePress(int x, int y)
		{
			ULOG_MESSAGE("%  %", x, y);
			return false;
		}
	};
	class MySmart : public ISmartBase
	{
	public:
		bool MousePress(int x, int y)
		{
			ULOG_MESSAGE("%  %", x, y);
			return false;
		}
	};
	bool StaticMousePress(int x, int y)
	{
		ULOG_MESSAGE("%  %", x, y);
		return false;
	}
// 	template<typename T> class TFPNew {};
// 	template<typename TRet, typename TArgs...> class TFPNew<TRet(TArgs...)>
// 	{
// 	public:
// 		using RetT = TRet;
// 		using Pattern = TRet(*)(TArgs...);
// 	};
	

	void LogTest()
	{
		auto threadProc = []()
		{
			for (size_t i = 0; i < 123; i++)
				ULOG_MESSAGE("%_%", i, Thread::ID());
		};

		Thread::CreateLambda(threadProc);
		Thread::CreateLambda(threadProc);
		Thread::CreateLambda(threadProc);

		
		Thread::Sleep(1000);

		GLog()->BeginReadLogs();
		{
			for (size_t i = 0; i < GLog()->GetNumLog(); i++)
			{
				const LogEntry* log = GLog()->GetLog(i);
				printf("%s\n", log->mMessage);
			}
		}
		GLog()->EndReadLogs();

	}
	void SmartPtrTest()
	{
		class MySmartClass : public ISmartBase
		{
		public:
			int a;
			int b;

			MySmartClass()
			{
				ULOG_MESSAGE("");
			}
			~MySmartClass()
			{
				ULOG_MESSAGE("");
			}
			bool MousePress(int x, int y)
			{
				ULOG_MESSAGE("%  %", b, a);
				return true;
			}
		};
		class MySmartClass2 : public MySmartClass
		{
		public:
			const char* m2_1;
			const char* m2_2;
			MySmartClass2()
			{
				m2_1 = "m2_1";
				m2_2 = "m2_2";
				
			}
			~MySmartClass2()
			{
				ULOG_MESSAGE("");
			}
			bool MousePress(int x, int y)
			{
				ULOG_MESSAGE("%  %", m2_1, m2_2);
				return true;
			}
		};

		class MySmartClass3 : public MySmartClass2
		{
		public:
			int m3_1 = 8;
			int m3_2 = 16;
			MySmartClass3()
			{
				ULOG_MESSAGE("");
			}
			~MySmartClass3()
			{
				ULOG_MESSAGE("");
			}
			bool MousePress(int x, int y)
			{
				ULOG_MESSAGE("%  %", m3_1, m3_2);
				return true;
			}
		};

		

		static int CDCounter = 0;
		{
			using TMousePress = bool(int x, int y);
			using TDMousePress = TDelegate<TMousePress>;
		
			MySmartClass3* ms3 = new MySmartClass3;
			TSPtr<MySmartClass3> sp1 = ms3;
			TWPtr<MySmartClass3> wptr = ms3;

			TDelegateMulti<TMousePress>	OnClick;

// 			OnClick << TDMousePress::MakeStatic(&StaticMousePress);
			OnClick << TDMousePress::MakeMember(ms3, &MySmartClass3::MousePress);
			OnClick << TDMousePress::MakeSharedPtr(sp1, &MySmartClass2::MousePress); 
			OnClick << TDMousePress::MakeWeakPtr(wptr, &MySmartClass2::MousePress);
			OnClick.UnbindAll(ms3);


			OnClick.InvokeAll(1, 2);


			//////////////////////////////////////////////////////////////////////////object Test
			Object* mObj = new MyObject;
			TWPtr<Object> mObjPtr = mObj;
		}

		
		ULOG_MESSAGE("Counter %", CDCounter);
	}

	template<typename TElement> struct EntChildren
	{
		TElement* ent;

		class Iter
		{
		public:
			TElement* ptr;
			Iter(TElement* p) : ptr(p) {}
			bool operator !=(Iter rhs) const { return ptr != rhs.ptr; }
			TElement& operator*() { return *ptr; }
			void operator++() 
			{
				ptr++; 
				if (ptr)
					ptr = ptr->mObjectDown;
			}
		};
	};
	void SmartMemTest()
	{
		TSPtr<SmartMemBlock> mem0;
		{
			mem0 = new SmartMemBlock(128);
			UASSERT(!mem0->IsNull());
		}
		SmartMemBlock* mem1 = new SmartMemBlock(128);
		SmartMemBlock* mem2 = new SmartMemBlock(std::move(*mem1));
		UASSERT(mem1->IsNull());
		UASSERT(!mem2->IsNull());
	}
	void FileTest()
	{
		if (SmartMemBlock* fileContent = UFileOpenReadFull("../../simple.txt"))
		{
			ULOG_MESSAGE("%", (char*)fileContent->Memory());
		}
	}

	void BoundTest()
	{

	}
	
	void Main()
	{
		LCATest();
		BoundTest();
		//TestSPMQ();
		//QueueTest();
		//MathTest();
		//SmartPtrTest();
		SmartMemTest();
		FileTest();
		ConfTest();
		TestArray();
		MetaTestMain();
		//LogTest();
	}
}
#include <experimental/filesystem>
#include <filesystem>

int main(int argc, char** argv)
{
	extern void MathTest();
	MathTest();
	UCoreTest::TestString();

	UCoreTest::Main();
	system("pause");
	return 0;
}