#pragma once

#include "Base.h"
#include "Array.h"
#include "Object.h"
#include "ObjectPtr.h"
#include "SmartPointers.h"

namespace UCore
{
	enum class EDelegateBindType
	{
		Null, StaticFunction, MemberFunction, ObjectMemberFunction, SharedPtrMemberFunction, WeakPtrMemberFunction, Lambda, 
	};


	template<typename T> class TDelegate {};
	template<typename TRet, class... TArgs> class TDelegate<TRet(TArgs...)>
	{
	public:
		static const unsigned MAX_LAMBDA_SIZE = 32;

		using RetT = TRet;
		using PatternT = TRet(TArgs...);

		//null initialization
		TDelegate()
		{
			mType = EDelegateBindType::Null;
		}
		TDelegate(std::nullptr_t)
		{
			mType = EDelegateBindType::Null;
		}
		TDelegate(const TDelegate& copy)
		{
			mType = copy.mType;

			switch (mType)
			{
			case EDelegateBindType::Null:
				break;
			case EDelegateBindType::StaticFunction:
				new (mBuffer) SFunc(*copy.AsFunc());
				break;
			case EDelegateBindType::MemberFunction:
				new (mBuffer) SMemFunc(*copy.AsMemFunc());
				break;
			case EDelegateBindType::ObjectMemberFunction:
				new (mBuffer) SMemFuncObj(*copy.AsMemFuncObj());
				break;
			case EDelegateBindType::SharedPtrMemberFunction:
				new (mBuffer) SMemFuncShared(*copy.AsMemFuncShared());
				break;
			case EDelegateBindType::WeakPtrMemberFunction:
				new (mBuffer) SMemFuncWeak(*copy.AsMemFuncWeak());
				break;
			case EDelegateBindType::Lambda:
				new (mBuffer) SLambda(*copy.AsLambda());
				break;
			}
		}
		TDelegate& operator = (std::nullptr_t)
		{
			this->~TDelegate();
			return *this;
		}
		static TDelegate MakeStatic(TRet(*function)(TArgs...))
		{
			UASSERT(function);
			return TDelegate(EDelegateBindType::StaticFunction, SFunc(function));
		}
		template<typename TClass> static TDelegate MakeMember(void* object, TRet(TClass::* memberfunction)(TArgs...))
		{
			UASSERT(object && memberfunction);
			return TDelegate(EDelegateBindType::MemberFunction, SMemFunc(object, memberfunction));
		}
		template<class TClass> static TDelegate MakeMemberObject(Object* object, TRet(TClass::* memberfunction)(TArgs...))
		{
			UASSERT(memberfunction);
			return TDelegate(EDelegateBindType::ObjectMemberFunction, SMemFuncObj(object, memberfunction));
		}
		template<typename TLambda> static TDelegate MakeLambda(const TLambda& lambda)
		{
			TDelegate dlg;
			dlg._InitLambda<TLambda>(lambda);
			return dlg;
		}
		template<class TClass> static TDelegate MakeSharedPtr(const TSPtr<ISmartBase>& sptr, TRet(TClass::* memberfunction)(TArgs...))
		{
			UASSERT(memberfunction);
			if (sptr.IsValid())
				return TDelegate(EDelegateBindType::SharedPtrMemberFunction, SMemFuncShared(sptr, memberfunction));

			return TDelegate();
		}
		template<typename TClass> static TDelegate MakeWeakPtr(const TWPtr<IWeakBase>& wptr, TRet(TClass::* memberfunction)(TArgs...))
		{
			UASSERT(memberfunction);
			if(wptr.IsValid())
				return TDelegate(EDelegateBindType::WeakPtrMemberFunction, SMemFuncWeak(wptr, memberfunction));

			return TDelegate();
		}

		void BindStatic(TRet(*function)(TArgs...))
		{
			UASSERT(function);
			this->~TDelegate();
			mType = EDelegateBindType::StaticFunction;
			AsFunc()->mFunc = function;
		}
		template<typename TClass> void BindMember(void* object, TRet(TClass::* memberfunction)(TArgs...))
		{
			UASSERT(object && memberfunction);
			this->~TDelegate();
			mType = EDelegateBindType::MemberFunction;
			AsMemFunc()->mFunc = memberfunction;
			AsMemFunc()->mObj = object;
		}
		template<class TClass> void BindMemberObject(Object* object, TRet(TClass::* memberfunction)(TArgs...))
		{
			UASSERT(memberfunction);
			this->~TDelegate();
			if (object)
			{
				mType = EDelegateBindType::ObjectMemberFunction;
				new (mBuffer) SMemFuncObj(memberfunction, object);
			}
		}
		template<typename TLambda> void BindLambda(const TLambda& lambda)
		{
			this->~TDelegate();

			_InitLambda<TLambda>(lambda);
		}
		template<typename TLambda> void BindLambdaObject(Object* object, const TLambda& lambda)
		{
			this->~TDelegate();

			_InitLambdaObject<TLambda>(object, lambda);
		}

		EDelegateBindType BindType() const { return mType; }

		bool IsBound() const
		{
			switch (mType)
			{
			case EDelegateBindType::Null: return false;
			case EDelegateBindType::ObjectMemberFunction: return AsMemFuncObj()->IsValid();
			case EDelegateBindType::WeakPtrMemberFunction: return AsMemFuncWeak()->IsValid();
			}
			return true;
		}
		bool IsObjectBound(void* object) const
		{
			if (object == nullptr) return false;

			switch (mType)
			{
			case EDelegateBindType::MemberFunction: return AsMemFunc()->mObj == object;
			case EDelegateBindType::ObjectMemberFunction: return AsMemFuncObj()->mObj.Get() == object;
			case EDelegateBindType::SharedPtrMemberFunction: return AsMemFuncShared()->mShared.Get() == object;
			case EDelegateBindType::WeakPtrMemberFunction: return AsMemFuncWeak()->mWeak.Get() == object;
			}

			return false;
		}
		TDelegate& operator = (const TDelegate& copy)
		{
			if (this == &copy) return *this;

			this->~TDelegate();
			new (this) TDelegate(copy);

			return *this;
		}
		~TDelegate()
		{
			switch (mType)
			{
			case EDelegateBindType::ObjectMemberFunction:
				AsMemFuncObj()->~SMemFuncObj();
				break;
			case EDelegateBindType::SharedPtrMemberFunction:
				AsMemFuncShared()->~SMemFuncShared();
				break;
			case EDelegateBindType::WeakPtrMemberFunction:
				AsMemFuncWeak()->~SMemFuncWeak();
				break;
			case EDelegateBindType::Lambda:
				AsLambda()->~SLambda();
				break;
			}
			mType = EDelegateBindType::Null;
		}
		TRet Invoke(TArgs... args) const
		{
			switch (mType)
			{
			case EDelegateBindType::StaticFunction: return AsFunc()->Call(args...);
			case EDelegateBindType::MemberFunction: return AsMemFunc()->Call(args...);
			case EDelegateBindType::ObjectMemberFunction: return AsMemFuncObj()->Call(args...);
			case EDelegateBindType::SharedPtrMemberFunction: return AsMemFuncShared()->Call(args...);
			case EDelegateBindType::WeakPtrMemberFunction: return AsMemFuncWeak()->Call(args...);
			case EDelegateBindType::Lambda: return AsLambda()->Call(args...);
			}
			return TRet();
		}

		operator bool() const { return IsBound(); }

		TRet operator()(TArgs... args) const
		{
			return Invoke(args...);
		}

		bool operator == (const TDelegate& other) const
		{
			if (mType == other.mType)
			{
				switch (mType)
				{
				case UCore::EDelegateBindType::Null: return true;
				case UCore::EDelegateBindType::StaticFunction:
					return *AsFunc() == *other.AsFunc();
				case UCore::EDelegateBindType::MemberFunction:
					return *AsMemFunc() == *other.AsMemFunc();
				case UCore::EDelegateBindType::ObjectMemberFunction:
					return *AsMemFuncObj() == *other.AsMemFuncObj();
				case UCore::EDelegateBindType::SharedPtrMemberFunction:
					return *AsMemFuncShared() == *other.AsMemFuncShared();
				case UCore::EDelegateBindType::WeakPtrMemberFunction:
					return *AsMemFuncWeak() == *other.AsMemFuncWeak();
				case UCore::EDelegateBindType::Lambda:
					return *AsLambda() == *AsLambda();
				}
			}
			return false;
		}
		bool operator != (const TDelegate& other) const
		{
			return !this->operator==(other);
		}
	private:


		using LambdaInvokeT = TRet(*)(void*, TArgs...);
		using LambdaDTorT = void(*)(void*);
		using LambdaCTorT = void(*)(void*);
		using LambdaCCTorT = void(*)(void*, const void*);

		using StaticFuncT = TRet(*)(TArgs...);
		using MemberFuncT = TMFP<TRet(TArgs...)>;


#pragma region structs
		struct SFunc
		{
			SFunc() {}
			SFunc(StaticFuncT func) : mFunc(func) {}

			StaticFuncT mFunc;

			inline auto Call(TArgs... args)
			{
				return mFunc(args...);
			}
			bool operator == (const SFunc& other) const { return mFunc == other.mFunc; }
		};
		struct SMemFunc
		{
			SMemFunc() {}
			SMemFunc(void* obj, MemberFuncT func) : mFunc(func), mObj(obj) {}

			MemberFuncT mFunc;
			void* mObj;
			inline auto Call(TArgs... args)
			{
				return mFunc(mObj, args...);
			}
			bool operator == (const SMemFunc& other) const { return mFunc == other.mFunc && mObj == other.mObj; }
		};
		struct SMemFuncObj
		{
			ObjectPtr mObj;
			MemberFuncT mFunc;
			
			SMemFuncObj() {}
			SMemFuncObj(Object* obj, MemberFuncT func) : mFunc(func), mObj(obj) {}

			inline auto Call(TArgs... args)
			{
				if (Object* obj = mObj.Get()) return mFunc(obj, args...);
				return TRet();
			}
			bool IsValid() const { return mObj.Get() != nullptr; }
			bool operator == (const SMemFuncObj& other) const { return mFunc == other.mFunc && mObj == other.mObj; }
		};
		struct SMemFuncShared
		{
			TSPtr <ISmartBase> mShared;
			MemberFuncT mFunc;

			SMemFuncShared() {}
			SMemFuncShared(const TSPtr<ISmartBase>& shared, MemberFuncT func) : mFunc(func), mShared(shared) {}

			inline auto Call(TArgs... args)
			{
				return mFunc(mShared.Get(), args...);
			}
			bool operator == (const SMemFuncShared& other) const { return mFunc == other.mFunc && mShared == other.mShared; }
		};
		struct SMemFuncWeak
		{
			TWPtr <IWeakBase> mWeak;
			MemberFuncT mFunc;

			SMemFuncWeak() {}
			SMemFuncWeak(const TWPtr<IWeakBase>& weak, MemberFuncT func) : mFunc(func), mWeak(weak) {}

			inline auto Call(TArgs... args)
			{
				if (auto obj = mWeak.Get()) return mFunc(obj, args...);
				return TRet();
			}
			bool IsValid() const { return mWeak.IsValid(); }
			bool operator == (const SMemFuncWeak& other) const { return mFunc == other.mFunc && mWeak == other.mWeak; }
		};
		struct SLambda
		{
			char				mBuffer[MAX_LAMBDA_SIZE];
			LambdaInvokeT		mInvoke;
			LambdaDTorT			mDTor;
			LambdaCCTorT		mCCTor;

			SLambda(const SLambda& copy)
			{
				mInvoke = copy.mInvoke;
				mDTor = copy.mDTor;
				mCCTor = copy.mCCTor;

				mCCTor(mBuffer, copy.mBuffer);
			}
			~SLambda()
			{
				mDTor(mBuffer);
			}
			inline auto Call(TArgs... args)
			{
				return mInvoke(mBuffer, args...);
			}
			bool operator == (const SLambda& other) const { return mInvoke == other.mInvoke; }
		};
#pragma endregion
		
		template<typename TImp> TDelegate(EDelegateBindType type, const TImp& imp) : mType(type)
		{
			new (mBuffer) TImp(imp);
		}

		SFunc* AsFunc() const { return (SFunc*)mBuffer; }
		SMemFunc* AsMemFunc() const { return (SMemFunc*)mBuffer; }
		SMemFuncObj* AsMemFuncObj() const { return (SMemFuncObj*)mBuffer; }
		SMemFuncShared* AsMemFuncShared() const { return (SMemFuncShared*)mBuffer; }
		SMemFuncWeak* AsMemFuncWeak() const { return (SMemFuncWeak*)mBuffer; }
		SLambda* AsLambda() const { return (SLambda*)mBuffer; }

		template<typename TAs> TAs* As() const { (TAs*)mBuffer; }

		template<typename TFunc> static TRet _LambdaInvoke(void* ins, TArgs... args)
		{
			return (*(reinterpret_cast<TFunc*>(ins)))(args...);
		}
		template<typename TFunc> static void _LambdaCTor(void* p)
		{
			new (p) TFunc;
		}
		template<typename TFunc> static void _LambdaDTor(void* p)
		{
			reinterpret_cast<TFunc*>(p)->~TFunc();
		}
		template<typename TFunc> static void _LambdaCCTor(void* a, const void* b)
		{
			new (a) TFunc(*((const TFunc*)b));
		}
		template<typename TFunc> void _InitLambda(const TFunc& func)
		{
			mType = EDelegateBindType::Lambda;

			SLambda* cast = AsLambda();
			cast->mInvoke = reinterpret_cast<LambdaInvokeT>(&_LambdaInvoke<TFunc>);
			cast->mCCTor = reinterpret_cast<LambdaCCTorT>(&_LambdaCCTor<TFunc>);
			cast->mDTor = reinterpret_cast<LambdaDTorT>(&_LambdaDTor<TFunc>);

			new (cast->mBuffer) TFunc(func);

		}

		//////////////////////////////////////////////////////////////////////////memvars
		EDelegateBindType	mType;
		char				mBuffer[sizeof(SLambda)];
	};


	//////////////////////////////////////////////////////////////////////////muticast delegate
	template<typename T> class TDelegateMulti {};

	template<typename TRet, typename... TArgs> class TDelegateMulti<TRet(TArgs...)>
	{
	public:
		using DelegateType = TDelegate<TRet(TArgs...)>;

		TArray<DelegateType>	mDeletages;


		TDelegateMulti()
		{

		}
		~TDelegateMulti()
		{

		}
		void InvokeAll(TArgs... args) const
		{
			for (size_t i = 0; i < mDeletages.Length(); i++)
			{
				if (mDeletages[i].IsBound())
				{
					mDeletages[i].Invoke(args...);
				}
			}
		}
		
		TDelegateMulti& operator << (const DelegateType& dlg) { return Add(dlg); }

		TDelegateMulti& Add(const DelegateType& dlg)
		{
			mDeletages.Add(dlg);
			return *this;
		}
		TDelegateMulti& AddUnique(const DelegateType& dlg)
		{
			mDeletages.AddUnique(dlg);
			return *this;
		}
		//unbind all the delegates that are using 'obj'
		void UnbindAll(void* obj)
		{
			mDeletages.ConditionalRemove([obj](const DelegateType& dlg)
			{
				return dlg.IsObjectBound(obj);
			});
		}
		void Clear()
		{
			mDeletages.RemoveAll();
		}
		TDelegateMulti& operator = (std::nullptr_t) { Clear(); return *this; }
	};
};