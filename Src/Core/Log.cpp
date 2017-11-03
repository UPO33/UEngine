#include "Log.h"

#include "Log.h"
#include "Threading.h"
#include "String.h"
#include <stdio.h>

#ifdef UPLATFORM_WIN
#include <windows.h>
#endif

#ifdef UPLATFORM_ANDROID
#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "LOG", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "LOG", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"LOG", __VA_ARGS__))

#endif

//helper to show the colorized messages in console
namespace Console
{
#ifdef UPLATFORM_WIN
	const WORD BACKGROUND_COLOR = 0; // black

	namespace Dark
	{
		const WORD BLUE = 1 + BACKGROUND_COLOR * 16;
		const WORD GREEN = 2 + BACKGROUND_COLOR * 16;
		const WORD CYAN = 3 + BACKGROUND_COLOR * 16;
		const WORD RED = 4 + BACKGROUND_COLOR * 16;
		const WORD PINK = 5 + BACKGROUND_COLOR * 16;
		const WORD YELLOW = 6 + BACKGROUND_COLOR * 16;
		const WORD WHITE = 7 + BACKGROUND_COLOR * 16;
		const WORD GRAY = 8 + BACKGROUND_COLOR * 16;
	}

	namespace Bright
	{
		const WORD BLUE = 9 + BACKGROUND_COLOR * 16;
		const WORD GREEN = 10 + BACKGROUND_COLOR * 16;
		const WORD CYAN = 11 + BACKGROUND_COLOR * 16;
		const WORD RED = 12 + BACKGROUND_COLOR * 16;
		const WORD PINK = 13 + BACKGROUND_COLOR * 16;
		const WORD YELLOW = 14 + BACKGROUND_COLOR * 16;
		const WORD WHITE = 15 + BACKGROUND_COLOR * 16;
	}

	void SetColor(int color)
	{

	}
	void MatchColor(UCore::ELogType logType)
	{
		static const int LUTColors[UCore::ELogType::ELT_Max] =
		{
			Console::Dark::WHITE, Console::Bright::GREEN, Console::Bright::YELLOW, Console::Bright::RED, Console::Bright::RED, Console::Bright::RED
		};

		HANDLE  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, LUTColors[logType]);
	}
#else
	void MatchColor(UCore::ELogType logType)
	{
		//#TODO
	}
#endif
};


namespace UCore
{
	//////////////////////////////////////////////////////////////////////////
	class UCORE_API LogImpl : public Log
	{
	public:
		static const unsigned MAX_LOG = 32;

		Lock									mLock;
		LogProcT								mListener = nullptr;
		unsigned								mLogWriteIndex = 0;	//the index that new log should be written at
		bool									mLogsWasFilled = false;
		LogEntry								mLogs[MAX_LOG];

		//////////////////////////////////////////////////////////////////////////
		//this function prints a log to std console, log cat or ...
		static void PrintLog(LogEntry* entry)
		{
#ifdef UPLATFORM_WIN
			Console::MatchColor(entry->mType);
			std::cout << entry->mFunctionName << "  " << entry->mMessage << "\n";
#endif
#ifdef UPLATFORM_ANDROID
			android_LogPriority LUTToAndroidType[] =
			{
				ANDROID_LOG_INFO, ANDROID_LOG_INFO, ANDROID_LOG_WARN, ANDROID_LOG_ERROR, ANDROID_LOG_FATAL, ANDROID_LOG_FATAL
			};
			__android_log_print(LUTToAndroidType[(unsigned)entry->mType], entry->mFunctionName, "%s", entry->mMessage);
#endif
		}

		unsigned GetNumLog() const
		{
			return (mLogsWasFilled ? MAX_LOG : mLogWriteIndex);
		}
		const LogEntry* GetLog(unsigned index) const
		{
			if (mLogsWasFilled)
				return mLogs + ((mLogWriteIndex + index) % MAX_LOG);
			else
				return mLogs + index;
		}
		LogImpl()
		{
		}
		~LogImpl()
		{
		}
		void Add(ELogType type, const char* file, const char* funcName, unsigned line, const char* text)
		{
			USCOPE_LOCK(mLock);


			LogEntry* newEntry = mLogs + mLogWriteIndex;

			mLogWriteIndex++;
			if (mLogWriteIndex >= MAX_LOG) mLogsWasFilled = true;
			mLogWriteIndex = mLogWriteIndex % MAX_LOG;


			newEntry->mType = type;
			newEntry->mLineNumber = line;
			newEntry->mThreadID = Thread::ID();
			newEntry->mFileName = funcName;
			newEntry->mFunctionName = funcName;

			StrCopy(newEntry->mMessage, text);

			//must be removed
			PrintLog(newEntry);

			if(mListener) mListener(*newEntry);

			if (type == ELT_Fatal || type == ELT_Assert)
			{
				//should crash be cause break debugger is after the adding log
				//std::abort();
			}
		}

	};

	//////////////////////////////////////////////////////////////////////////
	Log* Log::Get()
	{
		static TInstance<LogImpl> Ins;
		return Ins;
	}


	//////////////////////////////////////////////////////////////////////////
	void Log::AddVariadic(ELogType type, const char* file, const char* funcName, unsigned line, const char* format, ...)
	{
		char buffer[1024];
		va_list args;
		va_start(args, format);
		vsnprintf(buffer, sizeof(buffer), format, args);
		va_end(args);

		((LogImpl*)this)->Add(type, file, funcName, line, buffer);
	}
	//////////////////////////////////////////////////////////////////////////
	void Log::AddRaw(ELogType type, const char* file, const char* funcName, unsigned line, const char* message)
	{
		((LogImpl*)this)->Add(type, file, funcName, line, message);
	}

	//////////////////////////////////////////////////////////////////////////
	void Log::SetListener(LogProcT function)
	{
		((LogImpl*)this)->mListener = function;
	}
	unsigned Log::GetNumLog()
	{
		return ((LogImpl*)this)->GetNumLog();
	}
	const LogEntry* Log::GetLog(unsigned index)
	{
		return ((LogImpl*)this)->GetLog(index);
	}
	void Log::BeginReadLogs()
	{
		((LogImpl*)this)->mLock.Enter();
	}
	void Log::EndReadLogs()
	{
		((LogImpl*)this)->mLock.Leave();
	}

	UCORE_API StringStreamOut& operator <<(StringStreamOut& s, LogIndent indent)
	{
		for (decltype(indent.mValue) i = 0; i < indent.mValue; i++)
		{
			s << '\t';
		}
		return s;
	}
};
