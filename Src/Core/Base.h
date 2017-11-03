#pragma once

//#include <math.h>
#include <cmath>
#include <iostream>
#include <time.h>
#include <type_traits>
#include <functional>
#include <memory.h>
#include <cstdint>
#include <sstream>
#include <cstdlib>
#include <stdlib.h>
#include <assert.h>



#if defined(_MSC_VER)
#define UCOMPILER_MSVC
#define UMODULE_IMPORT __declspec(dllimport)
#define UMODULE_EXPORT __declspec(dllexport)
#elif defined(__GNUC__)
#define UCOMPILER_GCC
#define UMODULE_IMPORT
#define UMODULE_EXPORT  __attribute__((visibility("default")))
#else
#error not implemented yet
#endif


#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
#define UPLATFORM_WIN 1
#define UPLATFORM_ID  UPLATFORM_WIN
#endif
#if defined(__linux__) || defined(__gnu_linux__)
#define UPLATFORM_LINUX 2
#define UPLATFORM_ID  UPLATFORM_LINUX
#endif
#if defined(__ANDROID__)
#define UPLATFORM_ANDROID 3
#ifdef UPLATFORM_ID
#undef UPLATFORM_ID
#endif
#define UPLATFORM_ID  UPLATFORM_ANDROID
#endif



#if INTPTR_MAX == INT64_MAX
#define U64BIT
#elif INTPTR_MAX == INT32_MAX
#define U32BIT
#else
#error Unknown pointer size or missing size macros!
#endif

#ifdef UCORE_SHARED
#ifdef UCORE_BUILD
#define UCORE_API UMODULE_EXPORT
#else
#define UCORE_API UMODULE_IMPORT
#endif
#else
#define UCORE_API
#endif

#ifdef UCOMPILER_MSVC
//#define UBREAK_DEBUGGER() (*((int*)0) = 0)
#define UBREAK_DEBUGGER() __debugbreak()
#else
#define UBREAK_DEBUGGER() assert(false)
#endif



#pragma region Log and assert

#define ULOG_MESSAGE(format, ...) UCore::GLog()->Add(UCore::ELogType::ELT_Message, __FILE__, __FUNCTION__, __LINE__, format, ## __VA_ARGS__ )
#define ULOG_SUCCESS(format, ...) UCore::GLog()->Add(UCore::ELogType::ELT_Success, __FILE__, __FUNCTION__, __LINE__, format, ## __VA_ARGS__ )
#if 1 //add breakpoint to ULOG_ERROR or not?
#define ULOG_ERROR(format, ...) UCore::GLog()->Add(UCore::ELogType::ELT_Error, __FILE__, __FUNCTION__, __LINE__, format, ## __VA_ARGS__ ); __debugbreak()
#else
#define ULOG_ERROR(format, ...) UCore::GLog()->Add(UCore::ELogType::ELT_Error, __FILE__, __FUNCTION__, __LINE__, format, ## __VA_ARGS__ );
#endif
#define ULOG_WARN(format, ...) UCore::GLog()->Add(UCore::ELogType::ELT_Warn, __FILE__, __FUNCTION__, __LINE__, format, ## __VA_ARGS__ )

#define ULOG_TEST(format, ...) ULOG_ERROR(format, __VA__ARGS__ )


//it worked on android
//#define	UFATAL(e, ...)	((e) ? (void)0 : __assert(__FILE__, __LINE__, #e))

#define UFATAL(expression, ...) ((expression) ? (void)0 : (UCore::Log::Get()->Add(UCore::ELogType::ELT_Assert, __FILE__, __FUNCTION__, __LINE__, "Assertaion Failed [" #expression "]  ", ##__VA_ARGS__ ), UBREAK_DEBUGGER()))

#ifdef NDEBUG
#define ULOG_FATAL(format, ...) ((void)0)
#define UASSERT(expression, ...) ((expression) ? (void)0 : (void)0)
#else
//#define ULOG_FATAL(format, ...)
//#define UASSERT(expression, ...) 
#define ULOG_FATAL(format, ...) (UCore::GLog()->Add(UCore::ELogType::ELT_Fatal, __FILE__, __FUNCTION__, __LINE__, format, ## __VA_ARGS__), UBREAK_DEBUGGER())
#define UASSERT UFATAL
#endif


#pragma endregion


#define UARRAYLEN(Array)  (sizeof(Array) / sizeof(Array[0]))


//UWELD(prefix, __COUNTER__) will results  prefix__COUNTER__
#define UWELD(Prefix, Suffix) Prefix##Suffix
//UWELDINNER(prefix, __COUNTER__) will results  prefix1
#define UWELDINNER(Prefix, Suffix) UWELD(Prefix, Suffix)

#define UCACHE_ALIGN	64

namespace UCore
{
	class ClassInfo;
	class ByteSerializer;
	class ByteDeserializer;
	struct ZZ_ClassTypeCheckResult;


};

template<typename T = void> struct ZZParentCatch { typedef T TT; };

#define UCLASS(Class, ...)\
public:\
	static int ZZLineNumber() { return __LINE__; }\
	static const char* ZZFileName() { return __FILE__; }\
	void ZZIsMeta() {}\
	friend struct ZZZ_##Class;\
	friend UCore::ZZ_ClassTypeCheckResult;\
	typedef typename ZZParentCatch<__VA_ARGS__>::TT ParentT;\
	typedef Class SelfT;\
	static const UCore::ClassInfo* GetClassStatic();\









/*
in order to check a class has the specified member function or to get pointer to member function whether available or not,
first u must declare that with UDECLARE_MEMBERFUNCTION_CHECKING(DeclarationName, FunctionName, ReturnType, Args...)
then u are able to use UCLASS_HAS_MEMBERFUNCTION and UCLASS_GET_MEMBERFUNCTION
note:
it doesn't take the parent's class functions into account
*/
#define UDECLARE_MEMBERFUNCTION_CHECKING(DeclarationName, FunctionName, ReturnType, ...)\
	template <class Type> struct Z_##DeclarationName##HasFuncChecker\
	{\
		using TFunc = ReturnType (Type::*) (__VA_ARGS__);\
		struct FakeType { ReturnType FunctionName(__VA_ARGS__) { return exit(0); } };\
		template <typename T, T> struct TypeCheck; \
		template <typename T> struct FuncCheck { typedef ReturnType(T::*fptr)(__VA_ARGS__); }; \
		template <typename T> static char HasFunc(TypeCheck< typename FuncCheck<T>::fptr, &T::FunctionName >*); \
		template <typename T> static long  HasFunc(...); \
		static bool const value = (sizeof(HasFunc<Type>(0)) == sizeof(char)); \
	}; \
	template<typename T> struct Z_##DeclarationName##HasFunc\
	{\
		using TFunc = ReturnType (UCore::Void::*) (__VA_ARGS__);\
		struct FakeType { ReturnType FunctionName(__VA_ARGS__) { return exit(0); } };\
		struct FakeT { static bool const value = 0; };\
		static bool const value = std::conditional<std::is_class<T>::value, Z_##DeclarationName##HasFuncChecker<T>, FakeT>::type::value;\
		static TFunc GetFunc() \
		{\
			using TargetClass = typename std::conditional<value, T, FakeType>::type;\
			if (std::is_same<TargetClass, FakeType>::value) return nullptr;\
			else return (TFunc)&TargetClass::FunctionName;\
		}\
	}; \

//return true if 'Class' has specified function function must be declared with UDECLARE_MEMBERFUNCTION_CHECKING once
#define UCLASS_HAS_MEMBERFUNCTION(DeclarationName, Class) Z_##DeclarationName##HasFunc<Class>::value
//return pointer to member function as ReturnType(Void::*)(Args), u can cast result to any type
//or use MFP<ReturnType, Args> for convenience, returns null if not exist
//function must be declared with UDECLARE_MEMBERFUNCTION_CHECKING once
#define UCLASS_GET_MEMBERFUNCTION(DeclarationName, Class) Z_##DeclarationName##HasFunc<Class>::GetFunc()

//disable needs to have dll interface
#pragma warning(disable:4251)




namespace UCore
{
	typedef unsigned char byte;

	typedef unsigned char uint8;
	typedef signed char int8;
	typedef unsigned short uint16;
	typedef signed short int16;
	typedef unsigned int uint32;
	typedef int int32;
	typedef unsigned long long uint64;
	typedef long long int64;
	typedef size_t size;

	class Void {};

	using VoidFuncPtr = void(*)();
	using VoidMemFuncPtr = void(Void::*)();

	struct InitDefault {};
	struct InitConfig {};
	struct InitZero {};
	struct NoInit {};

	template<typename T> void SafeDelete(T*& object)
	{
		if (object) delete object;
		object = nullptr;
	}
	template<typename T> void SafeDeleteArray(T*& object)
	{
		if (object) delete[] object;
		object = nullptr;
	}
	template<typename T> void SafeRelease(T*& object)
	{
		if (object) object->Release();
		object = nullptr;
	}


#pragma region function pointer helper
	template <typename TFunction = VoidMemFuncPtr> void* MemFunc2Ptr(TFunction function)
	{
		// 		union
		// 		{
		// 			TFunction f;
		// 			void* p;
		// 		};
		// 		f = function;
		// 		return p;
		return (void*&)function;
	}
	template<typename TFunction = VoidMemFuncPtr> TFunction Ptr2MemFunc(const void* function)
	{
		union
		{
			TFunction f;
			const void* p;
		};
		p = function;
		return f;
	}

#pragma region log

	typedef std::ostringstream StringStreamOut;
	typedef std::istringstream StringStreamIn;

	static const unsigned LogArgPrintableMaxLen = 60;


	inline void SPrintAuto(StringStreamOut& out, const char *format)
	{
		while (*format)
		{
			if (*format == '%')
			{
				throw std::runtime_error("invalid format string: missing arguments");
			}
			out << *format++;
		}
	}
	template<typename T, typename... TArgs> void SPrintAuto(StringStreamOut& out, const char* format, const T& value, TArgs&... args)
	{
		while (*format)
		{
			if (*format == '%')
			{
				out << value;
				format++;
				SPrintAuto(out, format, args...);
				return;
			}
			out << *format++;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	enum  ELogType
	{
		ELT_Message, ELT_Success, ELT_Warn, ELT_Error, ELT_Fatal, ELT_Assert, ELT_Max
	};

	//////////////////////////////////////////////////////////////////////////
	struct LogEntry
	{
		static const unsigned MAX_MESSAGE_LENGTH = 1024;

		ELogType		mType;
		unsigned		mLineNumber;
		unsigned		mThreadID;
		const char*		mFileName;
		const char*		mFunctionName;
		char			mMessage[MAX_MESSAGE_LENGTH];
	};

	//////////////////////////////////////////////////////////////////////////
	class UCORE_API Log
	{
	public:
		static Log* Get();

		void AddVariadic(ELogType type, const char* file, const char* funcName, unsigned line, const char* format, ...);
		void AddRaw(ELogType type, const char* file, const char* funcName, unsigned line, const char* message);

		template<typename... TArgs> void Add(ELogType type, const char* file, const char* funcName, unsigned line, const char* format, const TArgs&... args)
		{
			StringStreamOut streamMessage;
			SPrintAuto(streamMessage, format, args...);
			AddRaw(type, file, funcName, line, streamMessage.str().c_str());
		}

		using LogProcT = void(*)(const LogEntry&);
		void SetListener(LogProcT);


		void				BeginReadLogs();
		const LogEntry*		GetLog(unsigned index);
		unsigned			GetNumLog();
		void				EndReadLogs();
	};
	inline Log* GLog() { return Log::Get(); }

#pragma endregion


	//////////////////////////////////////////////////////////////////////////function pointer
	template<typename T> class TFP {};
	template<typename TRet, typename... TArgs> class TFP<TRet (TArgs...)>
	{
	public:
		using RetT = TRet;
		using PatternT = TRet(*)(TArgs...);
		PatternT mFunction;

		TFP() {}
		TFP(std::nullptr_t) { mFunction = nullptr; }
		template<typename T> TFP(T pFunction) { mFunction = pFunction; }


		TRet operator() (TArgs... args) const
		{
			UASSERT(mFunction);
			return mFunction(args...);
		}
		operator bool() const { return mFunction != nullptr; }
		explicit operator void* () const { return (void*)mFunction; }
		bool operator == (const void* p) const { return mFunction == p; }
		bool operator != (const void* p) const { return mFunction != p; }
		bool operator == (const TFP& fp) const { return mFunction == fp.mFunction; }
		bool operator != (const TFP& fp) const { return mFunction != fp.mFunction; }
	};

	//////////////////////////////////////////////////////////////////////////member function pointer
	template<typename T> class TMFP {};
	template<typename TRet, typename... TArgs> class TMFP<TRet(TArgs...)>
	{
	public:
		using RetT = TRet;
		using PatternT = TRet(Void::*)(TArgs...);

		PatternT mFunction;

		TMFP() {}
		TMFP(std::nullptr_t) { mFunction = nullptr; }

		template <typename T> TMFP(TRet(T::* pFunction)(TArgs...))
		{
			union
			{
				PatternT	a;
				TRet(T::* b)(TArgs...);
			};
			b = pFunction;
			mFunction = a;
		}

		TRet operator() (void* object, TArgs... args) const
		{
			UASSERT(mFunction);
			return (((Void*)object)->*mFunction)(args...);
		}
		operator bool() const { return mFunction != nullptr; }
		explicit operator void* () const { return MemFunc2Ptr(mFunction); }

		bool operator == (const TMFP& other) const { return mFunction == other.mFunction; }
		bool operator != (const TMFP& other) const { return mFunction != other.mFunction; }
	};

#pragma endregion




#pragma  region Flags

	inline bool UFlagTestAny(unsigned flag, unsigned test) { return (flag & test); }
	inline bool UFlagTest(unsigned flag, unsigned test) { return (flag & test) == test; }
	inline bool UFlagTestAndClear(unsigned& flag, unsigned test)
	{
		if ((flag & test) == test)
		{
			flag &= ~test;
			return true;
		}
		return false;
	}
#pragma  endregion

#pragma region

	constexpr bool IsPow2(size_t x)
	{
		return x && ((x & (x - 1)) == 0);
	}
	//round up to next multiple of 'align'. 'align' must be power of 2
	inline size_t NextAlign(size_t numToRound, size_t align)
	{
		UASSERT(IsPow2(align));
		return (numToRound + align - 1) & ~(align - 1);
	}
	inline size_t NextMul(size_t numToRound, size_t multiple)
	{
		UASSERT(multiple);
		return ((numToRound + multiple - 1) / multiple) * multiple;
	}
	inline unsigned NextPow2(unsigned v)
	{
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		v++;
		return v;
	}

	static const float PI = 3.141592653589f;
	static float const RAD2DEG = 180.0f / PI;
	static float const DEG2RAD = PI / 180.0f;
	static float const FLOAT_EPSILON = 0.000001f;

	template<typename T> T Max(T a, T b) { return a > b ? a : b; }
	template<typename T> T Min(T a, T b) { return a < b ? a : b; }

	inline float Clamp(float value, float min, float max) { return value < min ? min : value > max ? max : value; }

	inline float Sqrt(float x) { return sqrtf(x); }
	inline float RSqrt(float x) { return 1.0f / sqrt(x); }

	inline float Sin(float x) { return sinf(x); }
	inline float Cos(float x) { return cosf(x); }
	inline void SinCos(float angleRad, float& outSin, float& outCos)
	{
		outSin = sin(angleRad);
		outCos = cos(angleRad);
	}
	inline void SinCosDeg(float angleDeg, float& outSin, float& outCos)
	{
		outSin = ::sin(angleDeg * DEG2RAD);
		outCos = ::cos(angleDeg * DEG2RAD);
	}
	inline float ACos(float f) { return acos(f); }
	inline float ASin(float f) { return asin(f); }

	inline float Abs(float x) { return x < 0 ? -x : x; }

	inline float Tan(float x)
	{
		return ::tanf(x);
	}
#ifdef UCOMPILER_GCC
	inline float Floor(float f) { return std::floor(f); }
	inline float Ceil(float f) { return std::ceil(f); }
	inline float Round(float f){ return ::round(f); }
	inline bool IsFinite(float f) { return std::isfinite(f); }
	inline bool IsNan(float f) { return std::isnan(f); }
	inline bool IsInf(float f) { return std::isinf(f); }
#else
	inline float Floor(float f) { return std::floor(f); }
	inline float Ceil(float f) { return std::ceil(f); }
	inline float Round(float f) { return std::round(f); }
	inline bool IsFinite(float f) { return std::isfinite(f); }
	inline bool IsNan(float f) { return std::isnan(f); }
	inline bool IsInf(float f) { return std::isinf(f); }
#endif
	
	inline int Truct(float f) { return (int)f; }

	inline float Distance(float x, float y)
	{
		return UCore::Abs(x - y);
	}
	template<typename T> void Swap(T& a, T& b)
	{
		T tmp = a;
		a = b;
		b = tmp;
	}
	inline float Lerp(float v0, float v1, float t)
	{
		return (1 - t) * v0 + t * v1;
	}
	template<typename T> T Sign(T value)
	{
		return value < 0 ? -1 : value > 0 ? 1 : 0;
	}



	inline bool RandBool() { return std::rand() % 2 == 0; }
	inline int RandInt() { return std::rand(); }
	inline float RandFloat01() { return ((float)std::rand()) / RAND_MAX; }
	inline float RandFloat(float min, float max) { return Lerp(min, max, RandFloat01()); }


#pragma endregion
	//////////////////////////////////////////////////////////////////////////
	template < typename T> struct TInstance
	{
		T* mPtr;

		template<typename... TArgs> TInstance(const TArgs... args)
		{
			mPtr = new T(args...);
		}
		explicit TInstance(const TInstance& copy)
		{
			mPtr = new T(copy);
		}
		TInstance& operator = (const TInstance& copy)
		{
			delete mPtr;
			mPtr = new T(copy);
		}
		~TInstance()
		{
			delete mPtr;
			mPtr = nullptr;
		}
		operator T* () const { return mPtr; }
	};

	//e.g TT_IsSpecialization<TArray, TArray<int>>::value === true
	template <class T, template <class...> class Template> struct TT_IsSpecialization : std::false_type {};
	template <template <class...> class Template, class... Args> struct TT_IsSpecialization<Template<Args...>, Template> : std::true_type {};


	////////////////////////////////////////////////////////////////////TT_IsMetaClass
	template <class Type> struct ZZ_Has_ZZIsMeta
	{
		template <typename T, T> struct TypeCheck;
		template <typename T> struct FuncCheck { typedef void (T::*fptr)(); };
		template <typename T> static char HasFunc(TypeCheck< typename FuncCheck<T>::fptr, &T::ZZIsMeta >*);
		template <typename T> static long  HasFunc(...);
		static bool const value = (sizeof(HasFunc<Type>(0)) == sizeof(char));
	};
	//checks whether T is meta class or not
	template<typename T> struct TT_IsMetaClass : std::conditional<std::is_class<T>::value, ZZ_Has_ZZIsMeta<T>, std::false_type>::type
	{
	};


};
